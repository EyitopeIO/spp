#include <cstdlib>
#include <string>
#include <vector>
#include <iostream>

struct table_element {
    size_t hash;
    std::string text;
    table_element *next;
};

extern std::vector<table_element*> global_hash_table;

/*
 * Function to get the hash of a string
 * @param s: the string to hash
 * @return: the hash of the string
 */
size_t get_string_hash(std::string& s);

/*
 * Function to get the index of a string in the hash table
 * @param s: the string to hash
 * @param h: a pointer to a size_t to store the hash of the string
 * @return: the index of the string in the hash table
 */
int get_string_index(std::string& s, size_t *h);

/*
 * Function to add a string to the hash table
 * @param s: the string to add
 */
void add_string_to_hash_table(std::string& s);

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