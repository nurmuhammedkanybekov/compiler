#ifndef TOKENS_H
#define TOKENS_H

typedef enum {
    TOKEN_INT,
    TOKEN_ID,
    TOKEN_ASSIGN,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_STAR,
    TOKEN_SLASH,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_SEMI,
    TOKEN_EOF,
    TOKEN_UNKNOWN
} TokenType;

typedef struct {
    TokenType type;
    char text[64];
} Token;

#endif
