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

static reader_output* getLine(std::ifstream& reader, pstate& stats);
bool judge_lines(std::ifstream& reader, std::ofstream& writer);


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

static bool simplify(reader_output* ro)
{
    if (!ro)
    {
        cerr_debug_print("[WARN]: pointer ro is null" << std::endl);
        return false;
    }
    
    ro->line.erase(0,token_len(ro->ltype));

    /* Match token with only alphanumerics and without brackets or logical */
    std::regex activex("^[a-zA-Z0-9]+",std::regex_constants::extended);
    std::smatch matchx;
    if (std::regex_search(ro->line,matchx,activex))
    {
        if (is_string_in_hash_table(ro->line))
        {
            cerr_debug_print("Eval [True] " << ro->line << std::endl);
            return true;
        }
    }
    cerr_debug_print("Eval [False] " << ro->line << std::endl);
    return false;
}

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

/* Function to find the terminating endif of the the block we're examining */
static spp::line_type fastforward_block(std::ifstream& reader, pstate& stats)
{
    cerr_debug_print("[>>] " << std::endl);
    spp::line_type l = spp::line_type::FILEEND;

    reader_output* ro = nullptr;
    while ((ro = getLine(reader,stats)) != nullptr)
    {
        if (ro->ltype == spp::line_type::ENDIF)
        {
            if (stats.opened_ifdefs == 0) {
                stats.wstate = spp::writestate::AWAIT_NONE;
                l = ro->ltype;
                break;
            }
        }
    }
    cerr_debug_print("[>] " << std::endl);
    return l;
}

static reader_output* getLine(std::ifstream& reader, pstate& stats)
{
    static reader_output output;
    reader_output* op = &output;
    if (std::getline(reader,output.line))
    {
        /* This function MUST be the only place where the line number and opened
           ifdefs are changed, otherwise you risk counting incorrectly */
        stats.current_line_number++;
        output.ltype = check_line_type(output.line, stats);
        switch (output.ltype)
        {
            case spp::line_type::IFDEF:
                stats.opened_ifdefs++;
                break;
            case spp::line_type::ENDIF:
                stats.opened_ifdefs--;
            default:
                break;
        }
    }
    else
    {
        output.line = "";
        output.ltype = spp::line_type::FILEEND;
        cerr_debug_print("[WARN] Segfault imminent!" << std::endl);
        op = nullptr;   
    }
    cerr_debug_print(" oif: " << stats.opened_ifdefs << " [line] " << output.line << std::endl);
    return op;
}

static void write_block(std::ifstream& reader, std::ofstream& writer, pstate& stats)
{
    reader_output* ro = nullptr;    
    while ((ro = getLine(reader,stats)) != nullptr)
    {
        if ( ro->ltype == spp::line_type::NORMAL)
        {
            cerr_debug_print(ro->line << std::endl);
            writer << ro->line << std::endl;
        }
        else
        {
            if (ro->ltype == spp::line_type::ENDIF)
                return;
            else
            {
                fastforward_block(reader,stats);
                return;
            }
        }
    } 
    std::cerr << "Unterminated block. line: " << stats.ifdef_line << std::endl;
    std::exit(EXIT_FAILURE);
}

bool judge_lines(std::ifstream& reader, std::ofstream& writer)
{
    static pstate stats = {
        .opened_ifdefs = 0,
        .ifdef_line = "",
        .current_line_number = 0,
        .recursive_depth = 0,
        .wstate = spp::writestate::AWAIT_NONE
    };
    reader_output* ro = nullptr;
   
    while ((ro = getLine(reader,stats)) != nullptr)
    {        
        if (ro->ltype == spp::line_type::IFDEF)
        {
            if (simplify(ro))
            {
                stats.ifdef_line = ro->line;
                write_block(reader,writer,stats);
            }
            else
            {
                stats.wstate = spp::writestate::AWAIT_NEXT;
            }
        }
        
        else if (ro->ltype == spp::line_type::ELIF)
        {
            if ((stats.wstate = spp::writestate::AWAIT_NEXT) && simplify(ro))
            {
                write_block(reader,writer,stats);
            }
        }
        
        else if (ro->ltype == spp::line_type::ELSE)
        {
            if ((stats.wstate = spp::writestate::AWAIT_NEXT))
            {
                write_block(reader,writer,stats);
            }
	    stats.wstate = spp::writestate::AWAIT_NONE;
        }
        
        else if (ro->ltype == spp::line_type::ENDIF)
        {
            stats.wstate = spp::writestate::AWAIT_NONE;
        }
        
        
        else if (ro->ltype == spp::line_type::NORMAL)
        {
            switch(stats.wstate)
            {
                case spp::AWAIT_NEXT:
                    continue;
                    
                default:            
                    cerr_debug_print("[write]: " << ro->line << std::endl);
                    writer << ro->line << std::endl;
                    break;
            }
        }
        
        else if (ro->ltype == spp::line_type::FILEEND)
        {
            break;
        }
        
        else
        {
            std::cerr << "Confusing line: " << ro->line << std::endl;
            std::exit(EXIT_FAILURE);
        }        
    }
    
    if (stats.opened_ifdefs != 0)
        return false;
        
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
