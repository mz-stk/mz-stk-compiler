#ifndef COMPILER_H
#define COMPILER_H

#include "./headers.h"

void compile(ASTNode* node, VM* vm);
void execute(VM* vm, ASTNode* ast_root);


#endif 
