#ifndef SPP_H
#define SPP_H

#include "hash.h"

/**
 * @brief Function to preprocess a file
 * 
 * @param filename The name of the file to preprocess as given on the command line
 */
void preprocess_file(char *filename);

/**
 * @brief Represents what type of annotation is in the line we read from the file
 */
enum line_type
{
    IFDEF,
    ELIF,
    ELSE,
    ENDIF,
    NORMAL,
    FILEEND
};

/**
 * @brief Represents the state of the preprocessor
 */
struct pstate
{
    size_t current_line_number;
};

/**
 * @brief Output of the @ref getLine function. It represent a line of the input file.
 */
struct reader_output
{
    std::string line;
    line_type ltype;
};

/** 
 * @brief "if" stack i.e. stack of #if/#ifdef/#ifndef/#elif/#else/#endif
 * 
 * @details See struct with same name in gcc source code, which inspired this implementation.
*/
struct if_stack
{
    struct if_stack *next;
    struct if_stack *prev;
    size_t line_number; /**< Line where condition started.  */
    bool fastforward;   /**< Can future #else / #elif be skipped?  */
    bool writeblock;    /**< Should the lines be written? */
};

#endif // SPP_H