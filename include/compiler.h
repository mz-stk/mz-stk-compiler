#ifndef COMPILER_H
#define COMPILER_H

#include "./headers.h"

void emit(FILE* out, const char* fmt, ...);
void compile(ASTNode* node, FILE* output, int indent);
void execute(ASTNode* ast_root, FILE* output);


#endif 
