#ifndef SPP_H
#define SPP_H

#include "hash.h"

void preprocess_file(char *filename);

namespace spp
{
    enum verdict
    {
        TRUE,   // an expression is on this line and evaluates to true
        FALSE,  // same as above
        WRITE,  // write this line to the output file
        SKIP,    // skip this line
        DONE    // done processing the file
    };

    enum line_type
    {
        IFDEF,
        ELIF,
        ELSE,
        ENDIF,
        NORMAL,
    };
}

struct parse_state
{
    size_t opened_ifdefs;
    size_t line_number;
};


#endif // SPP_H