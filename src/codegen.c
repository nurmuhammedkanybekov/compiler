#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "codegen.h"
#include <llvm-c/ExecutionEngine.h>
#include <llvm-c/Target.h>
#include <llvm-c/Analysis.h>
#include <llvm-c/BitWriter.h>

LLVMContextRef context;
LLVMModuleRef module;
LLVMBuilderRef builder;
LLVMValueRef printf_func;
LLVMTypeRef printf_type;
LLVMValueRef format_str;

struct {
    char name[64];
    LLVMValueRef alloca_inst;
} named_values[100];
int named_values_count = 0;

LLVMValueRef create_global_string(const char *str) {
    LLVMValueRef str_val = LLVMConstStringInContext(context, str, strlen(str) + 1, 0);
    LLVMValueRef global_var = LLVMAddGlobal(module, LLVMTypeOf(str_val), "format_str");
    LLVMSetLinkage(global_var, LLVMInternalLinkage);
    LLVMSetInitializer(global_var, str_val);
    LLVMSetGlobalConstant(global_var, 1);
    LLVMSetUnnamedAddr(global_var, 1);
    return global_var; 
}

LLVMValueRef get_variable(const char* name) {
    for (int i = 0; i < named_values_count; i++) {
        if (strcmp(named_values[i].name, name) == 0) {
            return named_values[i].alloca_inst;
        }
    }
    return NULL;
}

LLVMValueRef create_variable(const char* name) {
    LLVMValueRef existing = get_variable(name);
    if (existing) return existing;

    LLVMValueRef alloca = LLVMBuildAlloca(builder, LLVMInt32TypeInContext(context), name);
    strcpy(named_values[named_values_count].name, name);
    named_values[named_values_count].alloca_inst = alloca;
    named_values_count++;
    return alloca;
}

void codegen_init() {
    LLVMInitializeNativeTarget();
    LLVMInitializeNativeAsmPrinter();
    LLVMInitializeNativeAsmParser();

    context = LLVMContextCreate();
    module = LLVMModuleCreateWithNameInContext("my_simple_lang", context);
    builder = LLVMCreateBuilderInContext(context);

    LLVMTypeRef param_types[] = { LLVMPointerType(LLVMInt8TypeInContext(context), 0) };
    printf_type = LLVMFunctionType(LLVMInt32TypeInContext(context), param_types, 1, 1);
    printf_func = LLVMAddFunction(module, "printf", printf_type);

    format_str = create_global_string("%d\n");

    LLVMTypeRef main_type = LLVMFunctionType(LLVMInt32TypeInContext(context), NULL, 0, 0);
    LLVMValueRef main_func = LLVMAddFunction(module, "main", main_type);
    
    LLVMBasicBlockRef entry = LLVMAppendBasicBlockInContext(context, main_func, "entry");
    LLVMPositionBuilderAtEnd(builder, entry);
}

LLVMValueRef codegen_expression(ASTNode* node) {
    if (node->type == AST_NUMBER) {
        return LLVMConstInt(LLVMInt32TypeInContext(context), node->data.number_value, 0);
    }
    
    if (node->type == AST_VARIABLE) {
        LLVMValueRef alloca = get_variable(node->data.name);
        if (!alloca) {
            fprintf(stderr, "Error: Unknown variable '%s'\n", node->data.name);
            exit(1);
        }
        return LLVMBuildLoad2(builder, LLVMInt32TypeInContext(context), alloca, node->data.name);
    }

    if (node->type == AST_BINARY_OP) {
        LLVMValueRef left = codegen_expression(node->data.binary_op.left);
        LLVMValueRef right = codegen_expression(node->data.binary_op.right);

        switch (node->data.binary_op.op) {
            case '+': return LLVMBuildAdd(builder, left, right, "addtmp");
            case '-': return LLVMBuildSub(builder, left, right, "subtmp");
            case '*': return LLVMBuildMul(builder, left, right, "multmp");
            case '/': return LLVMBuildSDiv(builder, left, right, "divtmp");
            default: return NULL;
        }
    }
    return NULL;
}

void codegen_statement(ASTNode* node) {
    if (node->type == AST_ASSIGNMENT) {
        LLVMValueRef val = codegen_expression(node->data.assignment.value);
        LLVMValueRef var = create_variable(node->data.assignment.name);
        LLVMBuildStore(builder, val, var);
    }
    else if (node->type == AST_CALL) {
        if (strcmp(node->data.call.name, "print") == 0) {
            LLVMValueRef val = codegen_expression(node->data.call.arg);
            LLVMValueRef args[] = { format_str, val };
            LLVMBuildCall2(builder, printf_type, printf_func, args, 2, "calltmp");
        }
    }
}

void codegen_finalize() {
    LLVMBuildRet(builder, LLVMConstInt(LLVMInt32TypeInContext(context), 0, 0));
    
    char *error = NULL;
    if (LLVMVerifyModule(module, LLVMAbortProcessAction, &error)) {
        printf("Error verifying module: %s\n", error);
        LLVMDisposeMessage(error);
        return;
    }

    printf("\n[JIT] Module verified. Preparing execution engine...\n");

    LLVMExecutionEngineRef engine;
    if (LLVMCreateExecutionEngineForModule(&engine, module, &error) != 0) {
        printf("Failed to create execution engine: %s\n", error);
        LLVMDisposeMessage(error);
        return;
    }

    printf("[JIT] Running program...\n\n");
    printf("--- PROGRAM OUTPUT START ---\n");

    LLVMValueRef main_func = LLVMGetNamedFunction(module, "main");
    LLVMRunFunctionAsMain(engine, main_func, 0, NULL, NULL);

    printf("--- PROGRAM OUTPUT END ---\n\n");

    LLVMDisposeExecutionEngine(engine); 
    LLVMContextDispose(context);
}
