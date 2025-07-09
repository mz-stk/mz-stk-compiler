#ifndef AST_H
#define AST_H

#include "./headers.h"

void free_ast(ASTNode* node);
void print_ast(ASTNode* node, int indent);

#endif 
