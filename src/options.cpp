#include "options.hpp"
#include <cstring>
#include <cstdlib>

const char* global_files = nullptr;

void show_usage(void)
{
    std::cerr << "Usage: spp [-D<define>]... [files]..." << std::endl;
    exit(EXIT_FAILURE);
}

static void add_defines_to_hashtable(char *text)
{
    std::string s(text);
    add_string_to_hash_table(s);
}

void parse_cmdline_defines(char *text)
{
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
            std::cerr << "Unrecognised " << text[0] << text[1] << " in " <<
                text << std::endl;
    }
}

bool parse_cmdline_files(char *text)
{

    if (!global_files)
        global_files = text;
  
  return true;
}