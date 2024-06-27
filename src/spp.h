#ifndef SPP_H
#define SPP_H

#include "hash.h"

void preprocess_file(char *filename);

namespace spp
{
    enum verdict
    {
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
        FILEEND
    };

    enum writestate
    {
        AWAIT_NONE,
        AWAIT_NEXT,
        AWAIT_WRITEBLOCK,
        AWAIT_ENDIF
    };
}

struct pstate
{
    ssize_t opened_ifdefs;
    std::string ifdef_line;
    size_t current_line_number;
    size_t recursive_depth;
    spp::writestate wstate;
};

struct reader_output
{
    std::string line;
    spp::line_type ltype;
};


#endif // SPP_H