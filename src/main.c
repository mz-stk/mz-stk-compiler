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

    VM vm;

    printf("\nExecuting program:\n");
    execute(&vm, ast);

    free_ast(ast);
    free(tokens);
    free(input);

    return 0;
}
