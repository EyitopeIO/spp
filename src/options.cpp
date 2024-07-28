#include "options.hpp"
#include "spp.h"
#include <cstring>
#include <cstdlib>

#include <unistd.h>

#define INVALIDCMDLINE(text) do { \
    std::cerr << "Invalid definition: " << text << std::endl << std::endl;\
    show_usage();\
    } while (0);

char** first_file = nullptr;

void show_usage(void)
{
    std::cerr << "Usage: spp [-D<define>] [files]" << std::endl;
    exit(EXIT_FAILURE);
}

static void add_defines_to_hashtable(char *text)
{
    std::string s(text);
    add_string_to_hash_table(s);
}

void parse_cmdline_defines(char *text)
{
    int ln = std::strlen(text);
    if (ln <= 2)
        INVALIDCMDLINE(text);

    switch(text[1])
    {
        case 'D':
        {
            if (std::strrchr(text, '='))
            {
                std::cerr << "Ignoring `" << text << "' with `='" << std::endl;
                return;
            }
            add_defines_to_hashtable(text + 2);
            break;
        }
        default:
            INVALIDCMDLINE(text);
    }
}

void parse_cmdline_files(char **text)
{
    if (!first_file) // first file in the list from command line
        first_file = text;

    if (access(*text, R_OK) == -1)
    {
        switch(errno)
        {
            case EACCES:
                std::cerr << "Read permissions needed for file " << *text << std::endl;
                break;
            case ENOENT:
                std::cerr << "File " << *text << " does not exist" << std::endl;
                break;
            default:
                std::cerr << "File " << *text << " cannot be read" << std::endl;
        }
        exit(EXIT_FAILURE);
    }
    preprocess_file(*text);
}