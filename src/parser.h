#ifndef PARSER_H
#define PARSER_H

#include <string>
/* 
 * Function to parse a complex annotation
 * 
 * @param input: Annotated line read from a file but with annotation stripped.
 * @return: true if annotation evaluates to true; false otherwise
*/
bool parse(const std::string& input);

#endif