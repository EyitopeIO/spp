#include "spp.h"
#include <fstream>
#include <string>
#include <cstring>
#include <cstdio>

#define spp_extension ".spp"

#define token_prefix_len 6
#define token_ifdef "@ifdef "
#define token_elif "@elif "
#define token_else "@else "
#define token_endif "@endif"    // Note the absence of space in the string

enum verdict
{
    TRUE,   // an expression is on this line and evaluates to true
    FALSE,  // same as above
    WRITE,  // write this line to the output file
    SKIP    // skip this line
};

static bool evaluate_expression(std::string& expression)
{
    return true;
}

struct parse_state
{
    verdict next_line_verdict;
    size_t opened_ifdefs;
    size_t line_number;
};

static verdict judge_line(std::string& line, parse_state& state)
{

    if (line[0] == '#' && line[1] == '-' && line[2] == '-')
    {
        if (line.find(token_ifdef,2) != std::string::npos)
            state.opened_ifdefs++;
        else if (line.find(token_endif,2) != std::string::npos)
            state.opened_ifdefs--;

        if (evaluate_expression(line))
            return TRUE;
    }
    return WRITE;
}

void preprocess_file(char *filename)
{
    parse_state pstate = {
        .next_line_verdict = WRITE,
        .opened_ifdefs = 0,
        .line_number = 0
    };

    std::ifstream ifile(filename);

    char *output_filename = new char[std::strlen(filename) + std::strlen(spp_extension) + 1];
    std::strcpy(output_filename, filename);
    std::strcat(output_filename, spp_extension);
    std::ofstream ofile(output_filename);

    std::string line;
    while (std::getline(ifile, line))
    {
        switch(judge_line(line, pstate))
        {
            case TRUE:
                pstate.next_line_verdict = WRITE;
                break;
            case FALSE:
                pstate.next_line_verdict = SKIP;
                break;
            case WRITE:
                ofile << line << std::endl;
                break;
            case SKIP:  // also the default case
                break;
        }
        pstate.line_number++;
    }
    ifile.close();
    ofile.close();

    if (pstate.opened_ifdefs != 0)
    {
        std::cerr << "Error: Unterminated ifdefs on line " << pstate.line_number << std::endl;
        std::remove(output_filename);
        /*
         * Even though the program quits and the memory is released, this
         * explicit delete is to point out to the developer that memory
         * must be released in case the logic changes in future.
         */
        delete output_filename;
        std:exit(EXIT_FAILURE);
    }
}