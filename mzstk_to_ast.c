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

Token* lex(const char* input, int* token_count) {
    size_t input_len = strlen(input);
    Token* tokens = malloc((input_len + 1) * sizeof(Token));
    if (!tokens) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    *token_count = 0;

    for (size_t i = 0; input[i] != '\0'; i++) { 
        char c = input[i];
        Token token;
        token.value = 0; 

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

            case '&':
                if (i+1 >= strlen(input)) {
                    fprintf(stderr, "Unexpected end of input after '%c'\n", input[i]);
                    free(tokens);
                    exit(1);
                }

                if (input[i+1] == '&') {
                    token.type = TOKEN_AND;
                    i++;
                } else {
                    fprintf(stderr, "Invalid token: '&' must be followed by '&'\n");
                    free(tokens);
                    exit(1);
                } 
                break;

            case '|':
                if (i+1 >= strlen(input)) {
                    fprintf(stderr, "Unexpected end of input after '%c'\n", input[i]);
                    free(tokens);
                    exit(1);
                }

                if (input[i+1] == '|') {
                    token.type = TOKEN_OR;
                    i++;
                } else {
                    fprintf(stderr, "Invalid token: '|' must be followed by '|'\n");
                    free(tokens);
                    exit(1);
                }
                break;

            case '=':
            case '!':
                if (i+1 >= strlen(input)) {
                    fprintf(stderr, "Unexpected end of input after '%c'\n", input[i]);
                    free(tokens);
                    exit(1);
                }
                if (input[i+1] == '=') {
                    token.type = (c == '=') ? TOKEN_EQUAL : TOKEN_NOT_EQUAL;
                    i++; 
                } else if (c == '!') { 
                    token.type = TOKEN_NOT;
                } else {
                    fprintf(stderr, "Invalid token: '%c' must be followed by '='\n", c);
                    free(tokens);
                    exit(1);
                }
                break;

            case '<':
            case '>':
                token.type = (c == '<') ? TOKEN_LESS : TOKEN_GREATER;
                if (i+1 < strlen(input) && input[i+1] == '=') {
                    token.type = (c == '<') ? TOKEN_LESS_EQUAL : TOKEN_GREATER_EQUAL;
                    i++; 
                }
                break;

            case '[': token.type = TOKEN_STARTIF; break;
            case ']': token.type = TOKEN_ENDIF; break;
            case '{': token.type = TOKEN_STARTWHILE; break;
            case '}': token.type = TOKEN_ENDWHILE; break;
            case '(': token.type = TOKEN_STARTFOR; break;
            case ')': token.type = TOKEN_ENDFOR; break;

            case '@': token.type = TOKEN_STARTFUNCTION; break;
            case '$': token.type = TOKEN_ENDFUNCTION; break;

            case ':':
                if (i + 1 >= input_len || !isalpha(input[i + 1])) {
                    fprintf(stderr, "Invalid variable name after ':'\n");
                    free(tokens);
                    exit(1);
                }
                token.type = TOKEN_STORE;
                token.value = input[++i];
                break;
            case ';':
                if (i + 1 >= input_len || !isalpha(input[i + 1])) {
                    fprintf(stderr, "Invalid variable name after ';'\n");
                    free(tokens);
                    exit(1);
                }
                token.type = TOKEN_LOAD;
                token.value = input[++i];
                break;

            case 'S': token.type = TOKEN_START; break;
            case 'E': token.type = TOKEN_EXIT; break; 

            default:
                fprintf(stderr, "Unknown token: '%c' (ASCII %d)\n", c, c);
                free(tokens);
                exit(1);
        }
        
        tokens[(*token_count)++] = token;
    }

    Token eof_token = {TOKEN_EOF, 0};
    tokens[(*token_count)++] = eof_token;
    return tokens;
}

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

            case TOKEN_STARTIF:
                node = create_node(NODE_STARTIF, 0);
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
                expected_end[expected_end_ptr++] = TOKEN_ENDIF;
                continue;
                
            case TOKEN_STARTWHILE:
                node = create_node(NODE_STARTWHILE, 0);
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
                expected_end[expected_end_ptr++] = TOKEN_ENDWHILE;
                continue;
                
            case TOKEN_STARTFOR:
                node = create_node(NODE_STARTFOR, 0);
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
                expected_end[expected_end_ptr++] = TOKEN_ENDFOR;
                continue;
                
            case TOKEN_STARTFUNCTION:
                node = create_node(NODE_STARTFUNCTION, 0);
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
                continue;
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
    if (!input) {
        fprintf(stderr, "Memory allocation failed\n");
        fclose(file);
        return 1;
    }
    
    fread(input, 1, length, file);
    input[length] = '\0';
    fclose(file);

    int token_count;
    Token* tokens = lex(input, &token_count);
    ASTNode* ast = parse(tokens, token_count);
    print_ast(ast, 0);

    free_ast(ast);
    free(tokens);
    free(input);

    return 0;
}
