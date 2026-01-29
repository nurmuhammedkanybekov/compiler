#ifndef PARSER_H
#define PARSER_H

#include "ast.h"

void init_parser(const char* code);
ASTNode* parse_statement();

#endif