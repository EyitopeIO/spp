#ifndef SPP_H
#define SPP_H

#include "hash.h"

void preprocess_file(char *filename);

enum line_type
{
    IFDEF,
    ELIF,
    ELSE,
    ENDIF,
    NORMAL,
    FILEEND
};

struct pstate
{
    size_t current_line_number;
};

/**
 * Output of the \ref getLine function. It represent a line of the input file.
 */
struct reader_output
{
    std::string line;
    line_type ltype;
};

/** 
 * Stack of conditionals currently in progress. It includes both failed and
 * successful conditionals. See struct with same name in gcc.
*/
struct if_stack
{
    struct if_stack *next;
    struct if_stack *prev;
    size_t line_number; /* Line where condition started.  */
    bool fastforward;   /* Can future #else / #elif be skipped?  */
    bool writeblock;    /* Should the lines be written? */
};

#endif // SPP_H