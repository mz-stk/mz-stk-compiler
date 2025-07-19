# mz-stk

A LIFO stack-based compiled programming language that compiles to C.

## Overview

mz-stk is a stack-based programming language designed around Last-In-First-Out (LIFO) stack operations. The language compiles directly to C code, providing both performance and portability while maintaining the simplicity and elegance of stack-based computation.

## Features

- **Stack-based architecture**: All operations work with a LIFO stack
- **Compiles to C**: Generates readable C code for maximum portability
- **Simple toolchain**: Straightforward build and installation process
- **Executable generation**: Creates ready-to-run binaries
- **Online playground**: Test your code directly in the browser (coming soon)

## Installation

### Building from Source

```bash
# Clone the repository
git clone https://github.com/mz-stk/mz-stk-compiler.git
cd mz-stk

# Build the compiler
make

# Install system-wide (optional)
sudo make install
```

### Cleaning Build Files

```bash
# Clean installed and build artifacts 
make clean
```

### Uninstalling

```bash
# Remove installed files for system-wide installation
sudo make uninstall
```

## Usage

### Basic Usage

To compile and run a mz-stk program:

```bash
# Compile a .mzstk file
mzstk filename.mzstk

# This generates:
# - filename.c (C source code)
# - filename (executable binary)

# Run the executable
./filename
```

## Language Basics

mz-stk operates on a stack-based model where:

- Values are pushed onto a stack
- Operations pop values from the stack, perform computations, and push results back
- The stack follows Last-In-First-Out (LIFO) ordering

## Data Types

- mz-stk supports numeric values only, keeping the language and focused on stack-based computations.

## Language Reference

mz-stk provides a comprehensive set of operations for stack-based programming:

### Stack Operations
- **Numbers** - Push numbers onto the stack
- **`+`** - Add two numbers from stack
- **`-`** - Subtract two numbers from stack  
- **`*`** - Multiply two numbers from stack
- **`/`** - Divide two numbers from stack
- **`%`** - Modulo operation

### Input/Output
- **`^`** - Get input from user
- **`V`** - Print top value from stack

### Variables
- **`:n`** - Store top stack value in variable n (where n is a number)
- **`;n`** - Load variable n onto stack (where n is a number)

### Conditionals
- **`[`** - Start if statement
- **`]`** - End if statement

### Loops
- **`{`** - Start while loop
- **`}`** - End while loop
- **`(`** - Start for loop
- **`)`** - End for loop

### For Loop Components
- **`I`** - Start initialization section
- **`i`** - End initialization section
- **`C`** - Start condition section
- **`c`** - End condition section
- **`U`** - Start update section
- **`u`** - End update section
- **`L`** - Start statements section
- **`l`** - End statements section

### Functions
- **`@n`** - Define function n (where n is a number)
- **`$`** - End function definition
- **`!n`** - Call function n (where n is a number)

### Comparison Operations
- **`==`** - Equal to
- **`!=`** - Not equal to
- **`<`** - Less than
- **`>`** - Greater than
- **`<=`** - Less than or equal to
- **`>=`** - Greater than or equal to

### Logical Operations
- **`&&`** - Logical AND
- **`||`** - Logical OR
- **`!`** - Logical NOT (when not followed by a number)

### Program Control
- **`S`** - Start program
- **`E`** - Exit program
- **`#`** - Comments (rest of line ignored)

**Note**: Some tokens serve multiple purposes depending on context:
- **`!`** - Logical NOT (standalone) or function call (!n with number)
- **`L`** - Statement start or loop/condition statement
- **`l`** - Statement end or loop/condition statement
- **`C`** - Condition start or loop/condition condition 
- **`c`** - Condition end or loop/condition  

Please refer to the `samples/` folder for proper usage examples showing these context-dependent uses.

## File Structure

When you compile `program.mzstk`, mz-stk generates:

- `program.c` - Generated C source code
- `program` - Compiled executable binary

## Development

### Building

The project uses a standard Makefile:

```bash
make          # Build the compiler
make clean    # Clean build files
make install  # Install system-wide
make uninstall # Remove installation
```

## Playground

Try mz-stk online right in your browser with our interactive playground!

🔗 [Open Online Playground](https://mzstk.arungeorgesaji.hackclub.app)

Features:
- Write and edit mz-stk code directly in your browser
- Run programs instantly without any installation
- Share code snippets with others
- See compilation and execution output side-by-side

**Note about input**: The `^` operator (get input from user) is currently not supported in the playground environment due to technical limitations of running in a browser sandbox. For programs that require user input, you'll need to:
1. Download and run them locally, or
2. Pre-define your input values in the code

*mz-stk - Stack-based programming made simple*
