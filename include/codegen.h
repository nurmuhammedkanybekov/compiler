#ifndef CODEGEN_H
#define CODEGEN_H

#include "ast.h"
#include <llvm-c/Core.h>

void codegen_init();
void codegen_statement(ASTNode* node);
void codegen_finalize(); 

#endif