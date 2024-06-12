#include "hash.h"


std::vector<table_element*> global_hash_table(256);


/* Hash function shamelessly copied from 
 * https://cseweb.ucsd.edu/~kube/cls/100/Lectures/lec16/lec16-16.html#pgfId-977548
 */
size_t get_string_hash(std::string& s)
{
    size_t hashval = 0;
    for (auto c: s)
    {
        hashval = (hashval << 4) + c;
        size_t g = hashval & 0xF0000000;
        if (g != 0)
            hashval ^= g >> 24;
        hashval &= ~g;
    }
    return hashval;
}

int get_string_index(std::string& s, size_t *h)
{
    size_t tmp = get_string_hash(s);
    if (h)
        *h = tmp;
    return tmp % 256;
}

table_element* find_string_in_hash_table(std::string& s)
{
    size_t h;
    int index = get_string_index(s, &h);
    table_element* current = global_hash_table[index];
    while (current)
    {
        if (current->hash == h)
            return current;
        current = current->next;
    }
    return nullptr;
}

void add_string_to_hash_table(std::string& s)
{
    size_t hash = get_string_hash(s);
    int index = get_string_index(s, nullptr);
    
    table_element* elem = new table_element();
    elem->hash = hash;
    elem->text = s;
    elem->next = nullptr;

    table_element* current = global_hash_table[index];
    if (!current)
    {
        global_hash_table[index] = elem;
        return;
    }
    while (current)
    {
        if (!current->next)
            break;
        current = current->next;
    }
    current->next = elem;
}

void remove_string_from_hash_table(std::string& s)
{
    size_t hash;
    int index = get_string_index(s, &hash);
    table_element* prev = global_hash_table[index];
    table_element* current = prev->next;

    while (current)
    {
        if (current->hash == hash)
        {
            prev->next = current->next;
            delete current;
            return;
        }
        prev = current;
        current = current->next;
    }
}