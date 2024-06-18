#ifndef SPPDEBUG_H
#define SPPDEBUG_H

extern char global_file;
void dump_hash_table();

#if defined(_DEBUG) || defined(DEBUG)
#define cerr_debug_print(x) std::cerr << x << std::endl
#else
#define cerr_debug_print(x)
#endif

#endif