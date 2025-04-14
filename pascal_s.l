%{
#include "tokens.h"
#include <string.h>
#include <ctype.h>

// 定义位置跟踪宏
#define YY_USER_ACTION \
    yylloc.first_line = yylloc.last_line; \
    yylloc.first_column = yylloc.last_column; \
    for (int i = 0; i < yyleng; i++) { \
        if (yytext[i] == '\n') { \
            yylloc.last_line++; \
            yylloc.last_column = 1; \
        } else { \
            yylloc.last_column++; \
        } \
    }

%}

%option noyywrap
%option yylineno

%x COMMENT

DIGIT     [0-9]
LETTER    [a-zA-Z]
ID        {LETTER}({LETTER}|{DIGIT}){0,7}
INTEGER   {DIGIT}+
REAL      {DIGIT}+"."{DIGIT}+
CHAR      \'[a-zA-Z]\'
BOOL      [tT][rR][uU][eE]|[fF][aA][lL][sS][eE]

%%

<INITIAL>{
    /* 保留字 */
    "program"       { return TOK_PROGRAM; }
    "end"           { return TOK_END; }
    "var"           { return TOK_VAR; }
    "const"         { return TOK_CONST; }
    "procedure"     { return TOK_PROCEDURE; }
    "function"      { return TOK_FUNCTION; }
    "begin"         { return TOK_BEGIN; }
    "if"            { return TOK_IF; }
    "then"          { return TOK_THEN; }
    "else"          { return TOK_ELSE; }
    "for"           { return TOK_FOR; }
    "to"            { return TOK_TO; }
    "do"            { return TOK_DO; }
    "read"          { return TOK_READ; }
    "write"         { return TOK_WRITE; }
    "array"         { return TOK_ARRAY; }
    "of"            { return TOK_OF; }
    "integer"       { return TOK_INTEGER; }
    "real"          { return TOK_REAL; }
    "boolean"       { return TOK_BOOLEAN; }
    "char"          { return TOK_CHAR; }
    "div"           { return TOK_DIV; }
    "mod"           { return TOK_MOD; }
    "and"           { return TOK_AND; }
    "or"            { return TOK_OR; }
    "not"           { return TOK_NOT; }

    /* 布尔常量 */
    {BOOL}          {
        yylval.integer_val = (strcmp(yytext, "true") == 0 || strcmp(yytext, "TRUE") == 0) ? 1 : 0;
        return TOK_BOOL_CONST;
    }

    /* 标识符 */
    {ID}            {
        if (strlen(yytext) > 8) {
            fprintf(stderr, "Lexical Error: Identifier '%s' exceeds maximum length 8 at line %d, column %d\n",
                    yytext, yylloc.first_line, yylloc.first_column);
            return TOK_ERROR;
        }
        char *lower = strdup(yytext);
        for (int i = 0; lower[i]; i++) {
            lower[i] = tolower(lower[i]);
        }
        yylval.str = lower;
        return TOK_ID;
    }

    /* 其他常量 */
    {INTEGER}       {
        yylval.integer_val = atoi(yytext);
        return TOK_INTEGER_CONST;
    }

    {REAL}          {
        yylval.real_val = atof(yytext);
        return TOK_REAL_CONST;
    }

    {CHAR}          {
        yylval.char_val = yytext[1];
        return TOK_CHAR_CONST;
    }

    /* 运算符 */
    ":="            { return TOK_ASSIGN; }
    "="             { return TOK_EQ; }
    "<>"            { return TOK_NEQ; }
    "<="            { return TOK_LEQ; }
    ">="            { return TOK_GEQ; }
    "<"             { return TOK_LT; }
    ">"             { return TOK_GT; }
    "+"             { return TOK_PLUS; }
    "-"             { return TOK_MINUS; }
    "*"             { return TOK_MUL; }
    "/"             { return TOK_DIVIDE; }

    /* 分隔符 */
    ";"             { return TOK_SEMICOLON; }
    ":"             { return TOK_COLON; }
    ","             { return TOK_COMMA; }
    "("             { return TOK_LPAREN; }
    ")"             { return TOK_RPAREN; }
    "["             { return TOK_LBRACKET; }
    "]"             { return TOK_RBRACKET; }
    ".."            { return TOK_RANGE; }
    "."             { return TOK_DOT; }
    "{"             { BEGIN(COMMENT); }
    "}"             {
        fprintf(stderr, "Lexical Error: Unmatched closing brace '}' at line %d, column %d\n",
                yylloc.first_line, yylloc.first_column);
        return TOK_ERROR;
    }

    /* 空白符 */
    [ \t\r]+        { /* 忽略空格和制表符 */ }
    \n              { yylloc.last_line++; yylloc.last_column = 1; }

    /* 文件结束 */
    <<EOF>>         { return TOK_EOF; }

    /* 错误处理 */
    .               {
        fprintf(stderr, "Lexical Error: Illegal character '%s' at line %d, column %d\n",
                yytext, yylloc.first_line, yylloc.first_column);
        return TOK_ERROR;
    }
}

<COMMENT>{
    "{"             {
        fprintf(stderr, "Lexical Error: Nested comment not allowed at line %d, column %d\n",
                yylloc.first_line, yylloc.first_column);
        return TOK_ERROR;
    }
    "}"             { BEGIN(INITIAL); }
    [^\{\}\n]+      { /* 忽略注释内容 */ }
    \n              { yylloc.last_line++; yylloc.last_column = 1; }
    <<EOF>>         {
        fprintf(stderr, "Lexical Error: Unterminated comment at line %d\n", yylloc.first_line);
        return TOK_ERROR;
    }
}

%%