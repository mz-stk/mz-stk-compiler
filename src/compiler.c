#include "../include/compiler.h"

static int temp_counter = 0;

void emit(FILE* out, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vfprintf(out, fmt, args);
    va_end(args);
    fprintf(out, "\n");
}

void compile(ASTNode* node, FILE* output, int indent) {
    if (!node) return; 

    #define INDENT for (int i=0; i<indent; i++) fprintf(output, "    ")

    switch (node->type) {
        case NODE_PROGRAM: 
            emit(output, "#include <stdio.h>");
            emit(output, "#include <stdlib.h>");
            emit(output, "#include <stdbool.h>");
            emit(output, "int stack[1024], sp = -1;");
            emit(output, "#define MEM_SIZE %d", MEMORY_SIZE);
            emit(output, "int memory[MEM_SIZE];");
            
            for (int i = 0; i < node->child_count; i++) {
                if (node->children[i]->type == NODE_STARTFUNCTION) {
                    compile(node->children[i], output, indent);
                }
            }
            
            emit(output, "int main() {");
            for (int i = 0; i < node->child_count; i++) {
                if (node->children[i]->type != NODE_STARTFUNCTION) {
                    compile(node->children[i], output, indent+1);
                }
            }
            emit(output, "    return 0;");
            emit(output, "}");
            break;

        case NODE_START:
            for (int i = 0; i < node->child_count; i++) {
                compile(node->children[i], output, indent);
            }
            break;
        
        case NODE_PUSH:
            INDENT; emit(output, "stack[++sp] = %d;", node->value);
            break;

        case NODE_ADD: {
            INDENT; emit(output, "stack[sp-1] = stack[sp-1] + stack[sp];");
            INDENT; emit(output, "sp--;");
            break;
        }
        
        case NODE_SUBTRACT: {
            INDENT; emit(output, "stack[sp-1] = stack[sp-1] - stack[sp];");
            INDENT; emit(output, "sp--;");
            break;
        }

        case NODE_MULTIPLY: {
            INDENT; emit(output, "stack[sp-1] = stack[sp-1] * stack[sp];");
            INDENT; emit(output, "sp--;");
            break;
        }

        case NODE_DIVIDE: {
            INDENT; emit(output, "if (stack[sp] == 0) {");
            INDENT; emit(output, "    fprintf(stderr, \"Division by zero\\n\");");
            INDENT; emit(output, "    exit(1);");
            INDENT; emit(output, "}");
            INDENT; emit(output, "stack[sp-1] = stack[sp-1] / stack[sp];");
            INDENT; emit(output, "sp--;");
            break;
        }

        case NODE_MODULO: {
            INDENT; emit(output, "if (stack[sp] == 0) {");
            INDENT; emit(output, "    fprintf(stderr, \"Modulo by zero\\n\");");
            INDENT; emit(output, "    exit(1);");
            INDENT; emit(output, "}");
            INDENT; emit(output, "stack[sp-1] = stack[sp-1] %% stack[sp];");
            INDENT; emit(output, "sp--;");
            break;
        }
        
        case NODE_INPUT: { 
            INDENT; emit(output, "printf(\"Input: \");");
            INDENT; emit(output, "scanf(\"%%d\", &stack[++sp]);");
            break;
        }
        
        case NODE_PRINT: {
            INDENT; emit(output, "printf(\"%%d\\n\", stack[sp--]);");
            break;
        }

        case NODE_STORE: {
            INDENT; emit(output, "if (%d >= MEM_SIZE) {", node->value);
            INDENT; emit(output, "    fprintf(stderr, \"Variable index out of bounds\\n\");");
            INDENT; emit(output, "    exit(1);");
            INDENT; emit(output, "}");
            INDENT; emit(output, "memory[%d] = stack[sp--];", node->value);
            break;
        }

        case NODE_LOAD: { 
            INDENT; emit(output, "if (%d >= MEM_SIZE) {", node->value);
            INDENT; emit(output, "    fprintf(stderr, \"Variable index out of bounds\\n\");");
            INDENT; emit(output, "    exit(1);");
            INDENT; emit(output, "}");
            INDENT; emit(output, "stack[++sp] = memory[%d];", node->value);
            break;
        }

        case NODE_STARTIF: {
            compile(node->children[0], output, indent); 
            INDENT; emit(output, "if (stack[sp--]) {");
            for (int i = 1; i < node->child_count; i++) {
                compile(node->children[i], output, indent+1);
            }
            INDENT; emit(output, "}");
            break;
        }

        case NODE_STARTWHILE: {
            ASTNode *condition_block = NULL;
            ASTNode *statements_block = NULL;
            
            for (int i = 0; i < node->child_count; i++) {
                switch (node->children[i]->type) {
                    case NODE_CONDITION: 
                        condition_block = node->children[i]; 
                        break;
                    case NODE_STATEMENTS: 
                        statements_block = node->children[i]; 
                        break;
                    default: 
                        break;
                }
            }
            
            INDENT; emit(output, "while (1) {");
            
            if (condition_block) {
                for (int i = 0; i < condition_block->child_count; i++) {
                    compile(condition_block->children[i], output, indent+1);
                }
                INDENT; emit(output, "    if (!stack[sp--]) break;");
            }
            
            if (statements_block) {
                for (int i = 0; i < statements_block->child_count; i++) {
                    compile(statements_block->children[i], output, indent+1);
                }
            }
            
            for (int i = 0; i < node->child_count; i++) {
                if (node->children[i]->type != NODE_CONDITION && 
                    node->children[i]->type != NODE_STATEMENTS) {
                    compile(node->children[i], output, indent+1);
                }
            }
            
            INDENT; emit(output, "}");
            break;
        }


        case NODE_STARTFOR: {
            ASTNode *init_block = NULL;
            ASTNode *cond_block = NULL;
            ASTNode *update_block = NULL;
            ASTNode *statements_block = NULL;
            
            for (int i = 0; i < node->child_count; i++) {
                switch (node->children[i]->type) {
                    case NODE_INIT: init_block = node->children[i]; break;
                    case NODE_CONDITION: cond_block = node->children[i]; break;
                    case NODE_UPDATE: update_block = node->children[i]; break;
                    case NODE_STATEMENTS: statements_block = node->children[i]; break;
                    default: break;
                }
            }
            
            if (init_block) {
                for (int i = 0; i < init_block->child_count; i++) {
                    compile(init_block->children[i], output, indent);
                }
            }
            
            INDENT; emit(output, "while (1) {");
            
            if (cond_block) {
                for (int i = 0; i < cond_block->child_count; i++) {
                    compile(cond_block->children[i], output, indent+1);
                }
                INDENT; emit(output, "    if (!stack[sp--]) break;");
            }
            
            if (statements_block) {
                for (int i = 0; i < statements_block->child_count; i++) {
                    compile(statements_block->children[i], output, indent+1);
                }
            }
            
            if (update_block) {
                for (int i = 0; i < update_block->child_count; i++) {
                    compile(update_block->children[i], output, indent+1);
                }
            }
            
            INDENT; emit(output, "}");
            break;
        }

        case NODE_INIT:
        case NODE_UPDATE:
        case NODE_CONDITION:
        case NODE_STATEMENTS:
            for (int i = 0; i < node->child_count; i++) {
                compile(node->children[i], output, indent);
            }
            break;
        
        case NODE_ENDIF:
        case NODE_ENDWHILE:
        case NODE_ENDFOR:
        case NODE_ENDFUNCTION:
        case NODE_ENDINIT:
        case NODE_ENDUPDATE:
        case NODE_ENDCONDITION:
        case NODE_ENDSTATEMENTS:
            break;

        case NODE_STARTFUNCTION: {
            emit(output, "void func_%d() {", node->value);
            
            for (int i = 0; i < node->child_count; i++) {
                compile(node->children[i], output, indent+1);
            }
            
            emit(output, "}");
            break;
        }

        case NODE_CALL: {
            INDENT; emit(output, "func_%d();", node->value);
            break;
        }
        
        case NODE_EQUAL: {
            int current_temp = temp_counter++;
            INDENT; emit(output, "int temp%d = (stack[sp-1] == stack[sp]);", current_temp);
            INDENT; emit(output, "sp -= 2;");
            INDENT; emit(output, "stack[++sp] = temp%d;", current_temp);
            break;
        }
        
        case NODE_NOT_EQUAL: {
            int current_temp = temp_counter++;
            INDENT; emit(output, "int temp%d = (stack[sp-1] != stack[sp]);", current_temp);
            INDENT; emit(output, "sp -= 2;");
            INDENT; emit(output, "stack[++sp] = temp%d;", current_temp);
            break;
        }
        
        case NODE_LESS: {
            int current_temp = temp_counter++;
            INDENT; emit(output, "int temp%d = (stack[sp-1] < stack[sp]);", current_temp);
            INDENT; emit(output, "sp -= 2;");
            INDENT; emit(output, "stack[++sp] = temp%d;", current_temp);
            break;
        }

        case NODE_GREATER: {
            int current_temp = temp_counter++;
            INDENT; emit(output, "int temp%d = (stack[sp-1] > stack[sp]);", current_temp);
            INDENT; emit(output, "sp -= 2;");
            INDENT; emit(output, "stack[++sp] = temp%d;", current_temp);
            break;
        }
        
        case NODE_LESS_EQUAL: {
            int current_temp = temp_counter++;
            INDENT; emit(output, "int temp%d = (stack[sp-1] <= stack[sp]);", current_temp);
            INDENT; emit(output, "sp -= 2;");
            INDENT; emit(output, "stack[++sp] = temp%d;", current_temp);
            break;
        }

        case NODE_GREATER_EQUAL: {
            int current_temp = temp_counter++;
            INDENT; emit(output, "int temp%d = (stack[sp-1] >= stack[sp]);", current_temp);
            INDENT; emit(output, "sp -= 2;");
            INDENT; emit(output, "stack[++sp] = temp%d;", current_temp);
            break;
        }
      
        case NODE_AND: {
            int current_temp = temp_counter++;
            INDENT; emit(output, "int temp%d = (stack[sp-1] && stack[sp]);", current_temp);
            INDENT; emit(output, "sp -= 2;");
            INDENT; emit(output, "stack[++sp] = temp%d;", current_temp);
            break;
        }
      
        case NODE_OR: {
            int current_temp = temp_counter++;
            INDENT; emit(output, "int temp%d = (stack[sp-1] || stack[sp]);", current_temp);
            INDENT; emit(output, "sp -= 2;");
            INDENT; emit(output, "stack[++sp] = temp%d;", current_temp);
            break;
        }

        case NODE_NOT: {
            INDENT; emit(output, "stack[sp] = !stack[sp];");
            break;
        }

        case NODE_EXIT:
            INDENT; emit(output, "exit(0);");
            break;

        default:
            INDENT; emit(output, "// Unknown node type: %d", node->type);
            for (int i = 0; i < node->child_count; i++) {
                compile(node->children[i], output, indent);
            }
            break;
    }

    #undef INDENT
}

void execute(ASTNode* ast_root, FILE* output) {
    if (!output) {
        fprintf(stderr, "invalid file pointer in execute()\n");
        return;
    }
    
    compile(ast_root, output, 0);
    
    printf("C code generation completed.\n");
}
