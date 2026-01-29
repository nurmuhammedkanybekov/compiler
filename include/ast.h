#ifndef AST_H
#define AST_H

typedef enum {
    AST_NUMBER,
    AST_VARIABLE,
    AST_ASSIGNMENT,
    AST_BINARY_OP,
    AST_CALL
} NodeType;

typedef struct ASTNode {
    NodeType type;
    
    union {
        int number_value;           
        char name[64];             
        
        struct {
            char name[64];
            struct ASTNode *value; 
        } assignment;
        
        struct {
            char op;         
            struct ASTNode *left;
            struct ASTNode *right;
        } binary_op;
        
        struct {
            char name[64];         
            struct ASTNode *arg; 
        } call;
    } data;
} ASTNode;

struct ASTNode* create_node(NodeType type);

#endif