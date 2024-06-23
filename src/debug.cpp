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

std::string print_line_type(spp::line_type t)
{
    switch (t)
    {
        case spp::line_type::IFDEF:
            return "IFDEF";
        case spp::line_type::ELIF:
            return "ELIF";
        case spp::line_type::ELSE:
            return "ELSE";
        case spp::line_type::ENDIF:
            return "ENDIF";
        case spp::line_type::NORMAL:
            return "NORMAL";
        default:
            return "UNKNOWN";
    }
}

std::string print_blockstate(spp::writestate b)
{
    switch (b)
    {
        case spp::writestate::AWAIT_NONE:
            return "NONE";
        case spp::writestate::AWAIT_NEXT:
            return "NEXT";
        case spp::writestate::AWAIT_WRITEBLOCK:
            return "WRITEBLOCK";
        case spp::writestate::AWAIT_ENDIF:
            return "AWAIT_ENDIF";
        default:
            return "UNKNOWN";
    }
}

#else
void dump_hash_table() {}
std::string print_line_type(spp::line_type t) { return "";}
std::string print_verdict(spp::verdict v) { return "";}
std::string print_blockstate(spp::writestate b) { return "";}
#endif
