#ifndef LEXER_H
#define LEXER_H

#include "tokens.h"

void init_lexer(const char *source_code);
Token get_next_token();

#endif