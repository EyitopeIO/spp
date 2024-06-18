#ifndef HASH_H
#define HASH_H

#include <cstdlib>
#include <string>
#include <vector>
#include <iostream>

#define HASH_TABLE_SIZE 31

typedef uint64_t hash_type;

struct table_element {
    hash_type hash;
    std::string text;
    table_element *next;
};

extern std::vector<table_element*> global_hash_table;

/*
 * Function to get the hash of a string
 * @param s: the string to hash
 * @return: the hash of the string
 */
hash_type get_string_hash(std::string& s);

/*
 * Function to get the index of a string in the hash table
 * @param s: the string to hash
 * @param h: a pointer to a size_t to store the hash of the string
 * @return: the index of the string in the hash table
 */
int get_string_index(std::string& s, hash_type *h);

/*
 * Function to add a string to the hash table
 * @param s: the string to add
 */
void add_string_to_hash_table(std::string& s);

/*
 * Function to check if a string is in the hash table
 * @param s: the string to check
 * @return: true if the string is in the hash table, false otherwise
 */
bool is_string_in_hash_table(std::string& s);


/*
 * Function to find a string in the hash table
 * @param s: the string to find
 * @return: a pointer to the table_element containing the string
 */
table_element* find_string_in_hash_table(std::string& s);

/*
 * Function to remove a string from the hash table
 * @param s: the string to remove
 */
void remove_string_from_hash_table(std::string& s);

#endif