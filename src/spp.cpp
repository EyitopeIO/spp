#include "spp.h"
#include <fstream>
#include <string>
#include <cstring>
#include <cstdio>
#include <memory>
#include <regex>

#define spp_extension ".spp"

#define token_prefix_len 6
#define token_ifdef "@ifdef "
#define token_elif "@elif "
#define token_else "@else"  // Note the absence of space in the string
#define token_endif "@endif"

static bool simplify(std::string& line)
{
    /* Match token with only alphanumerics and without brackets or logical */
    std::regex activex("^[a-zA-Z0-9]+[:space:]*",std::regex_constants::extended);
    std::smatch match;

    return true;
}

static spp::line_type check_line_type(std::string& line, parse_state& state)
{
    if (line[0] == '#' && line[1] == '-' && line[2] == '-')
    {
        if (line.find(token_ifdef,2) != std::string::npos)
            return spp::IFDEF;
        else if (line.find(token_endif,2) != std::string::npos)
            return spp::ENDIF;
        else if (line.find(token_elif,2) != std::string::npos)
            return spp::ELIF;
        else if (line.find(token_else,2) != std::string::npos)
            return spp::ELSE;
        else
            std::cerr << "Error: Invalid directive on line " << state.line_number << std::endl;
    }
    return spp::NORMAL;
}


static spp::verdict judge_line(std::ifstream& reader, std::ofstream& writer,
                               parse_state& pstate,
                               spp::verdict upstream_verdict)
{
    std::string line;
    while (std::getline(reader,line))
    {
        pstate.line_number++;
        spp::line_type ltype = check_line_type(line,pstate);

        /* The recursive case */
        if (ltype == spp::line_type::IFDEF)
        {
            pstate.opened_ifdefs++;
            if (simplify(line))
            {
                judge_line(reader,writer,pstate,spp::verdict::TRUE);
            }
        }

        /* To be defined */
        else if (ltype == spp::line_type::ELSE || ltype == spp::line_type::ELIF)
        {

        }

        /* The terminating case */
        else if (ltype == spp::line_type::ENDIF)
        {
            pstate.opened_ifdefs--;
            return spp::verdict::DONE;
        }

        /* This is our base case. We are in the innermost block following a
         * previous evaluation
         */
        else if (ltype == spp::line_type::NORMAL) {

            /* This is the base case. We are in the innermost ifdef and make
            * our decision to write a line or not */
            writer << line << std::endl;
        }
        else {
            std::cerr << "What sort of line is line " << pstate.line_number << " ?" << std::endl;
            std::exit(EXIT_FAILURE);
        }
    }

    return spp::DONE;
}

void preprocess_file(char *filename)
{
    parse_state pstate = {
        .opened_ifdefs = 0,
        .line_number = 0
    };

    std::ifstream ifile(filename);
    std::unique_ptr<char> output_filename = std::make_unique<char>(std::strlen(filename) + std::strlen(spp_extension) + 1);
    std::strcpy(output_filename.get(), filename);
    std::strcat(output_filename.get(), spp_extension);
    std::ofstream ofile(output_filename.get());

    spp::verdict final_verdict = spp::verdict::WRITE;

    while (final_verdict != spp::verdict::DONE)
        final_verdict = judge_line(ifile,ofile,pstate,final_verdict);

    ifile.close();
    ofile.close();
    if (pstate.opened_ifdefs != 0)
    {
        std::cerr << "Error: Unterminated ifdefs on line " << pstate.line_number << std::endl;
        std::remove(output_filename.get());
        std:exit(EXIT_FAILURE);
    }
}