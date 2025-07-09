#include "../include/compiler.h"

void compile(ASTNode* node, VM* vm) {
    if (!node) return; 

    switch (node->type) {
        case NODE_PROGRAM: 
            for (int i = 0; i < node->child_count; i++) {
                compile(node->children[i], vm);
            }
            break;

        case NODE_START:
            for (int i = 0; i < node->child_count; i++) {
                compile(node->children[i], vm);
            }
            break;
        
        case NODE_PUSH:
            vm->stack[++vm->sp] = node->value;
            break;

        case NODE_ADD: {
            int a = vm->stack[vm->sp--];
            int b = vm->stack[vm->sp--];
            vm->stack[++vm->sp] = b + a;
            break;
        }
        
        case NODE_SUBTRACT: {
            int a = vm->stack[vm->sp--];
            int b = vm->stack[vm->sp--];
            vm->stack[++vm->sp] = b - a;
            break;
        }

        case NODE_MULTIPLY: {
            int a = vm->stack[vm->sp--];
            int b = vm->stack[vm->sp--];
            vm->stack[++vm->sp] = b * a;
            break;
        }

        case NODE_DIVIDE: {
            int a = vm->stack[vm->sp--];
            if (a == 0) {
                fprintf(stderr, "Division by zero\n");
                exit(1);
            }
            int b = vm->stack[vm->sp--];
            vm->stack[++vm->sp] = b / a;
            break;
        }

        case NODE_MODULO: {
            int a = vm->stack[vm->sp--];
            if (a == 0) {
                fprintf(stderr, "Modulo by zero\n");
                exit(1);
            }
            int b = vm->stack[vm->sp--];
            vm->stack[++vm->sp] = b % a;
            break;   
        }
        
        case NODE_INPUT: { 
            int input;
            printf("Input: ");
            scanf("%d", &input);
            vm->stack[++vm->sp] = input;
            break;
        }
        
        case NODE_PRINT: {
            int val = vm->stack[vm->sp--];
            printf("%d\n", val);
            break;
        }

        case NODE_STORE: {
            int val = vm->stack[vm->sp--];
            if (node->value >= MEMORY_SIZE) {
                fprintf(stderr, "Variable index out of bounds\n");
                exit(1);
            }
            vm->memory[node->value] = val;
            break;
        }

        case NODE_LOAD: { 
            if (node->value >= MEMORY_SIZE) {
                fprintf(stderr, "Variable index out of bounds\n");
                exit(1);
            }
            vm->stack[++vm->sp] = vm->memory[node->value];
            break;
        }

        case NODE_STARTIF: {
            if (node->children[0]->type == NODE_CONDITION) {
                compile(node->children[0], vm);  
            } else {
                compile(node->children[0], vm);  
            }
    
            int condition = vm->stack[vm->sp--];
    
            if (condition) {
                for (int i = 1; i < node->child_count; i++) {
                    compile(node->children[i], vm);
                }
            }
            break;
        }

        case NODE_STARTWHILE: {
            while (1) {
                if (node->children[0]->type == NODE_CONDITION) {
                    compile(node->children[0], vm);
                } else {
                    compile(node->children[0], vm);
                }
                
                int condition = vm->stack[vm->sp--];
                if (!condition) break;

                for (int i = 1; i < node->child_count; i++) {
                    compile(node->children[i], vm);
                }
            }
            break;
        }

        case NODE_STARTFOR: {
            compile(node->children[0], vm);  
            
            while (1) {
                compile(node->children[2], vm);  
                int condition = vm->stack[vm->sp--];
                if (!condition) break;

                compile(node->children[3], vm);  
                
                compile(node->children[1], vm);  
            }
            break;
        }

        case NODE_INIT:
        case NODE_UPDATE:
        case NODE_CONDITION:
        case NODE_STATEMENTS:
            for (int i = 0; i < node->child_count; i++) {
                compile(node->children[i], vm);
            }
            break;
        

        case NODE_ENDIF:
        case NODE_ENDWHILE:
        case NODE_ENDFOR:
        case NODE_ENDFUNCTION:
        case NODE_ENDSTATEMENTS:
            break;

        case NODE_STARTFUNCTION: {
            vm->functions = realloc(vm->functions, (vm->function_count + 1) * sizeof(ASTNode*));
            vm->functions[vm->function_count++] = node;
            break;
        }

        case NODE_CALL: {
            ASTNode* func = NULL;
            
            for (int i = 0; i < vm->function_count; i++) {
                if (vm->functions[i]->type == NODE_STARTFUNCTION && vm->functions[i]->value == node->value) {
                    func = vm->functions[i];
                    break;
                }
            }

            if (!func) {
                fprintf(stderr, "Function %d not found\n", node->value);
                exit(1);
            }

            for (int i = 1; i < func->child_count; i++) {
                compile(func->children[i], vm);
            }
            break;
        }
        
        case NODE_EQUAL: {
            int a = vm->stack[vm->sp--];
            int b = vm->stack[vm->sp--];
            vm->stack[++vm->sp] = (b == a);
            break;
        }
        
        case NODE_NOT_EQUAL: {
            int a = vm->stack[vm->sp--];
            int b = vm->stack[vm->sp--];
            vm->stack[++vm->sp] = (b != a);
            break;
        }
        
        case NODE_LESS: {
            int a = vm->stack[vm->sp--];
            int b = vm->stack[vm->sp--];
            vm->stack[++vm->sp] = (b < a);
            break;
        }
        
        case NODE_GREATER:{
            int a = vm->stack[vm->sp--];
            int b = vm->stack[vm->sp--];
            vm->stack[++vm->sp] = (b > a);
            break;
        }

        case NODE_LESS_EQUAL: {
            int a = vm->stack[vm->sp--];
            int b = vm->stack[vm->sp--];
            vm->stack[++vm->sp] = (b <= a);
            break;
        }

        case NODE_GREATER_EQUAL: {
            int a = vm->stack[vm->sp--];
            int b = vm->stack[vm->sp--];
            vm->stack[++vm->sp] = (b >= a);
            break;
        }
      
        case NODE_AND: {
            int a = vm->stack[vm->sp--];
            int b = vm->stack[vm->sp--];
            vm->stack[++vm->sp] = (b && a);
            break;
        }
      
        case NODE_OR: {
            int a = vm->stack[vm->sp--];
            int b = vm->stack[vm->sp--];
            vm->stack[++vm->sp] = (b || a);
            break;
        }

        case NODE_NOT: {
            int a = vm->stack[vm->sp--];
            vm->stack[++vm->sp] = !a;
            break;
        }

        case NODE_EXIT:
            return;

        default:
            printf("Unknown node type: %d\n", node->type);
            for (int i = 0; i < node->child_count; i++) {
                compile(node->children[i], vm);
            }
            break;
    }
}

void execute(VM* vm, ASTNode* ast_root) {
    vm->sp = -1;  
    vm->function_count = 0;
    vm->functions = NULL;

    for (int i = 0; i < ast_root->child_count; i++) {
        compile(ast_root->children[i], vm);
    }
    
    if (vm->functions) {
        free(vm->functions);
    }
}
