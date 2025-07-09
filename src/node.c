#include "../include/node.h"

ASTNode* create_node(NodeType type, int value) {
    ASTNode* node = malloc(sizeof(ASTNode));
    if (!node) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    node->type = type;
    node->value = value;
    node->children = NULL;
    node->child_count = 0;
    return node;
}
