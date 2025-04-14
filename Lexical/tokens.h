#ifndef TOKENS_H
#define TOKENS_H

#include <stdio.h>

typedef enum {
    // 保留字
    TOK_PROGRAM, TOK_VAR, TOK_CONST, TOK_PROCEDURE, TOK_FUNCTION,
    TOK_BEGIN, TOK_END, TOK_IF, TOK_THEN, TOK_ELSE, TOK_FOR, TOK_TO, TOK_DO,
    TOK_READ, TOK_WRITE, TOK_ARRAY, TOK_OF, TOK_INTEGER, TOK_REAL, TOK_BOOLEAN, TOK_CHAR,
    TOK_DIV, TOK_MOD, TOK_AND, TOK_OR, TOK_NOT,

    // 标识符和常量
    TOK_ID, TOK_INTEGER_CONST, TOK_REAL_CONST, TOK_CHAR_CONST, TOK_BOOL_CONST,

    // 运算符
    TOK_ASSIGN, TOK_EQ, TOK_NEQ, TOK_LT, TOK_LEQ, TOK_GT, TOK_GEQ,
    TOK_PLUS, TOK_MINUS, TOK_MUL, TOK_DIVIDE,

    // 分隔符
    TOK_SEMICOLON, TOK_COLON, TOK_COMMA, TOK_LPAREN, TOK_RPAREN,
    TOK_LBRACKET, TOK_RBRACKET, TOK_DOT, TOK_LBRACE, TOK_RBRACE, TOK_RANGE,

    // 特殊符号
    TOK_EOF, TOK_ERROR
} TokenType;

typedef union {
    int integer_val;
    float real_val;
    char char_val;
    char* str;
} YYSTYPE;

typedef struct YYLTYPE {
    int first_line;
    int first_column;
    int last_line;
    int last_column;
} YYLTYPE;

extern YYSTYPE yylval;
extern YYLTYPE yylloc;  // 恢复为全局变量

#endif