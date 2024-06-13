#include "debug.h"
#include "hash.h"

#ifdef DEBUG
void dump_hash_table()
{
    table_element *csr;

    for (int i = 0; i < global_hash_table.size(); i++)
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
#else
void dump_hash_table() {}
#endif