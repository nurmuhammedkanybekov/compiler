#include <stdio.h>
#include <string.h>
#include "parser.h"
#include "codegen.h"

int main(int argc, char **argv) {
    const char *code;

    if (argc > 1) {
        code = argv[1];
    } else {
        printf("No code provided. Running default test case...\n");
        code = "x = 10; y = 5; print(x * y);";
    }

    printf("Executing: \"%s\"\n", code);
    printf("-------------------------\n");

    init_parser(code);
    codegen_init();

    ASTNode* stmt;
    while ((stmt = parse_statement()) != NULL) {
        codegen_statement(stmt);
    }

    codegen_finalize();
    
    return 0;
}
