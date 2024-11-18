#include <iostream>
#include <fstream>
#include "options.hpp"
#include "sppdebug.h"

/**
 * @brief Function to parse definitions provided on the command line
 * @param text: a single definition i.e. -DNAME
 */
static inline void judge_cmdline(char **cmd_line_text)
{
    /* Likely a definition */
    if (*cmd_line_text[0] == '-' )
    {
        /* Quit to respect command line order. Files should come last */
        if (first_file)
            show_usage();

        parse_cmdline_defines(*cmd_line_text);
        return;
    }
    parse_cmdline_files(cmd_line_text);
}


int main (int argc, char **argv)
{
    char **argv_local = argv + 1;   // Skip the first argument which is the program name   
    int argc_local = argc - 1;

    setvbuf(stdout, nullptr, _IONBF, 0);    // Disable buffering for stdout
    
    while (argc_local--)
        judge_cmdline(argv_local++);
    // dump_hash_table();
    return 0;
}
