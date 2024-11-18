#ifndef OPTIONS_HPP
#define OPTIONS_HPP

#include "hash.h"

/**
 * @brief Points to the first file in the list of files from the command line
 *  
 * @details If this null, then we are still processing definitions. Files should come last. 
 */
extern char** first_file;

/**
 * @brief Function to show the usage of the program
 */
void show_usage(void);

/**
 * @brief Function to parse definitions provided on the command line
 * 
 * @param text a single definition i.e. -DNAME
 */
void parse_cmdline_defines(char *text);

/**
 * @brief Function to parse files provided on the command line
 *
 * @details File to parse come after all definitions. So when the first file is
 *          encountered, we assume all subsequent arguments are files to parse.
 * 
 * @param text: a single file name
 */
void parse_cmdline_files(char **text);

#endif