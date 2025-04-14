#include <stdio.h>
#include "y.tab.h"

// 显式声明 yylex() 和 yytext
extern int yylex(void);
extern char *yytext;
extern int line_number;  // 从 lexer.l 中声明的全局变量
extern int yydebug; // 添加声明
extern FILE *yyin;

FILE *output_file = NULL;

// 定义 yyerror，添加更多上下文
void yyerror(const char *s) {
    fprintf(stderr, "Error at line %d: %s (near token '%s')\n", line_number, s, yytext);
    fprintf(output_file, "Error at line %d: %s (near token '%s')\n", line_number, s, yytext);
}

// 将 TokenType 转换为字符串的辅助函数（从 lexer.l 移到这里）
const char* token_type_to_string(int token) {
    switch (token) {
        case PROGRAM: return "TOK_PROGRAM";
        case VAR: return "TOK_VAR";
        case CONST: return "TOK_CONST";
        case PROCEDURE: return "TOK_PROCEDURE";
        case FUNCTION: return "TOK_FUNCTION";
        case TOKEN_BEGIN: return "TOK_BEGIN";
        case END: return "TOK_END";
        case IF: return "TOK_IF";
        case THEN: return "TOK_THEN";
        case ELSE: return "TOK_ELSE";
        case FOR: return "TOK_FOR";
        case TO: return "TOK_TO";
        case DO: return "TOK_DO";
        case READ: return "TOK_READ";
        case WRITE: return "TOK_WRITE";
        case ARRAY: return "TOK_ARRAY";
        case OF: return "TOK_OF";
        case INTEGER: return "TOK_INTEGER";
        case REAL: return "TOK_REAL";
        case BOOLEAN: return "TOK_BOOLEAN";
        case CHAR: return "TOK_CHAR";
        case DIV: return "TOK_DIV";
        case MOD: return "TOK_MOD";
        case AND: return "TOK_AND";
        case OR: return "TOK_OR";
        case NOT: return "TOK_NOT";
        case IDENTIFIER: return "TOK_ID";
        case INTEGER_CONST: return "TOK_INTEGER_CONST";
        case REAL_CONST: return "TOK_REAL_CONST";
        case CHAR_CONST: return "TOK_CHAR_CONST";
        case BOOLEAN_CONST: return "TOK_BOOL_CONST";
        case ASSIGN: return "TOK_ASSIGN";
        case EQUAL: return "TOK_EQ";
        case NOT_EQUAL: return "TOK_NEQ";
        case LESS: return "TOK_LT";
        case LESS_EQUAL: return "TOK_LEQ";
        case GREATER: return "TOK_GT";
        case GREATER_EQUAL: return "TOK_GEQ";
        case PLUS: return "TOK_PLUS";
        case MINUS: return "TOK_MINUS";
        case MULTIPLY: return "TOK_MUL";
        case DIVIDE: return "TOK_DIVIDE";
        case SEMICOLON: return "TOK_SEMICOLON";
        case COLON: return "TOK_COLON";
        case COMMA: return "TOK_COMMA";
        case LPAREN: return "TOK_LPAREN";
        case RPAREN: return "TOK_RPAREN";
        case LBRACKET: return "TOK_LBRACKET";
        case RBRACKET: return "TOK_RBRACKET";
        case DOT: return "TOK_DOT";
        case DOTDOT: return "TOK_RANGE";
        case EOF_TOKEN: return "TOK_EOF";
        default: return "TOK_UNKNOWN";
    }
}


int main() {
    // 打开输入文件
    FILE *input_file = fopen("input.txt", "r");
    if (!input_file) {
        perror("Cannot open input.txt");
        return 1;
    }
    fseek(input_file, 0, SEEK_END);
    long file_size = ftell(input_file);
    if (file_size == 0) {
        fprintf(stderr, "Error: input.txt is empty\n");
        fclose(input_file);
        return 1;
    }
    rewind(input_file);
    yyin = input_file;

    // 打开输出文件（词法分析和语法分析共用）
    output_file = fopen("output.txt", "w");
    if (!output_file) {
        perror("Cannot open output.txt");
        fclose(input_file);
        return 1;
    }

    // 打印 token 列表
    fprintf(output_file, "Token List:\n");
    int token;
    while ((token = yylex()) != 0) {
        const char* token_name = token_type_to_string(token);
        switch (token) {
            case IDENTIFIER:

                fprintf(output_file, "%s: %s\n", token_name, yylval.str);
                break;
            case INTEGER_CONST:
                fprintf(output_file, "%s: %d\n", token_name, yylval.int_val);
                break;
            case REAL_CONST:
                fprintf(output_file, "%s: %f\n", token_name, yylval.float_val);
                break;
            case CHAR_CONST:
                fprintf(output_file, "%s: %c\n", token_name, yylval.char_val);
                break;
            case BOOLEAN_CONST:
                fprintf(output_file, "%s: %s\n", token_name, yylval.bool_val ? "true" : "false");
                break;
            default:
                fprintf(output_file, "%s: %s\n", token_name, yytext);
                break;
        }
    }

    // 重置输入流以供语法分析使用
    rewind(yyin);
    long pos = ftell(yyin);
    if (pos != 0) {
        fprintf(stderr, "Error: Failed to rewind input file, position = %ld\n", pos);
        fclose(input_file);
        fclose(output_file);
        return 1;
    }

    // 启用 Bison 调试
    yydebug = 1;

    // 解析 token 流
    int result = yyparse();
    fprintf(stderr, "yyparse() returned: %d\n", result);
    if (result == 0) {
        fprintf(output_file, "\nParsing successful!\n");
        fprintf(output_file, "Syntax Tree:\n");
        print_tree(root, 0);
        fprintf(output_file, "\nSymbol Table:\n");
        print_symbol_table(current_table, 0, output_file);

        // 新增逻辑：将详细的语法树信息输出到 ast.txt
        FILE *ast_file = fopen("ast.txt", "w");
        if (!ast_file) {
            fprintf(stderr, "Error: Failed to open ast.txt for writing\n");
            fclose(input_file);
            fclose(output_file);
            return 1;
        }
        fprintf(ast_file, "Abstract Syntax Tree (AST):\n");
        print_detailed_tree(root, NULL, ast_file);  // 输出详细语法树到 ast.txt，根节点的父节点为 NULL
        fclose(ast_file);
    } else {
        fprintf(output_file, "\nParsing failed!\n");
    }

    // 释放语法树内存
    free_tree(root);
    // 释放符号表内存
    free_symbol_table(current_table);
    current_table = NULL;

    // 关闭文件
    fclose(input_file);
    fclose(output_file);
    return 0;
}