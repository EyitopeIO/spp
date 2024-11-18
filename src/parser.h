#ifndef PARSER_H
#define PARSER_H

#include <string>
/** 
 * @brief Function to parse an annotation
 * 
 * @details An annotation is a shell comment in the form of a directive that spp recognizes as a command.
 *          The annotation will then be evaluated with respect to the current state of the parser.
 *          
 * @param input Annotated line read from a file but with annotation stripped.
 * 
 * @return true if annotation evaluates to true; false otherwise
 */
bool parse(const std::string& input);

#endif