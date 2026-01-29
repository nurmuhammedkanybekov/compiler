#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "tokens.h"

const char *code;
int pos = 0;

void init_lexer(const char *source_code) {
    code = source_code;
    pos = 0;
}

Token make_token(TokenType type, const char *text) {
    Token t;
    t.type = type;
    strcpy(t.text, text);
    return t;
}

Token get_next_token() {
    while (isspace(code[pos])) {
        pos++;
    }

    if (code[pos] == '\0') return make_token(TOKEN_EOF, "EOF");

    if (isdigit(code[pos])) {
        char buffer[64];
        int i = 0;
        while (isdigit(code[pos])) {
            buffer[i++] = code[pos++];
        }
        buffer[i] = '\0';
    return make_token(TOKEN_INT, buffer);
    }

    if (isalpha(code[pos])) {
        char buffer[64];
        int i = 0;
        while (isalnum(code[pos])) {
            buffer[i++] = code[pos++];
        }
        buffer[i] = '\0';
        return make_token(TOKEN_ID, buffer);
    }

    char current = code[pos++];
    char sym[2] = {current, '\0'};
    
    switch (current) {
        case '=': return make_token(TOKEN_ASSIGN, sym);
        case '+': return make_token(TOKEN_PLUS, sym);
        case '-': return make_token(TOKEN_MINUS, sym);
        case '*': return make_token(TOKEN_STAR, sym);
        case '/': return make_token(TOKEN_SLASH, sym);
        case '(': return make_token(TOKEN_LPAREN, sym);
        case ')': return make_token(TOKEN_RPAREN, sym);
        case ';': return make_token(TOKEN_SEMI, sym);
        default:  return make_token(TOKEN_UNKNOWN, sym);
    }
}
