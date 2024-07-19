#ifndef OPTIONS_HPP
#define OPTIONS_HPP

#include "hash.h"

/*
 * Variable that points to the first file to parse specified on the command line
 * If we ever need to revisit files again, we'd simply begin iterating from here.
*/
extern char** first_file;

void show_usage(void);

/*
 * Function to parse definitions provided on the command line
 * @param text: a single definition i.e. -DNAME
 */
void parse_cmdline_defines(char *text);

/*
 * Function to parse files provided on the command line
 *
 * File to parse come after all definitions. So when the first file  is
 * encountered, we assume all subswquent arguments are files to parse.
 * 
 * @param text: a single file name
 */
void parse_cmdline_files(char **text);

#endif