#include "spp.h"
#include "sppdebug.h"
#include <fstream>
#include <string>
#include <cstring>
#include <cstdio>
#include <memory>
#include <regex>

#define spp_extension ".spp"

#define token_prefix_len 5
#define token_ifdef "@ifdef "
#define token_elif "@elif "
#define token_else "@else"  // Note the absence of space in the string
#define token_endif "@endif"

/*
 * We use this to track the nested ifdef blocks. We could recursively call
 * `judge_line` for each ifdef block, but for a large nest, we don't want
 * to bother increasing the stack size allowed to a process.
*/
std::stack<judge_ruling> ruling_stack;


static int token_len(spp::line_type type)
{
    switch (type)
    {
        case spp::IFDEF:
            return token_prefix_len + std::strlen(token_ifdef) - 1;
        case spp::ELIF:
            return token_prefix_len + std::strlen(token_elif) - 1;
        case spp::ELSE:
            return token_prefix_len + std::strlen(token_else) - 1;
        case spp::ENDIF:
            return token_prefix_len + std::strlen(token_endif) - 1;
        default:
            return -1;
    }
}

static bool simplify(std::string& line, spp::line_type ltype)
{
    line.erase(0,token_len(ltype));

    /* Match token with only alphanumerics and without brackets or logical */
    std::regex activex("^[a-zA-Z0-9]+",std::regex_constants::extended);
    std::smatch matchx;
    if (std::regex_search(line,matchx,activex))
    {
        if (is_string_in_hash_table(line))
        {
            cerr_debug_print("Eval [True] " << line << std::endl);
            return true;
        }
    }
    cerr_debug_print("Eval [False] " << line << std::endl);
    return false;
}

static spp::line_type check_line_type(std::string& line, parse_state& state)
{
    if (line[0] == '#' && line[1] == '-' && line[2] == '-')
    {
        if (line.find(token_ifdef,2) != std::string::npos)
            return spp::line_type::IFDEF;
        else if (line.find(token_endif,2) != std::string::npos)
            return spp::line_type::ENDIF;
        else if (line.find(token_elif,2) != std::string::npos)
            return spp::line_type::ELIF;
        else if (line.find(token_else,2) != std::string::npos)
            return spp::line_type::ELSE;
        else
            std::cerr << "Error: Invalid directive on line " << state.line_number << std::endl;
    }
    return spp::line_type::NORMAL;
}

