#ifndef HASH_H
#define HASH_H

#include <cstdlib>
#include <string>
#include <vector>
#include <iostream>

#define HASH_TABLE_SIZE 31

/**
 * @brief A type to represent a hash
 */
typedef uint64_t hash_type;

/**
 * @brief A struct to represent an element in the hash table
 */
struct table_element {
    hash_type hash;
    std::string text;
    table_element *next;
};
/**
 * @brief A global hash table
 */
extern std::vector<table_element*> global_hash_table;

/**
 * @brief Function to dump the hash table
 * 
 * @param s A reference to a string whose has we want to calculate
 * 
 * @return An element of type @ref table_element
 */
hash_type get_string_hash(std::string& s);

/**
 * @brief Function to get the index of a string in the hash table
 * 
 * @param s A reference to a string whose index we want to get
 * @param h A pointer to @ref hash_type to store the hash of the string
 * 
 * @return The index of the string in the hash table
 */
int get_string_index(std::string& s, hash_type *h);

/**
 * @brief Function to add a string to the hash table
 * 
 * @param s A reference to a string to add
 */
void add_string_to_hash_table(std::string& s);

/**
 * @brief Function to check if a string is in the hash table
 * 
 * @param s A reference to a string to check
 * 
 * @return true if the string is in the hash table, false otherwise
 */
bool is_string_in_hash_table(std::string& s);

/**
 * @brief Function to find a string in the hash table
 * 
 * @param s A reference to a string to find
 * 
 * @return A pointer to @ref table_element which contains the string
 */
table_element* find_string_in_hash_table(std::string& s);

/**
 * @brief Function to remove a string from the hash table
 * 
 * @param s A reference to a string to remove
 */
void remove_string_from_hash_table(std::string& s);

#endif