#ifndef AST_H
#define AST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

typedef struct Node {
    char *type;
    char *value;
    struct Node **children;
    int child_count;
} Node;

typedef struct Symbol {
    char *name;
    char *kind;
    char *type;
    char *value;
    struct Symbol *next;
} Symbol;

typedef struct SymbolTable {
    struct SymbolTable *parent;
    Symbol *symbols;
    int scope_level;
} SymbolTable;

extern SymbolTable *current_table;
extern FILE *symbol_table_file; // 第一阶段输出文件
extern FILE *annotated_ast_file; // 第二阶段输出文件

Node* create_node(const char *type, const char *value, int child_count, ...);
void free_node(Node *node);
SymbolTable* create_symbol_table(SymbolTable *parent);
void insert_symbol(SymbolTable *table, const char *name, const char *kind, const char *type, const char *value);
Symbol* lookup_symbol(SymbolTable *table, const char *name);
void free_symbol_table(SymbolTable *table);
void enter_scope();
void exit_scope();
void check_symbol_table(Node *root);
void print_annotated_ast(Node *node, int indent);
const char* infer_expr_type(Node *expr);

#endif