static spp::verdict judge_line(std::ifstream& reader, std::ofstream& writer,
                               parse_state& pstate, spp::verdict upstream_verdict)
{
    /*
        This will depict what this function is doing. Each vertical "slash"
        represents a line. We read each line of the file once from top to
        bottom. We do not want to pass through the file twice or generate any
        parse tree or whatnot.

        The important thing to note is that the reader and writer file stream
        always refers to the same location in the file, no matter how deep the
        recursive call stack may be. Each call uses the same reader and writer
        reference. So from the view of the outermost ifdef, we enter the
        recurse at [A] and exit to [B]. When the recursive call to `judge_line`
        returns back to outermost ifdef block, the writer would be at point B,
        even though outermost loop would hav only done 3 or so iterations. Just
        count the vertical slashes to see what I mean.
    ______________________________________________________
    | <first line of file here>
    |
    |
    |   <-- ifdef
    |            [A]|   <-- ifdef
    |               |
    |               |
    |               |               |   <-- ifdef
    |               |               |
    |               |               |
    |               |               |   <-- elif
    |               |               |
    |               |               |   <-- else
    |               |               |
    |               |               |   <-- endif
    |            [B]|   <-- endif
    |               |
    |               |
    |  <-- endif
    |
    |
    | <last line of file here>
    |____________________________________________________
    */

    std::string line;
    spp::line_type ltype;
    spp::blockwrite el_state = spp::blockwrite::ELIF_STOP;
    spp::verdict local_verdict = upstream_verdict;

    while (std::getline(reader,line))
    {
        pstate.line_number++;

        ltype = check_line_type(line,pstate);
        cerr_debug_print("Line [read] " << pstate.line_number << ": type " << print_line_type(ltype)
            << " upverdict " << print_verdict(upstream_verdict) << " " << line << std::endl);

        if (ltype == spp::line_type::IFDEF)
        {
            pstate.opened_ifdefs++;

            spp::verdict v;            
            if (simplify(line,ltype))
                v = spp::verdict::WRITE;
            else
                v = spp::verdict::SKIP;

            /* As explained above, when this function returns, its writer would
             * be at the line just after the ifdef block that caused the recursion */  
            spp::verdict rv = judge_line(reader, writer, pstate, v);

            cerr_debug_print("Line [judge] " << pstate.line_number << " type " << print_line_type(ltype) << ": " 
                << " retverdict " << print_verdict(rv) << " " << line << std::endl);

            continue;
        }

        else if (ltype == spp::line_type::ELIF)
        {
            if (upstream_verdict == spp::verdict::SKIP) // upstream ifdef
            {
                if (el_state == spp::blockwrite::ELIF_STOP && simplify(line,ltype))
                {
                    switch (el_state)
                    {
                        case spp::blockwrite::ELIF_STOP:    // We just saw the first elif
                            el_state = spp::blockwrite::ELIF_WRITE;
                            local_verdict = spp::verdict::WRITE;
                            break;
                        case spp::blockwrite::ELIF_WRITE:   // Next elif after previous we'd just seen
                            el_state = spp::blockwrite::ELIF_SKIP;
                            local_verdict = spp::verdict::SKIP;
                            break;
                    }
                }
            }
            else    // upstream ifdef was true, so ignore elifs
            {
                el_state = spp::blockwrite::ELIF_SKIP;
                local_verdict = spp::verdict::SKIP;
            }

            cerr_debug_print("Line [block] " << pstate.line_number << " "
                << print_blockwrite(el_state) << std::endl);
        }

        else if (ltype == spp::line_type::ELSE)
        {
            /* If the elif that evaluated to true was the one just before the else,
             * then obviously the else singnifies the last line of the previous elif
             * block.
             */
            if (el_state == spp::blockwrite::ELIF_WRITE)
                el_state = spp::blockwrite::ELIF_SKIP;

            /* One ifdef and one else*/
            if (upstream_verdict == spp::verdict::WRITE || el_state == spp::blockwrite::ELIF_SKIP)
                local_verdict = spp::verdict::SKIP;

            /* One ifdef and at least one elif and neither the ifdef nor any of the elifs evaluated
             * to true 
             */
            else if (el_state == spp::blockwrite::ELIF_STOP && upstream_verdict == spp::verdict::SKIP)
                local_verdict = spp::verdict::WRITE;

            cerr_debug_print("Line [else] " << pstate.line_number << " lo_verdict " << print_verdict(local_verdict) 
               << " blockstate " << print_blockwrite(el_state) << std::endl);
        }

        /* Terminating case. We return from the innermost ifdef-endif block.
         * The caller of the recursive function can now continue writing the
         * line just after the block that caused it to recurse. The outermost
         * ifdef block does not need any any context of whatever happened in
         * the inner blocks.
         */
        else if (ltype == spp::line_type::ENDIF)
        {
            pstate.opened_ifdefs--;
            el_state = spp::blockwrite::ELIF_STOP;
            return spp::verdict::SKIP;
        }

        if (ltype == spp::line_type::NORMAL)
        {
            if (local_verdict == spp::verdict::WRITE)
            {
                writer << line << std::endl;
                cerr_debug_print("Line [write] " << pstate.line_number << ": " << line << std::endl);
                continue;
            }
            cerr_debug_print("Line [skip] " << pstate.line_number << ": " << line << std::endl);
        }
    }
    return spp::verdict::DONE;
}

void preprocess_file(char *filename)
{
    parse_state pstate = {
        .opened_ifdefs = 0,
        .line_number = 0,
    };

    std::ifstream ifile(filename);
    std::string output_filename = std::string(filename) + spp_extension;
    std::ofstream ofile(output_filename);

    while (judge_line(ifile,ofile,pstate,spp::verdict::WRITE) != spp::verdict::DONE)

    ifile.close();
    ofile.close();
    if (pstate.opened_ifdefs != 0)
    {
        std::cerr << "Error: Unterminated ifdefs on line " << pstate.line_number << std::endl;
        std::remove(output_filename.c_str());
        std::exit(EXIT_FAILURE);
    }
}
