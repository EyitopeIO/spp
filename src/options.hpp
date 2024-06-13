#ifndef OPTIONS_HPP
#define OPTIONS_HPP

#include "hash.h"

extern const char* global_files;

void show_usage(void);
void parse_cmdline_defines(char *text);
bool parse_cmdline_files(char *text);

#endif