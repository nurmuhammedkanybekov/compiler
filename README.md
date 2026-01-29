# Custom Compiler & JIT Engine

## Project Overview: The One-Day Compiler Challenge

This project was built under a strict self-imposed deadline to create a functional compiler frontend and execution engine from scratch. The primary goal was to demystify compiler internals by building a system that doesn't just parse text, but generates machine code and executes it in memory.

The strategy was to implement a "Toy Language" that is simple in syntax but complex in execution, utilizing the LLVM C API (Core) for the backend.

## Language Specifications

The compiler supports a strictly typed subset of C designed for integer arithmetic:

* **Data Types:** Integers only.
* **Variables:** Storage and retrieval (e.g., `x = 10;`).
* **Arithmetic:** Basic operations (`+`, `-`, `*`, `/`).
* **I/O:** Built-in print function (e.g., `print(result);`).

## System Architecture

The project follows a standard three-stage compiler pipeline:

**1. The Lexer (Scanner)**

* **Input:** Raw source code string.
* **Process:** Tokenization. It breaks the string into discrete units (Identifiers, Operators, Numeric Literals).

**2. The Parser**

* **Input:** A list of Tokens.
* **Process:** Semantic Analysis. It organizes tokens into an Abstract Syntax Tree (AST) to validate the logic and enforce operator precedence.

**3. The Code Generator (LLVM Backend)**

* **Input:** The AST.
* **Process:** IR Generation. It traverses the tree and calls LLVM C functions to generate LLVM Intermediate Representation (IR).
* **Execution:** Instead of writing to an object file, the system uses an LLVM Execution Engine to compile the IR into machine code in RAM and execute it via JIT (Just-In-Time) compilation.

## Technical Challenges

Adapting the project to **Modern LLVM (v15+)** required solving specific architectural changes in the library:

**Opaque Pointers**
Modern LLVM uses Opaque Pointers, which renders older tutorials using functions like `LLVMBuildGlobalStringPtr` obsolete. To resolve segmentation faults caused by this shift, I had to:

* Explicitly manage the global context using `LLVMContextCreate`.
* Manually construct global strings for `printf` calls.

**Platform Linking (macOS)**
The default Apple Clang compiler is often version-mismatched with upstream LLVM. The build system was configured to link specifically against the Homebrew LLVM installation to ensure access to the latest C headers and linkers.

## Project Structure

The codebase was refactored from a flat file structure to a modular C project layout to ensure maintainability:

```text
/compiler_project
  ├── Makefile          # Automated build and linking script
  ├── src/              # Source code (Lexer, Parser, CodeGen)
  ├── include/          # Header definitions
  ├── prerequisites/    # Environment setup
  └── build/            # Compiled artifacts

```

## Build and Usage

The project includes a Makefile to automate the complex linking of LLVM libraries.

**1. Build the project**

```bash
make

```

**2. Run the compiler**
Pass the source code string directly as an argument.

```bash
./my_jit_compiler "x = 10; y = 5; print(x * y + 2);"

```

**Output:**

```text
Executing: "x = 10; y = 5; print(x * y + 2);"
-------------------------
[JIT] Module verified. Preparing execution engine...
[JIT] Running program...

--- PROGRAM OUTPUT START ---
52
--- PROGRAM OUTPUT END ---

```

**3. Clean build files**

```bash
make clean

```
