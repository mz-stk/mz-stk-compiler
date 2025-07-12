#include "../include/lexer.h"

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

            case '^': token.type = TOKEN_INPUT; break;
            case 'V': token.type = TOKEN_PRINT; break;

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
                } else if (c == '!' && isdigit(input[i+1])) {
                    token.type = TOKEN_CALL;
                    token.value = 0;
                    i++;
                    while (i < strlen(input) && isdigit(input[i])) {
                        token.value = token.value * 10 + (input[i] - '0');
                        i++;
                    }
                    i--;
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

            case 'I': token.type = TOKEN_INIT; break;
            case 'i': token.type = TOKEN_ENDINIT; break;
            case 'C': token.type = TOKEN_CONDITION; break;
            case 'c': token.type = TOKEN_ENDCONDITION; break;
            case 'U': token.type = TOKEN_UPDATE; break;
            case 'u': token.type = TOKEN_ENDUPDATE; break;
            case 'L': token.type = TOKEN_STATEMENTS; break;
            case 'l': token.type = TOKEN_ENDSTATEMENTS; break;

            case '@': 
                if (i + 1 >= input_len || !isdigit(input[i + 1])) {
                    fprintf(stderr, "Invalid function name after '@'\n");
                    free(tokens);
                    exit(1);
                }
                token.type = TOKEN_STARTFUNCTION; 
                token.value = 0;
                i++;
                while (i < strlen(input) && isdigit(input[i])) {
                    token.value = token.value * 10 + (input[i] - '0');
                    i++;
                }
                i--;
                break;
        
            case '$': token.type = TOKEN_ENDFUNCTION; break;

            case ':':
                if (i + 1 >= input_len || !isdigit(input[i + 1])) {
                    fprintf(stderr, "Invalid variable name after ':'\n");
                    free(tokens);
                    exit(1);
                }
                token.type = TOKEN_STORE; 
                token.value = 0;
                i++;
                while (i < strlen(input) && isdigit(input[i])) {
                    token.value = token.value * 10 + (input[i] - '0');
                    i++;
                }
                i--;
                break;
            case ';':
                if (i + 1 >= input_len || !isdigit(input[i + 1])) {
                    fprintf(stderr, "Invalid variable name after ';'\n");
                    free(tokens);
                    exit(1);
                }
                token.type = TOKEN_LOAD;
                token.value = 0;
                i++;
                while (i < strlen(input) && isdigit(input[i])) {
                    token.value = token.value * 10 + (input[i] - '0');
                    i++;
                }
                i--;
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
    
    for (int i = 0; i < *token_count; i++) {
        printf("Token %d: type=%d, value=%d\n", i, tokens[i].type, tokens[i].value);
    }

    return tokens;
}
