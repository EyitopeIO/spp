#include "sppdebug.h"
#include "hash.h"

#ifdef DEBUG
void dump_hash_table()
{
    for (size_t i = 0; i < global_hash_table.size(); i++)
    {
        std::cout << "----------------" << std::endl;
        std::cout << "Index: " << i << std::endl;
        std::cout << "----------------" << std::endl;
        table_element *elem = global_hash_table[i];
        while (elem)
        {
            std::cout << elem->text << " ";
            elem = elem->next;
        }
        std::cout << std::endl << std::endl;
    }
}

std::string print_line_type(line_type t)
{
    switch (t)
    {
        case line_type::IFDEF:
            return "IFDEF";
        case line_type::ELIF:
            return "ELIF";
        case line_type::ELSE:
            return "ELSE";
        case line_type::ENDIF:
            return "ENDIF";
        case line_type::NORMAL:
            return "NORMAL";
        default:
            return "UNKNOWN";
    }
}


#else
void dump_hash_table() {}
std::string print_line_type(line_type t) { return "";}
#endif
