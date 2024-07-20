#include "spp.h"
#include "sppdebug.h"
#include "parser.h"
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


static reader_output* getLine(std::ifstream& reader, pstate& stats);
static bool simplify(reader_output* ro);
bool judge_lines(std::ifstream& reader, std::ofstream& writer);


if_stack *if_stack_head = nullptr;
if_stack *if_stack_tail = nullptr;  // Also represents the innermost ifdef-block


static if_stack* create_if_stack(pstate& stat)
{
    if_stack* ifs = new if_stack;
    ifs->line_number = stat.current_line_number;
    ifs->fastforward = false;
    ifs->writeblock = false;
    ifs->next = nullptr;
    ifs->prev = nullptr;
    return ifs;
}

static void push_if_stack(if_stack* ifs)
{

    if (!if_stack_head)
    {
        if_stack_head = ifs;
        if_stack_tail = if_stack_head;
        cerr_debug_print("[push]: head " << if_stack_tail->line_number);
    }
    else
    {
        if_stack* cursor = if_stack_head;
        while (cursor)
        {
            if (!cursor->next)
            {
                cursor->next = ifs;
                ifs->prev = cursor;
                ifs->fastforward = cursor->fastforward;
                if_stack_tail = ifs;
                break;
            }
            cursor = cursor->next;
        }
        cerr_debug_print("[push]: line " << if_stack_tail->line_number);
    }
}

static void pop_if_stack(void)
{
    if (if_stack_tail)
    {
        if_stack* tmp = if_stack_tail;
        if_stack_tail = if_stack_tail->prev;
        if (!if_stack_tail)
        {
            if_stack_head = nullptr;
            cerr_debug_print("[pop] head " << tmp->line_number << std::endl);
        }
        else
            cerr_debug_print("[pop] line " << tmp->line_number << std::endl);
        
        delete tmp;
        return; 
    }
    cerr_debug_print("WARN: nothing to pop" << std::endl);
}

/*
 * Function to get the length of a token
 */
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
/*
 * Simplify an annotated line to mean either true or false

 * @param ro: a pointer to a reader_output struct
 * @return: true, meaning the line is to be written to the output file; false otherwise
 */
static bool simplify(reader_output* ro)
{
    bool ret = false;

    if (!ro)
    {
        cerr_debug_print("[WARN]: pointer ro is null" << std::endl);
        return ret;
    }
    
    ro->line.erase(0,token_len(ro->ltype)); // Strip leading annotation
    
    std::regex activex("^[a-zA-Z0-9]+[ ]+$",std::regex_constants::extended);
    std::smatch matchx;
    if (std::regex_search(ro->line,matchx,activex))
    {
        if (is_string_in_hash_table(ro->line))
            ret = true;
    }
    else if (parse(ro->line))
        ret = true;

    if (ret)
        cerr_debug_print("Eval [True] " << ro->line << std::endl);
    else
        cerr_debug_print("Eval [False] " << ro->line << std::endl);    

    return ret;
}

/*
 * Function to check what type of annotation the line has
 * 
 * @param line: the line to check
 * @param state: the global parser state object
 */
static spp::line_type check_line_type(std::string& line, pstate& state)
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
            std::cerr << "Error: Invalid directive on line " << state.current_line_number << std::endl;
    }
    return spp::line_type::NORMAL;
}

static reader_output* getLine(std::ifstream& reader, pstate& stats)
{
    static reader_output output;
    reader_output* op = &output;
    if (std::getline(reader,output.line))
    {
        /* WARNING: This function MUST be the only place where the line number and opened
           ifdefs are changed, otherwise you risk counting incorrectly */
        stats.current_line_number++;
        output.ltype = check_line_type(output.line, stats);
    }
    else
    {
        output.line = "";
        output.ltype = spp::line_type::FILEEND;
        cerr_debug_print("[WARN] Segfault imminent!" << std::endl);
        op = nullptr;   
    }
    return op;
}


/*
 * This is the main function that reach line of the file and decides whether to write it to the output file
 * or not. It is essentially the heart of spp
*/
bool judge_lines(std::ifstream& reader, std::ofstream& writer)
{
    static pstate stats = {
        0, "", 0, 0, spp::writestate::AWAIT_NONE
    };
    reader_output* ro = nullptr;

    while ((ro = getLine(reader,stats)) != nullptr)
    {
        cerr_debug_print("[line]:" << print_line_type(ro->ltype) << " "
            << ro->line << std::endl);

        if (ro->ltype == spp::line_type::IFDEF)
        {
            if_stack* tmp = create_if_stack(stats);
            push_if_stack(tmp);   // if_stack_tail is tmp after this line

            if (simplify(ro))
            {
                if_stack_tail->writeblock = true;
            }
        }

        if (ro->ltype == spp::line_type::ELIF)
        {
            if (if_stack_tail && !if_stack_tail->fastforward)
            {
                if (if_stack_tail && (if_stack_tail->writeblock == true))
                {
                    if_stack_tail->writeblock = false;
                    if_stack_tail->fastforward = true;
                }
                else
                {
                    if (simplify(ro))
                        if_stack_tail->writeblock = true;
                }
            }
        }

        if (ro->ltype == spp::line_type::ELSE)
        {
            if (if_stack_tail && !if_stack_tail->fastforward)
                if_stack_tail->writeblock = !if_stack_tail->writeblock;
        }

        else if (ro->ltype == spp::line_type::ENDIF)
        {
            pop_if_stack();
        }

        else if (ro->ltype == spp::line_type::NORMAL)
        {
            if (if_stack_tail && if_stack_tail->fastforward)
            {
                cerr_debug_print("[>>]" << std::endl);
                continue;
            }
            if (if_stack_tail && !if_stack_tail->writeblock)
                continue;

            cerr_debug_print("[write]: " << ro->line << std::endl);
            writer << ro->line << std::endl;
        }
    }

    return true;
}


void preprocess_file(char *filename)
{    
    std::ifstream ifile(filename);
    std::string output_filename = std::string(filename) + spp_extension;
    std::ofstream ofile(output_filename);
    bool s = judge_lines(ifile, ofile);
    ifile.close();
    ofile.close();
    if (!s)
    {
        std::cerr << "Error: Unterminated block" << std::endl;
        std::remove(output_filename.c_str());
        std::exit(EXIT_FAILURE);
    }
}
