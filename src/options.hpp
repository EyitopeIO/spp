#ifndef OPTIONS_HPP
#define OPTIONS_HPP

#include "hash.h"

extern char** first_file;

void show_usage(void);
void parse_cmdline_defines(char *text);
void parse_cmdline_files(char **text);

#endif