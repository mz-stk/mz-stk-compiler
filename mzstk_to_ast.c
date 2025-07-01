#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#define MAX_STACK_DEPTH 100

typedef enum {
    NODE_PROGRAM, 
    NODE_PUSH, 
    NODE_ADD,
    NODE_SUBTRACT,
    NODE_MULTIPLY,
    NODE_DIVIDE,
    NODE_MODULO, 
    NODE_STORE,
    NODE_LOAD,
    NODE_STARTIF,
    NODE_ENDIF,
    NODE_STARTWHILE,
    NODE_ENDWHILE,
    NODE_STARTFOR,
    NODE_ENDFOR,  
    NODE_STARTFUNCTION,
    NODE_ENDFUNCTION, 
    NODE_EXIT,
} NodeType;

typedef struct ASTNode {
    NodeType type;
    int value;
    struct ASTNode** children;
    int child_count;
} ASTNode;

typedef enum {
    TOKEN_PUSH, 
    TOKEN_ADD,
    TOKEN_SUBTRACT,
    TOKEN_MULTIPLY,
    TOKEN_DIVIDE,
    TOKEN_MODULO,
    TOKEN_STORE,
    TOKEN_LOAD,
    TOKEN_STARTIF,
    TOKEN_ENDIF,
    TOKEN_STARTWHILE,
    TOKEN_ENDWHILE,
    TOKEN_STARTFOR,
    TOKEN_ENDFOR,
    TOKEN_STARTFUNCTION, 
    TOKEN_ENDFUNCTION, 
    TOKEN_START,
    TOKEN_EXIT,
    TOKEN_EOF
} TokenType;

typedef struct {
    TokenType type;
    int value;
} Token;

Token* lex(const char* input, int* token_count) {
    Token * tokens = malloc(strlen(input) * sizeof(Token));
    *token_count = 0;

    for (int i = 0; input[i] != '\0'; i++) { 
        char c = input[i];
        Token token;

        if (isspace(c)) continue;
        
        if (c == '#') {  
            while (input[i] != '\n' && input[i] != '\0') i++;
            continue;
        }
    
        if (isdigit(c)) {
            token.type = TOKEN_PUSH;
            token.value = 0;
            while (isdigit(input[i])) {
                token.value = token.value * 10 + (input[i] - '0');
                i++;
            }
            i--; 
            tokens[(*token_count)++] = token;
            continue;
        }
    
        switch(c) {
            case '+': token.type = TOKEN_ADD; break;
            case '-': token.type = TOKEN_SUBTRACT; break;
            case '*': token.type = TOKEN_MULTIPLY; break;
            case '/': token.type = TOKEN_DIVIDE; break;
            case '%': token.type = TOKEN_MODULO; break;

            case '[': token.type = TOKEN_STARTIF; break;
            case ']': token.type = TOKEN_ENDIF; break;
            case '{': token.type = TOKEN_STARTWHILE; break;
            case '}': token.type = TOKEN_ENDWHILE; break;
            case '(': token.type = TOKEN_STARTFOR; break;
            case ')': token.type = TOKEN_ENDFOR; break;

            case '@': token.type = TOKEN_STARTFUNCTION; break;
            case '!': token.type = TOKEN_ENDFUNCTION; break;

            case ':':
                if (!isalpha(input[i + 1])) {
                    fprintf(stderr, "Invalid variable name after ':'\n");
                    exit(1);
                }
                token.type = TOKEN_STORE;
                token.value = input[++i];
                break;
            case ';':
                if (!isalpha(input[i + 1])) {
                    fprintf(stderr, "Invalid variable name after ';'\n");
                    exit(1);
                }
                token.type = TOKEN_LOAD;
                token.value = input[++i];
                break;

            case 'S': token.type = TOKEN_START; break;
            case 'E': token.type = TOKEN_EXIT; break; 

            default:
                fprintf(stderr, "Unknown token: '%c' (ASCII %d)\n", c, c);
                exit(1);
        }
        
        tokens[(*token_count)++] = token;
    }

    tokens[*token_count].type = TOKEN_EOF;
    return tokens;
}

