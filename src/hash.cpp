#include "hash.h"


std::vector<table_element*> global_hash_table(HASH_TABLE_SIZE);


/* Hash function shamelessly copied from 
 * https://cseweb.ucsd.edu/~kube/cls/100/Lectures/lec16/lec16-16.html#pgfId-977548
 */
hash_type get_string_hash(std::string& s)
{
    hash_type hashval = 0;
    for (auto c: s)
    {
        hashval = (hashval << 4) + c;
        hash_type g = hashval & 0xF0000000;
        if (g != 0)
            hashval ^= g >> 24;
        hashval &= ~g;
    }
    return hashval;
}

int get_string_index(std::string& s, hash_type *h)
{
    hash_type tmp = get_string_hash(s);
    if (h)
        *h = tmp;
    return tmp % HASH_TABLE_SIZE;
}

table_element* find_string_in_hash_table(std::string& s)
{
    hash_type h;
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
    hash_type hash = get_string_hash(s);
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
    hash_type hash;
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

bool is_string_in_hash_table(std::string& s)
{
    return find_string_in_hash_table(s) != nullptr;
}