#ifndef PARSER_H
#define PARSER_H 

#include "./headers.h"
#include "./node.h"

ASTNode* parse(Token* tokens, int token_count);

#endif 
