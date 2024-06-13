#include <iostream>
#include "options.hpp"

#include <getopt.h>


int main (int argc, char **argv)
{
    char **argv_local = argv + 1;
    int argc_local = argc - 1;
    
    while (argc_local--)
    {
        std::cout << *argv_local << std::endl;
        add_defines_to_hashtable(*argv_local++);
    }

    return 0;
}