ASTNode* parse(Token* tokens, int token_count) {
    if (token_count == 0 || tokens[0].type != TOKEN_START) {
        fprintf(stderr, "Error: Program must start with 'S'\n");
        exit(1);
    }
    if (tokens[token_count-1].type != TOKEN_EXIT) {
        fprintf(stderr, "Error: Program must end with 'E'\n");
        exit(1);
    }

    ASTNode* program = malloc(sizeof(ASTNode));
    program->type = NODE_PROGRAM;
    program->children = malloc(token_count * sizeof(ASTNode*));
    program->child_count = 0;

    ASTNode* stack[MAX_STACK_DEPTH];
    int stack_ptr = 0;
    stack[stack_ptr++] = program;

    TokenType expected_end[MAX_STACK_DEPTH];
    int expected_end_ptr = 0;
    
    for (int i = 0; i < token_count; i++) {
        ASTNode* node = malloc(sizeof(ASTNode));  
        switch (tokens[i].type) {
            case TOKEN_PUSH:
                node->type = NODE_PUSH;
                node->value = tokens[i].value;
                break;

            case TOKEN_ADD:      node->type = NODE_ADD; break;
            case TOKEN_SUBTRACT: node->type = NODE_SUBTRACT; break;
            case TOKEN_MULTIPLY: node->type = NODE_MULTIPLY; break;
            case TOKEN_DIVIDE:   node->type = NODE_DIVIDE; break;
            case TOKEN_MODULO:  node->type = NODE_MODULO; break;

            case TOKEN_STARTIF:
                node->type = NODE_STARTIF;
                node->children = malloc(2 * sizeof(ASTNode*));
                node->child_count = 0; 
                if (stack_ptr >= MAX_STACK_DEPTH) {
                    fprintf(stderr, "Stack overflow: too many nested blocks\n");
                    exit(1);
                }
                stack[stack_ptr++] = node;
                expected_end[expected_end_ptr++] = TOKEN_ENDIF;
                continue;
            case TOKEN_STARTWHILE:
                node->type = NODE_STARTWHILE;
                node->children = malloc(2 * sizeof(ASTNode*));
                node->child_count = 0; 
                if (stack_ptr >= MAX_STACK_DEPTH) {
                    fprintf(stderr, "Stack overflow: too many nested blocks\n");
                    exit(1);
                }
                stack[stack_ptr++] = node;
                expected_end[expected_end_ptr++] = TOKEN_ENDWHILE;
                continue;
            case TOKEN_STARTFOR:
                node->type = NODE_STARTFOR;
                node->children = malloc(4 * sizeof(ASTNode*));
                node->child_count = 0; 
                if (stack_ptr >= MAX_STACK_DEPTH) {
                    fprintf(stderr, "Stack overflow: too many nested blocks\n");
                    exit(1);
                } 
                stack[stack_ptr++] = node;
                expected_end[expected_end_ptr++] = TOKEN_ENDFOR;
                continue;
            case TOKEN_STARTFUNCTION:
                node->type = NODE_STARTFUNCTION;
                node->children = malloc(token_count * sizeof(ASTNode*)); 
                node->child_count = 0; 
                if (stack_ptr >= MAX_STACK_DEPTH) {
                    fprintf(stderr, "Stack overflow: too many nested blocks\n");
                    exit(1);
                }
                stack[stack_ptr++] = node;
                expected_end[expected_end_ptr++] = TOKEN_ENDFUNCTION;
                continue;
            
            case TOKEN_ENDIF:
            case TOKEN_ENDWHILE:
            case TOKEN_ENDFOR:
            case TOKEN_ENDFUNCTION: {
                if (expected_end_ptr == 0) {
                    fprintf(stderr, "Unexpected end block at position %d\n", i);
                    exit(1);
                }
                
                TokenType expected = expected_end[expected_end_ptr-1];
                if ((tokens[i].type == TOKEN_ENDIF && expected != TOKEN_ENDIF) ||
                    (tokens[i].type == TOKEN_ENDWHILE && expected != TOKEN_ENDWHILE) ||
                    (tokens[i].type == TOKEN_ENDFOR && expected != TOKEN_ENDFOR) ||
                    (tokens[i].type == TOKEN_ENDFUNCTION && expected != TOKEN_ENDFUNCTION)) {
                    fprintf(stderr, "Mismatched block ending at position %d\n", i);
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
                free(node);
                continue;
            }

            case TOKEN_STORE:
                node->type = NODE_STORE;
                node->value = tokens[i].value;  
                break;
            case TOKEN_LOAD:
                node->type = NODE_LOAD;
                node->value = tokens[i].value;  
                break;

            case TOKEN_START: 
                free(node); 
                continue;  
            case TOKEN_EXIT:  
                node->type = NODE_EXIT; 
                break;
            case TOKEN_EOF: 
                fprintf(stderr, "Unexpected EOF token\n"); 
                exit(1);

            default:
                fprintf(stderr, "Unexpected token in parser: %d\n", tokens[i].type);
                exit(1);
        }
        
        if (node->type != NODE_STARTIF && node->type != NODE_STARTWHILE && 
            node->type != NODE_STARTFOR && node->type != NODE_STARTFUNCTION) {
            node->children = NULL;
            node->child_count = 0;
        }
        
        ASTNode* current = stack[stack_ptr-1];
        current->children[current->child_count++] = node;
    }
    
    if (expected_end_ptr != 0) {
        fprintf(stderr, "Error: Unclosed block(s) at end of program\n");
        exit(1);
    }

    return program;
}

void print_ast(ASTNode* node, int indent) {
    const char* type_names[] = {
        "PROGRAM", 
        "PUSH", 
        "ADD",
        "SUBTRACT",
        "MULTIPLY",
        "DIVIDE",
        "MODULO", 
        "STORE",
        "LOAD",
        "STARTIF",
        "ENDIF",
        "STARTWHILE",
        "ENDWHILE",
        "STARTFOR",
        "ENDFOR",  
        "STARTFUNCTION",
        "ENDFUNCTION", 
        "EXIT"
    };

    for (int i = 0; i < indent; i++) printf("  ");
    printf("%s", type_names[node->type]);

    if (node->type == NODE_PUSH) {
        printf(" (%d)", node->value);
    } else if (node->type == NODE_STORE || node->type == NODE_LOAD) {
        printf(" (%c)", (char)node->value);
    }
    printf("\n");

    for (int i = 0; i < node->child_count; i++) {
        print_ast(node->children[i], indent + 1);
    }
}

int main(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <input.mzstk>\n", argv[0]);
        return 1;
    }

    char* ext = strrchr(argv[1], '.');
    if (ext == NULL || strcmp(ext, ".mzstk") != 0) {
        fprintf(stderr, "Error: Input file must have .mzstk extension\n");
        return 1;
    }

    FILE* file = fopen(argv[1], "r");
    if (!file) {
        perror("Failed to open file");
        return 1;
    }
    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);
    char* input = malloc(length + 1);
    fread(input, 1, length, file);
    input[length] = '\0';
    fclose(file);

    int token_count;
    Token* tokens = lex(input, &token_count);
    ASTNode* ast = parse(tokens, token_count);
    print_ast(ast, 0);

    free(input);
    free(tokens);

    return 0;
}
