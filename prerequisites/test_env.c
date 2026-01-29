#include <stdio.h>
#include <llvm-c/Core.h>

int main() {
    LLVMModuleRef mod = LLVMModuleCreateWithName("my_test_module");
    printf("Success! LLVM Module created.\n");
    LLVMDisposeModule(mod);

    return 0;
}