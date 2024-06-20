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


static judge_ruling judge_line(std::ifstream& reader, std::ofstream& writer,
                               parse_state& pstate)
{


    /*
        This will depict what this function is doing. Each vertical "slash"
        represents a line. We read each line of the file once from top to
        bottom. We do not want to pass through the file twice or generate any
        parse tree or whatnot.

        The important thing to note is that the reader and writer file stream
        always refers to the same location in the file, no matter how deep the
        recursive call stack may be. So from the view of the outermost (i.e.
        letter A in square brackets) ifdef, when the recursive call to
        `judge_line` returns, the writer would be at point B, even though it
        has only gone through the loop once i.e. when the writer was at A.


    ______________________________________________________
    | <first line of file here>
    |
    |
    |   <-- ifdef [A]
    |
    |
    |               |   <-- ifdef
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
    |               |   <-- endif
    |
    |
    |  <-- endif [B]
    |
    |
    | <last line of file here>
    |____________________________________________________
    */


    judge_ruling j = {
        .v = spp::verdict::WRITE,
        .l = spp::line_type::NORMAL,
        .see_next_block = false
    };

    std::string line;
    while (std::getline(reader,line))
    {
        pstate.line_number++;
        j.l = check_line_type(line,pstate);
        cerr_debug_print("Line [A] " << pstate.line_number << ": type " << j.l << ": " << line << std::endl);

        /* The recursive case */
        if (j.l == spp::line_type::IFDEF)
        {
            pstate.opened_ifdefs++;

            if (simplify(line, j.l))
            {
                /* Return value ignored because as discussed above, the recursive
                   calls use the same reference to the reader and writer file, so
                   decisions to read or write would have been made, and there is
                   no need to overwrite what the previous call thought its line to
                   be */
                judge_line(reader,writer,pstate);

                j.see_next_block = false;
            }
            else
            {
                j.v = spp::verdict::SKIP;
                j.see_next_block = true;
            }
            cerr_debug_print("Line [B] " << pstate.line_number << ": verdict " << j.v << ": " << line << std::endl);
        }

        /* Beware that in the else-if and else blocks, setting the verdict to
         * SKIP is not logically equivalent to returning to the next iteration
         * (i.e. `continue' statement) of the main loop.
         */
        else if (j.l == spp::line_type::ELIF)
        {
            if (j.see_next_block && simplify(line, j.l))
            {
                j.see_next_block = false;
                j.v = spp::verdict::WRITE;
            }
            else {
                j.v = spp::verdict::SKIP;
            }
        }

        else if ( j.l == spp::line_type::ELSE)
        {
            if (j.see_next_block)
            {
                j.see_next_block = false;
                j.v = spp::verdict::WRITE;
            }
            else {
                j.v = spp::verdict::SKIP;
            }
        }

        /* The terminating case i.e. we have reached the innermost endif */
        else if (j.l == spp::line_type::ENDIF)
        {
            pstate.opened_ifdefs--;
            j.v = spp::verdict::SKIP;
            j.see_next_block = false;
            return j;
        }

        else {
            j.l = spp::line_type::NORMAL;
        }

        cerr_debug_print("Line [C] " << pstate.line_number << ": verdict " << j.v << ": " << line << std::endl);

        /* This is the base case. We are in the innermost ifdef and make
         * our decision to write a line or not. Online normal lines should be
         * written anyway
         */
        if (j.v == spp::verdict::WRITE && j.l == spp::line_type::NORMAL)
            writer << line << std::endl;
    }
    j.v = spp::verdict::DONE;
    return j;
}

void preprocess_file(char *filename)
{
    parse_state pstate = {
        .opened_ifdefs = 0,
        .line_number = 0
    };

    std::ifstream ifile(filename);
    std::string output_filename = std::string(filename) + spp_extension;
    std::ofstream ofile(output_filename);

    judge_ruling final_verdict = {
        spp::verdict::WRITE,
        spp::line_type::NORMAL
    };

    while (final_verdict.v != spp::verdict::DONE)
        final_verdict = judge_line(ifile,ofile,pstate);

    ifile.close();
    ofile.close();
    if (pstate.opened_ifdefs != 0)
    {
        std::cerr << "Error: Unterminated ifdefs on line " << pstate.line_number << std::endl;
        std::remove(output_filename.c_str());
        std::exit(EXIT_FAILURE);
    }
}
