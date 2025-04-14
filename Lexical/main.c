#include <stdio.h>
#include <stdlib.h>
#include "tokens.h"

// 定义 yylval 和 yylloc
YYSTYPE yylval;
YYLTYPE yylloc = {1, 1, 1, 1};

extern FILE *yyin;
extern int yylex(void);

const char *token_to_string(int token) {
    switch(token) {
        case TOK_PROGRAM:    return "PROGRAM";
        case TOK_VAR:        return "VAR";
        case TOK_CONST:      return "CONST";
        case TOK_PROCEDURE:  return "PROCEDURE";
        case TOK_FUNCTION:   return "FUNCTION";
        case TOK_BEGIN:      return "BEGIN";
        case TOK_END:        return "END";
        case TOK_IF:         return "IF";
        case TOK_THEN:       return "THEN";
        case TOK_ELSE:       return "ELSE";
        case TOK_FOR:        return "FOR";
        case TOK_TO:         return "TO";
        case TOK_DO:         return "DO";
        case TOK_READ:       return "READ";
        case TOK_WRITE:      return "WRITE";
        case TOK_ARRAY:      return "ARRAY";
        case TOK_OF:         return "OF";
        case TOK_INTEGER:    return "INTEGER";
        case TOK_REAL:       return "REAL";
        case TOK_BOOLEAN:    return "BOOLEAN";
        case TOK_CHAR:       return "CHAR";
        case TOK_DIV:        return "DIV";
        case TOK_MOD:        return "MOD";
        case TOK_AND:        return "AND";
        case TOK_OR:         return "OR";
        case TOK_NOT:        return "NOT";
        case TOK_ID:         return "IDENTIFIER";
        case TOK_INTEGER_CONST: return "INTEGER_CONST";
        case TOK_REAL_CONST: return "REAL_CONST";
        case TOK_CHAR_CONST: return "CHAR_CONST";
        case TOK_BOOL_CONST: return "BOOL_CONST";
        case TOK_ASSIGN:     return "ASSIGN";
        case TOK_EQ:         return "EQ";
        case TOK_NEQ:        return "NEQ";
        case TOK_LT:         return "LT";
        case TOK_LEQ:        return "LEQ";
        case TOK_GT:         return "GT";
        case TOK_GEQ:        return "GEQ";
        case TOK_PLUS:       return "PLUS";
        case TOK_MINUS:      return "MINUS";
        case TOK_MUL:        return "MUL";
        case TOK_DIVIDE:     return "DIVIDE";
        case TOK_SEMICOLON:  return "SEMICOLON";
        case TOK_COLON:      return "COLON";
        case TOK_COMMA:      return "COMMA";
        case TOK_LPAREN:     return "LPAREN";
        case TOK_RPAREN:     return "RPAREN";
        case TOK_LBRACKET:   return "LBRACKET";
        case TOK_RBRACKET:   return "RBRACKET";
        case TOK_DOT:        return "DOT";
        case TOK_RANGE:      return "RANGE";
        case TOK_EOF:        return "EOF";
        case TOK_ERROR:      return "ERROR";
        default:             return "UNKNOWN_TOKEN";
    }
}

int main() {
    FILE *input = fopen("input.text", "r");
    FILE *output = fopen("output.text", "w");
    if (!input || !output) {
        perror("无法打开文件");
        exit(EXIT_FAILURE);
    }
    yyin = input;

    int token;
    while ((token = yylex()) != TOK_EOF) {
        if (token == TOK_ERROR) {
            continue; // 跳过错误 token，避免输出混乱
        }
        fprintf(output, "Token: %-15s", token_to_string(token));
        switch(token) {
            case TOK_ID:
                fprintf(output, " (Value: %s)", yylval.str);
                free(yylval.str); // 释放动态分配的内存
                break;
            case TOK_INTEGER_CONST:
                fprintf(output, " (Value: %d)", yylval.integer_val);
                break;
            case TOK_REAL_CONST:
                fprintf(output, " (Value: %f)", yylval.real_val);
                break;
            case TOK_CHAR_CONST:
                fprintf(output, " (Value: '%c')", yylval.char_val);
                break;
            case TOK_BOOL_CONST:
                fprintf(output, " (Value: %s)", yylval.integer_val ? "true" : "false");
                break;
        }
        fprintf(output, " \tLocation: [%d:%d]-[%d:%d]\n",
                yylloc.first_line, yylloc.first_column,
                yylloc.last_line, yylloc.last_column);
    }

    // 输出 EOF 标记（可选）
    fprintf(output, "Token: %-15s \tLocation: [%d:%d]-[%d:%d]\n",
            token_to_string(TOK_EOF), yylloc.first_line, yylloc.first_column,
            yylloc.last_line, yylloc.last_column);

    fclose(input);
    fclose(output);
    return 0;
}