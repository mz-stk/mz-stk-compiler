#include "../include/parser.h"

ASTNode* parse(Token* tokens, int token_count) {
    if (token_count == 0 || tokens[0].type != TOKEN_START) {
        fprintf(stderr, "Error: Program must start with 'S'\n");
        exit(1);
    }
    
    bool has_exit = false;
    for (int i = 0; i < token_count; i++) {
        if (tokens[i].type == TOKEN_EXIT) {
            has_exit = true;
            break;
        }
    }
    if (!has_exit) {
        fprintf(stderr, "Error: Program must end with 'E'\n");
        exit(1);
    }

    ASTNode* program = create_node(NODE_PROGRAM, 0);
    program->children = malloc(token_count * sizeof(ASTNode*));
    if (!program->children) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }

    ASTNode* stack[MAX_STACK_DEPTH];
    int stack_ptr = 0;
    stack[stack_ptr++] = program;

    TokenType expected_end[MAX_STACK_DEPTH];
    int expected_end_ptr = 0;
    
    for (int i = 0; i < token_count; i++) {
        if (tokens[i].type == TOKEN_EOF) break; 
        
        ASTNode* node = NULL;
        
        switch (tokens[i].type) {
            case TOKEN_PUSH:
                node = create_node(NODE_PUSH, tokens[i].value);
                break;

            case TOKEN_ADD: node = create_node(NODE_ADD, 0); break;
            case TOKEN_SUBTRACT: node = create_node(NODE_SUBTRACT, 0); break;
            case TOKEN_MULTIPLY: node = create_node(NODE_MULTIPLY, 0); break;
            case TOKEN_DIVIDE: node = create_node(NODE_DIVIDE, 0); break;
            case TOKEN_MODULO: node = create_node(NODE_MODULO, 0); break;

            case TOKEN_AND: node = create_node(NODE_AND, 0); break;             
            case TOKEN_OR: node = create_node(NODE_OR, 0); break;
            case TOKEN_NOT: node = create_node(NODE_NOT, 0); break;

            case TOKEN_EQUAL: node = create_node(NODE_EQUAL, 0); break;
            case TOKEN_NOT_EQUAL: node = create_node(NODE_NOT_EQUAL, 0); break;
            case TOKEN_LESS: node = create_node(NODE_LESS, 0); break;
            case TOKEN_GREATER: node = create_node(NODE_GREATER, 0); break;
            case TOKEN_LESS_EQUAL: node = create_node(NODE_LESS_EQUAL, 0); break;
            case TOKEN_GREATER_EQUAL: node = create_node(NODE_GREATER_EQUAL, 0); break;

            case TOKEN_INPUT:
                node = create_node(NODE_INPUT, 0);
                break;
            case TOKEN_PRINT: 
                node = create_node(NODE_PRINT, 0);
                break;
            case TOKEN_CALL: 
                node = create_node(NODE_CALL, tokens[i].value);
                break;

            case TOKEN_STARTIF:
            case TOKEN_STARTWHILE:
            case TOKEN_STARTFOR:
            case TOKEN_STARTFUNCTION:
            case TOKEN_INIT:
            case TOKEN_UPDATE:
            case TOKEN_CONDITION:
            case TOKEN_STATEMENTS:
                switch (tokens[i].type) {
                  case TOKEN_STARTIF:     node = create_node(NODE_STARTIF, 0);    break;
                  case TOKEN_STARTWHILE:  node = create_node(NODE_STARTWHILE, 0); break;
                  case TOKEN_STARTFOR:    node = create_node(NODE_STARTFOR, 0);   break;
                  case TOKEN_STARTFUNCTION: node = create_node(NODE_STARTFUNCTION, tokens[i].value); break;
                  case TOKEN_INIT: node = create_node(NODE_INIT, 0); break;
                  case TOKEN_UPDATE: node = create_node(NODE_UPDATE, 0); break;
                  case TOKEN_CONDITION:   node = create_node(NODE_CONDITION, 0);  break;
                  case TOKEN_STATEMENTS:  node = create_node(NODE_STATEMENTS, 0); break;
                  default:
                      fprintf(stderr, "Unexpected token in parser: %d\n", tokens[i].type);
                      exit(1);
                }

                node->children = malloc(token_count * sizeof(ASTNode*));
                if (!node->children) {
                    fprintf(stderr, "Memory allocation failed\n");
                    exit(1);
                }
                if (stack_ptr >= MAX_STACK_DEPTH) {
                    fprintf(stderr, "Stack overflow: too many nested blocks\n");
                    exit(1);
                }
                stack[stack_ptr++] = node;

                switch(tokens[i].type) {
                    case TOKEN_STARTIF:
                        expected_end[expected_end_ptr++] = TOKEN_ENDIF;
                        break;
                    case TOKEN_STARTWHILE:
                        expected_end[expected_end_ptr++] = TOKEN_ENDWHILE;
                        break;
                    case TOKEN_STARTFOR:
                        expected_end[expected_end_ptr++] = TOKEN_ENDFOR;
                        break;
                    case TOKEN_STARTFUNCTION: 
                        expected_end[expected_end_ptr++] = TOKEN_ENDFUNCTION;
                        break;
                    case TOKEN_INIT:
                        expected_end[expected_end_ptr++] = TOKEN_ENDINIT;
                        break;
                    case TOKEN_UPDATE:
                        expected_end[expected_end_ptr++] = TOKEN_ENDUPDATE;
                        break;
                    case TOKEN_CONDITION:
                        expected_end[expected_end_ptr++] = TOKEN_ENDCONDITION;
                        break;
                    case TOKEN_STATEMENTS:
                        expected_end[expected_end_ptr++] = TOKEN_ENDSTATEMENTS;
                        break;
                    default:
                        fprintf(stderr, "Unexpected token in parser: %d\n", tokens[i].type);
                        exit(1);
                }
                continue;

            case TOKEN_ENDINIT:
            case TOKEN_ENDUPDATE:
            case TOKEN_ENDCONDITION:
            case TOKEN_ENDIF:
            case TOKEN_ENDWHILE:
            case TOKEN_ENDFOR:
            case TOKEN_ENDFUNCTION:
            case TOKEN_ENDSTATEMENTS: {
                if (expected_end_ptr == 0) {
                    fprintf(stderr, "Unexpected end block at position %d\n", i);
                    exit(1);
                }
                
                TokenType expected = expected_end[expected_end_ptr-1];
                
                if (tokens[i].type != expected) {
                    fprintf(stderr, "Mismatched block ending at position %d: expected %d, got %d\n", 
                            i, expected, tokens[i].type);
                    exit(1);
                }
                
                expected_end_ptr--;

                if (stack_ptr <= 1) {
                    fprintf(stderr, "Unexpected end block\n");
                    exit(1);
                }
                ASTNode* block = stack[--stack_ptr];  
                ASTNode* parent = stack[stack_ptr-1];
                parent->children[parent->child_count++] = block;

                node = create_node(
                    (tokens[i].type == TOKEN_ENDIF) ? NODE_ENDIF :
                    (tokens[i].type == TOKEN_ENDWHILE) ? NODE_ENDWHILE :
                    (tokens[i].type == TOKEN_ENDFOR) ? NODE_ENDFOR :
                    (tokens[i].type == TOKEN_ENDFUNCTION) ? NODE_ENDFUNCTION :
                    (tokens[i].type == TOKEN_ENDINIT) ? NODE_ENDINIT :
                    (tokens[i].type == TOKEN_ENDUPDATE) ? NODE_ENDUPDATE :
                    (tokens[i].type == TOKEN_ENDCONDITION) ? NODE_ENDCONDITION :
                    NODE_ENDSTATEMENTS, 0
                );
                break;
            }

            case TOKEN_STORE:
                node = create_node(NODE_STORE, tokens[i].value);
                break;
            case TOKEN_LOAD:
                node = create_node(NODE_LOAD, tokens[i].value);
                break;

            case TOKEN_START: 
                node = create_node(NODE_START, 0);
                break;
            case TOKEN_EXIT:  
                node = create_node(NODE_EXIT, 0);
                break;

            default:
                fprintf(stderr, "Unexpected token in parser: %d\n", tokens[i].type);
                exit(1);
        }
        
        if (node) {
            ASTNode* current = stack[stack_ptr-1];
            current->children[current->child_count++] = node;
        }
    }
    
    if (expected_end_ptr != 0) {
        fprintf(stderr, "Error: Unclosed block(s) at end of program\n");
        exit(1);
    }

    return program;
}
