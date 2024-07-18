#include <iostream>
#include <fstream>
#include "options.hpp"
#include "sppdebug.h"


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
    char **argv_local = argv + 1;
    int argc_local = argc - 1;
    
    while (argc_local--)
        judge_cmdline(argv_local++);
    // dump_hash_table();
    return 0;
}
