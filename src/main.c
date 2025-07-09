#include "../include/main.h"

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

    char filename[MAX_PATH_LEN];
    strncpy(filename, argv[1], MAX_PATH_LEN - 1);
    filename[MAX_PATH_LEN - 1] = '\0';
    
    char* base = basename(filename);
    char* dot = strrchr(base, '.');
    if (dot) *dot = '\0'; 

    if (access(argv[1], F_OK) != 0) {
        perror("Input file not found");
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

    char c_filename[MAX_PATH_LEN];
    snprintf(c_filename, sizeof(c_filename), "%s.c", base);
    
    if (access(c_filename, F_OK) == 0) {
        printf("File %s already exists. Overwrite? [y/N] ", c_filename);
        int c = getchar();
        if (c != 'y' && c != 'Y') {
            printf("Aborted\n");
            free_ast(ast);
            free(tokens);
            free(input);
            return 0;
        }
    }
    
    printf("\nCompiling program to C: %s\n", c_filename);
    FILE* c_file = fopen(c_filename, "w");
    if (!c_file) {
        perror("Failed to create C file");
        free_ast(ast);
        free(tokens);
        free(input);
        return 1;
    }
    
    char exec_filename[MAX_PATH_LEN];
    snprintf(exec_filename, sizeof(exec_filename), "%s", base);
    
    execute(ast, c_file);
    fclose(c_file);

    printf("Compiling C to executable: %s\n", exec_filename);
    char compile_command[MAX_PATH_LEN * 2 + 50]; 
    snprintf(compile_command, sizeof(compile_command), 
            "gcc -Wall -Wextra %s -o %s", c_filename, exec_filename);
    
    int compile_result = system(compile_command);
    if (compile_result != 0) {
        fprintf(stderr, "Compilation failed\n");
    } else {
        printf("Successfully compiled to %s\n", exec_filename);
        printf("Run with: ./%s\n", exec_filename);
    }

    free_ast(ast);
    free(tokens);
    free(input);

    return 0;
}
