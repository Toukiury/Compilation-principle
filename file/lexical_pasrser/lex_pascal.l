%top{


#define YY_TYPEDEF_YY_SIZE_T
typedef int yy_size_t;

/* yylex()初始化宏 */
#define YY_USER_INIT yycolumn = 0;

/* token位置跟踪宏 */
#define YY_USER_ACTION                          \
do {                                            \
    yylloc->first_line = yylloc->last_line = yylineno; \
    yylloc->first_column = yycolumn;           \
    yylloc->last_column = yylloc->first_column + yyleng - 1; \
    yycolumn += yyleng;                        \
} while (0);

}

%{
#include <cstring>
#include <cstdio>
/* 包含所需头文件 */

#include "ast/stmt.hpp"
#include "common/log/log.hpp"
#include "yacc_pascal.hpp"

#define TABSIZE 4
#ifndef register
#define register 
#endif

extern int atoi();
extern double atof();

#define RETURN_TOKEN(token) LOG_DEBUG("%s  %s, Line: %d, Column: %d", #token, yytext, yylloc->first_line, yylloc->first_column); return token

char *strdup_tolower(const char *s) {
    char *d = (char *)malloc(strlen(s) + 1);
    if (d == NULL) return NULL;
    for (int i = 0; s[i]; i++) {
        d[i] = tolower((unsigned char)s[i]);
    }
    d[strlen(s)] = '\0';
    return d;
}

%}

/* Flex配置 */
%option noyywrap
%option nounput
%option noinput
%option bison-bridge
%option reentrant
%option yylineno
%option case-insensitive
%option bison-locations

/* 注释状态 */
%x COMMENT_BRACE
%x COMMENT_LINE
%x COMMENT_BLOCK

/* 词法规则 */
DIGIT       [0-9]
INTEGER     {DIGIT}+
REAL        {DIGIT}+"."{DIGIT}+

LETTER      [a-zA-Z]
IDENTIFIER  ({LETTER}|[_])({LETTER}|{DIGIT}|[_])*

CHAR        (\'([^'\n]|\\.)\')

BRACE_PAIR  (\]\[)
OTHER_SIGN  [\~\;\:\.\,\+\-\*\/\(\)\[\]\=\>\<\&\|\!]

%%

/* 关键字 */
"program"       RETURN_TOKEN(PROGRAM);
"const"         RETURN_TOKEN(CONST);
"type"          RETURN_TOKEN(TYPE);
"record"        RETURN_TOKEN(RECORD);
"array"         RETURN_TOKEN(ARRAY);
"of"            RETURN_TOKEN(OF);
"var"           RETURN_TOKEN(VAR);
"function"      RETURN_TOKEN(FUNCTION);
"procedure"     RETURN_TOKEN(PROCEDURE);
"begin"         RETURN_TOKEN(BEGIN_TOKEN);
"end"           RETURN_TOKEN(END);
"if"            RETURN_TOKEN(IF);
"then"          RETURN_TOKEN(THEN);
"else"          RETURN_TOKEN(ELSE);
"case"          RETURN_TOKEN(CASE);
"while"         RETURN_TOKEN(WHILE);
"repeat"        RETURN_TOKEN(REPEAT);
"until"         RETURN_TOKEN(UNTIL);
"for"           RETURN_TOKEN(FOR);
"to"            RETURN_TOKEN(TO);
"downto"        RETURN_TOKEN(DOWNTO);
"do"            RETURN_TOKEN(DO);
"read"          RETURN_TOKEN(READ);
"readln"        RETURN_TOKEN(READLN);
"break"         RETURN_TOKEN(BREAK);
"continue"      RETURN_TOKEN(CONTINUE);
"write"         RETURN_TOKEN(WRITE);
"writeln"       RETURN_TOKEN(WRITELN);
"char"          RETURN_TOKEN(CHAR_KW);
"integer"       RETURN_TOKEN(INTEGER_KW);
"real"          RETURN_TOKEN(REAL_KW);
"boolean"       RETURN_TOKEN(BOOLEAN_KW);

/* 布尔值 */
"true"          { yylval->boolean = true;  return BOOLEAN; }
"false"         { yylval->boolean = false; return BOOLEAN; }

/* 运算符 */
"not"           RETURN_TOKEN(NOT);
"div"           RETURN_TOKEN(DIV);
"mod"           RETURN_TOKEN(MOD);
"and"           RETURN_TOKEN(AND);
"or"            RETURN_TOKEN(OR);
"<>"            RETURN_TOKEN(NE);
"<="            RETURN_TOKEN(LE);
">="            RETURN_TOKEN(GE);
":="            RETURN_TOKEN(ASSIGNOP);
"in"            RETURN_TOKEN(IN);
"or else"       RETURN_TOKEN(ORELSE);
"and then"      RETURN_TOKEN(ANDTHEN);

/* 符号 */
{BRACE_PAIR}    RETURN_TOKEN(BRACE_PAIR);
".."            RETURN_TOKEN(DOUBLE_DOT);
{OTHER_SIGN}    RETURN_TOKEN(yytext[0]);

/* 常量 */
{INTEGER}       { yylval->number = atoi(yytext); RETURN_TOKEN(INTEGER); }
{REAL}          { yylval->real = strdup(yytext); RETURN_TOKEN(REAL); }
{CHAR}          { yylval->charactor = yytext[1]; RETURN_TOKEN(CHAR); }
{IDENTIFIER}    { yylval->string = strdup_tolower(yytext); RETURN_TOKEN(IDENTIFIER); }

/* 注释处理 */
"{"             { BEGIN(COMMENT_BRACE); }
"//"            { BEGIN(COMMENT_LINE); }
"/*"            { BEGIN(COMMENT_BLOCK); }

<COMMENT_BRACE>{
    "}"         { BEGIN(INITIAL); }
    [^}]*       { /* 忽略 */ }
    <<EOF>>     { 
        LOG_DEBUG("Error: 未闭合的大括号注释, Line: %d", yylineno);
        BEGIN(INITIAL); 
    }
}

<COMMENT_LINE>{
    \n          { BEGIN(INITIAL); }
    [^\n]*      { /* 忽略 */ }
    <<EOF>>     { BEGIN(INITIAL); }
}

<COMMENT_BLOCK>{
    "*/"        { BEGIN(INITIAL); }
    [^*]*       { /* 忽略 */ }
    "*"[^/]     { /* 忽略 */ }
    <<EOF>>     { 
        LOG_DEBUG("Error: 未闭合的块注释, Line: %d", yylineno);
        BEGIN(INITIAL); 
    }
}

/* 空白与字符串 */
[ \t\r\n]       { /* 忽略 */ }
\"[^"]*\"       { yylval->string = strdup(yytext); RETURN_TOKEN(STRING); }
'[^']*\'        { yylval->string = strdup(yytext); RETURN_TOKEN(STRING); }

/* 其他 */
.               { /* 忽略 */ }

%%

void scan_string(const char *str, yyscan_t scanner) {
    yy_switch_to_buffer(yy_scan_string(str, scanner), scanner);
    yyset_lineno(1, scanner);
}