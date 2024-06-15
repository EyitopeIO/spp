#include "spp.h"
#include <fstream>
#include <string>


enum verdict
{
    TRUE,   // an expression is on this line and evaluates to true
    FALSE,  // same as above
    WRITE,  // write this line to the output file
    SKIP    // skip this line
};


struct parse_state
{
    verdict v_for_next_line;
};


static verdict judge_line(std::string line)
{
    return WRITE;
}


void preprocess_file(char *filename)
{
    parse_state pstate;
    std::ifstream file(filename);
    std::string line;
    while (std::getline(file, line))
    {
        switch(judge_line(line))
        {
            case TRUE:
                break;
            case FALSE:
                break;
            case WRITE:
                std::cout << line << std::endl;
                break;
            case SKIP:  
                break;
        }
    }
}