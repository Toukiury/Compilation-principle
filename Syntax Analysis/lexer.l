%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "y.tab.h"

extern FILE *output_file;

int line_number = 1;  // 添加行号跟踪
// 声明 yyerror
extern void yyerror(const char *s);
%}

%option noyywrap

DIGIT   [0-9]
LETTER  [a-zA-Z]

%x COMMENT

%%

"program"       { return PROGRAM; }
"var"           { return VAR; }
"const"         { return CONST; }
"procedure"     { return PROCEDURE; }
"function"      { return FUNCTION; }
"begin"         { return TOKEN_BEGIN; }
"end"           { return END; }
"if"            { return IF; }
"then"          { return THEN; }
"else"          { return ELSE; }
"for"           { return FOR; }
"to"            { return TO; }
"do"            { return DO; }
"read"          { return READ; }
"write"         { return WRITE; }
"array"         { return ARRAY; }
"of"            { return OF; }
"integer"       { return INTEGER; }
"real"          { return REAL; }
"boolean"       { return BOOLEAN; }
"char"          { return CHAR; }
"div"           { return DIV; }
"mod"           { return MOD; }
"and"           { return AND; }
"or"            { return OR; }
"not"           { return NOT; }

"TRUE"          { yylval.bool_val = 1; return BOOLEAN_CONST; }
"FALSE"         { yylval.bool_val = 0; return BOOLEAN_CONST; }

{LETTER}({LETTER}|{DIGIT})* {
    yylval.str = strdup(yytext);
    if (!yylval.str) {
                            fprintf(stderr, "Error: Failed to allocate memory for identifier '%s'\n", yytext);
                            exit(1);
                        }
                        fprintf(stderr, "Found identifier: %s\n", yylval.str);
    return IDENTIFIER;
}

{DIGIT}+ {
    yylval.int_val = atoi(yytext);
    return INTEGER_CONST;
}

{DIGIT}+"."{DIGIT}+ {
    yylval.float_val = atof(yytext);
    return REAL_CONST;
}

\'[a-zA-Z]\' {
    yylval.char_val = yytext[1];
    return CHAR_CONST;
}

"<="    { return LESS_EQUAL; }
">="    { return GREATER_EQUAL; }
"<>"    { return NOT_EQUAL; }
":="    { return ASSIGN; }
"="     { return EQUAL; }
"<"     { return LESS; }
">"     { return GREATER; }
"+"     { return PLUS; }
"-"     { return MINUS; }
"*"     { return MULTIPLY; }
"/"     { return DIVIDE; }

";"     { return SEMICOLON; }
":"     { return COLON; }
","     { return COMMA; }
"("     { return LPAREN; }
")"     { return RPAREN; }
"["     { return LBRACKET; }
"]"     { return RBRACKET; }
"."     { return DOT; }
".."    { return DOTDOT; }

"{"     { BEGIN(COMMENT); }
<COMMENT>"}" { BEGIN(INITIAL); }
<COMMENT>.   { /* 忽略注释内容 */ }
<COMMENT>\n  { line_number++; /* 忽略注释中的换行，但更新行号 */ }

[ \t\r\n]+ {
    /* 处理所有空白字符，包括空格、制表符、回车符和换行符 */
    char *p = yytext;
    while (*p) {
        if (*p == '\n') line_number++;
        p++;
    }
}

.       { fprintf(stderr, "Unrecognized character '%s' at line %d\n", yytext, line_number); }

<<EOF>> { return 0; }

%%

