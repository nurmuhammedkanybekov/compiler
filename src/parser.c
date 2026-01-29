#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "lexer.h"

Token current_token;

void eat(TokenType type) {
    if (current_token.type == type) {
        current_token = get_next_token();
    } else {
        printf("Syntax Error: Expected token type %d but got %d ('%s')\n", 
               type, current_token.type, current_token.text);
        exit(1);
    }
}

ASTNode* create_node(NodeType type) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = type;
    return node;
}

ASTNode* parse_expression();

ASTNode* parse_factor() {
    ASTNode* node = create_node(AST_NUMBER);
    
    if (current_token.type == TOKEN_INT) {
        node->type = AST_NUMBER;
        node->data.number_value = atoi(current_token.text);
        eat(TOKEN_INT);
    } 
    else if (current_token.type == TOKEN_ID) {
        node->type = AST_VARIABLE;
        strcpy(node->data.name, current_token.text);
        eat(TOKEN_ID);
    } 
    else if (current_token.type == TOKEN_LPAREN) {
        eat(TOKEN_LPAREN);
        free(node);
        node = parse_expression();
        eat(TOKEN_RPAREN);
    } 
    else {
        printf("Syntax Error: Unexpected token '%s'\n", current_token.text);
        exit(1);
    }
    return node;
}

ASTNode* parse_term() {
    ASTNode* node = parse_factor();

    while (current_token.type == TOKEN_STAR || current_token.type == TOKEN_SLASH) {
        char op = current_token.text[0];
        eat(current_token.type);

        ASTNode* new_node = create_node(AST_BINARY_OP);
        new_node->data.binary_op.op = op;
        new_node->data.binary_op.left = node;
        new_node->data.binary_op.right = parse_factor();
        node = new_node;
    }
    return node;
}

ASTNode* parse_expression() {
    ASTNode* node = parse_term();

    while (current_token.type == TOKEN_PLUS || current_token.type == TOKEN_MINUS) {
        char op = current_token.text[0];
        eat(current_token.type);

        ASTNode* new_node = create_node(AST_BINARY_OP);
        new_node->data.binary_op.op = op;
        new_node->data.binary_op.left = node;
        new_node->data.binary_op.right = parse_term();
        node = new_node;
    }
    return node;
}

ASTNode* parse_statement() {
    if (current_token.type == TOKEN_ID) {
        char name[64];
        strcpy(name, current_token.text);
        eat(TOKEN_ID);

        if (current_token.type == TOKEN_ASSIGN) {
            eat(TOKEN_ASSIGN);
            ASTNode* node = create_node(AST_ASSIGNMENT);
            strcpy(node->data.assignment.name, name);
            node->data.assignment.value = parse_expression();
            eat(TOKEN_SEMI);
            return node;
        }
        else if (current_token.type == TOKEN_LPAREN) {
            eat(TOKEN_LPAREN);
            ASTNode* node = create_node(AST_CALL);
            strcpy(node->data.call.name, name);
            node->data.call.arg = parse_expression();
            eat(TOKEN_RPAREN);
            eat(TOKEN_SEMI);
            return node;
        }
    }
    return NULL;
}

void init_parser(const char* code) {
    init_lexer(code);
    current_token = get_next_token();
}
