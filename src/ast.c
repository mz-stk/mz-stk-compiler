#include "../include/ast.h"

void free_ast(ASTNode* node) {
    if (!node) return;
    
    for (int i = 0; i < node->child_count; i++) {
        free_ast(node->children[i]);
    }
    
    if (node->children) {
        free(node->children);
    }
    free(node);
}

void print_ast(ASTNode* node, int indent) {
    if (!node) return;

    const char* type_names[] = {
        "PROGRAM", 
        "PUSH", 
        "ADD",
        "SUBTRACT",
        "MULTIPLY",
        "DIVIDE",
        "MODULO", 
        "INPUT",
        "PRINT",
        "STORE",
        "LOAD",
        "STARTIF",
        "ENDIF",
        "STARTWHILE",
        "ENDWHILE",
        "STARTFOR",
        "ENDFOR",  
        "INIT",
        "UPDATE",
        "CONDITION",
        "STATEMENTS",
        "ENDSTATEMENTS",
        "STARTFUNCTION",
        "ENDFUNCTION", 
        "CALL",
        "EQUAL",
        "NOT_EQUAL",
        "LESS",
        "GREATER",
        "LESS_EQUAL",
        "GREATER_EQUAL",
        "AND",
        "OR",
        "NOT",
        "START",
        "EXIT"
    };

    for (int i = 0; i < indent; i++) printf("  ");
    printf("%s", type_names[node->type]);

    if (node->type == NODE_PUSH || node->type == NODE_STORE || node->type == NODE_LOAD || node->type == NODE_CALL || node->type == NODE_STARTFUNCTION) {
        printf(" (%d)", node->value);
    } 
    printf("\n");

    for (int i = 0; i < node->child_count; i++) {
        print_ast(node->children[i], indent + 1);
    }
}
