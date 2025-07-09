#ifndef HEADERS_H
#define HEADERS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>

#define MAX_STACK_DEPTH 100
#define STACK_SIZE 1000
#define MEMORY_SIZE 1000

typedef enum {
    NODE_PROGRAM, 
    NODE_PUSH, 
    NODE_ADD,
    NODE_SUBTRACT,
    NODE_MULTIPLY,
    NODE_DIVIDE,
    NODE_MODULO, 
    NODE_INPUT, 
    NODE_PRINT,
    NODE_STORE,
    NODE_LOAD,
    NODE_STARTIF,
    NODE_ENDIF,
    NODE_STARTWHILE,
    NODE_ENDWHILE,
    NODE_STARTFOR,
    NODE_ENDFOR,  
    NODE_INIT,
    NODE_CONDITION,
    NODE_UPDATE,
    NODE_STATEMENTS,
    NODE_ENDSTATEMENTS,
    NODE_STARTFUNCTION,
    NODE_ENDFUNCTION, 
    NODE_CALL,
    NODE_EQUAL,
    NODE_NOT_EQUAL,
    NODE_LESS,
    NODE_GREATER, 
    NODE_LESS_EQUAL,
    NODE_GREATER_EQUAL,
    NODE_AND, 
    NODE_OR, 
    NODE_NOT,
    NODE_START,
    NODE_EXIT,
} NodeType;

typedef struct ASTNode {
    NodeType type;
    int value;
    struct ASTNode** children;
    int child_count;
    struct ASTNode* condition;
} ASTNode;

typedef enum {
    TOKEN_PUSH, 
    TOKEN_ADD,
    TOKEN_SUBTRACT,
    TOKEN_MULTIPLY,
    TOKEN_DIVIDE,
    TOKEN_MODULO,
    TOKEN_INPUT,
    TOKEN_PRINT, 
    TOKEN_STORE,
    TOKEN_LOAD,
    TOKEN_STARTIF,
    TOKEN_ENDIF,
    TOKEN_STARTWHILE,
    TOKEN_ENDWHILE,
    TOKEN_STARTFOR,
    TOKEN_ENDFOR,
    TOKEN_INIT,
    TOKEN_CONDITION, 
    TOKEN_UPDATE,
    TOKEN_STATEMENTS,
    TOKEN_ENDSTATEMENTS,
    TOKEN_STARTFUNCTION, 
    TOKEN_ENDFUNCTION, 
    TOKEN_CALL,
    TOKEN_EQUAL,
    TOKEN_NOT_EQUAL,
    TOKEN_LESS,
    TOKEN_GREATER,
    TOKEN_LESS_EQUAL,
    TOKEN_GREATER_EQUAL,
    TOKEN_AND, 
    TOKEN_OR, 
    TOKEN_NOT,
    TOKEN_START,
    TOKEN_EXIT,
    TOKEN_EOF
} TokenType;

typedef struct {
    TokenType type;
    int value;
} Token;

typedef struct {
    int stack[STACK_SIZE];
    int sp;  
    int memory[MEMORY_SIZE];  
    ASTNode** functions;      
    int function_count;
} VM;

#endif
