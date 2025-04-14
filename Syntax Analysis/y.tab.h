
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton interface for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* "%code requires" blocks.  */

/* Line 1676 of yacc.c  */
#line 184 "parser.y"

    typedef struct Node {
        char *type;           // 节点类型（例如 "program", "var_declaration"）
        char *value;          // 节点值（如果有，例如标识符的值）
        struct Node **children; // 子节点数组
        int child_count;      // 子节点数量
    } Node;



/* Line 1676 of yacc.c  */
#line 51 "y.tab.h"

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     OR = 258,
     MINUS = 259,
     PLUS = 260,
     AND = 261,
     MOD = 262,
     DIV = 263,
     DIVIDE = 264,
     MULTIPLY = 265,
     NOT = 266,
     GREATER_EQUAL = 267,
     GREATER = 268,
     LESS_EQUAL = 269,
     LESS = 270,
     NOT_EQUAL = 271,
     EQUAL = 272,
     THEN = 273,
     ELSE = 274,
     LPAREN = 275,
     ASSIGN = 276,
     IDENTIFIER = 277,
     INTEGER_CONST = 278,
     REAL_CONST = 279,
     CHAR_CONST = 280,
     BOOLEAN_CONST = 281,
     PROGRAM = 282,
     VAR = 283,
     CONST = 284,
     PROCEDURE = 285,
     FUNCTION = 286,
     TOKEN_BEGIN = 287,
     END = 288,
     IF = 289,
     FOR = 290,
     TO = 291,
     DO = 292,
     READ = 293,
     WRITE = 294,
     ARRAY = 295,
     OF = 296,
     INTEGER = 297,
     REAL = 298,
     BOOLEAN = 299,
     CHAR = 300,
     SEMICOLON = 301,
     COLON = 302,
     COMMA = 303,
     RPAREN = 304,
     LBRACKET = 305,
     RBRACKET = 306,
     DOT = 307,
     DOTDOT = 308,
     EOF_TOKEN = 309,
     UMINUS = 310
   };
#endif
/* Tokens.  */
#define OR 258
#define MINUS 259
#define PLUS 260
#define AND 261
#define MOD 262
#define DIV 263
#define DIVIDE 264
#define MULTIPLY 265
#define NOT 266
#define GREATER_EQUAL 267
#define GREATER 268
#define LESS_EQUAL 269
#define LESS 270
#define NOT_EQUAL 271
#define EQUAL 272
#define THEN 273
#define ELSE 274
#define LPAREN 275
#define ASSIGN 276
#define IDENTIFIER 277
#define INTEGER_CONST 278
#define REAL_CONST 279
#define CHAR_CONST 280
#define BOOLEAN_CONST 281
#define PROGRAM 282
#define VAR 283
#define CONST 284
#define PROCEDURE 285
#define FUNCTION 286
#define TOKEN_BEGIN 287
#define END 288
#define IF 289
#define FOR 290
#define TO 291
#define DO 292
#define READ 293
#define WRITE 294
#define ARRAY 295
#define OF 296
#define INTEGER 297
#define REAL 298
#define BOOLEAN 299
#define CHAR 300
#define SEMICOLON 301
#define COLON 302
#define COMMA 303
#define RPAREN 304
#define LBRACKET 305
#define RBRACKET 306
#define DOT 307
#define DOTDOT 308
#define EOF_TOKEN 309
#define UMINUS 310




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 1676 of yacc.c  */
#line 216 "parser.y"

    struct Node *node;
    char *str;
    int int_val;
    float float_val;
    char char_val;
    int bool_val;



/* Line 1676 of yacc.c  */
#line 189 "y.tab.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE yylval;


/* "%code provides" blocks.  */

/* Line 1676 of yacc.c  */
#line 194 "parser.y"

    extern FILE *output_file;
    extern struct Node *root;
    extern int yylex(void);
    extern char *yytext;
    extern void yyerror(const char *s);  // 声明 yyerror
    extern int line_number;  // 声明 line_number
    extern int yydebug; // 添加声明
    Node* create_node(const char *type, const char *value);
    void add_child(Node *parent, Node *child);
    void print_tree(Node *node, int level);
    void free_tree(Node *node);
    int yyparse(void);
    // 添加符号表相关声明
    typedef struct SymbolTable SymbolTable;
    extern SymbolTable *current_table;
    void print_symbol_table(SymbolTable *table, int level, FILE *file);
    void free_symbol_table(SymbolTable *table);

    void print_detailed_tree(Node *node, Node *parent, FILE *file);  // 新增函数



/* Line 1676 of yacc.c  */
#line 227 "y.tab.h"
