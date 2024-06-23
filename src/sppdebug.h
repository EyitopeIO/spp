#ifndef SPPDEBUG_H
#define SPPDEBUG_H

#include "spp.h"

extern char global_file;
void dump_hash_table();
std::string print_line_type(spp::line_type t);
std::string print_verdict(spp::verdict v);
std::string print_blockstate(spp::writestate b);

#if defined(_DEBUG) || defined(DEBUG)
#define cerr_debug_print(x) std::cerr << __func__ << ": " << x << std::endl
#else
#define cerr_debug_print(x)
#endif

#endif