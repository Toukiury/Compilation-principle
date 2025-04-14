%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define YYDEBUG 1

extern FILE *output_file;  // 与 lexer.l 中的 output_file 一致
struct Node *root = NULL; // 全局变量，存储语法树根节点

extern int yylex();//词法分析器接口，用于从输入流中读取 token
extern void yyerror(const char *s);//错误处理

// 符号表相关定义
typedef struct Symbol {
    char *name;           // 标识符名称
    char *kind;           // 种类：program, var, const, function, procedure
    char *type;           // 数据类型：integer, real, char, boolean, array（仅对变量和常量适用）
    char *value;          // 常量值（仅对常量适用）
    struct Symbol *next;  // 指向同一作用域中的下一个符号
} Symbol;

typedef struct SymbolTable {
    struct SymbolTable *parent;   // 指向父作用域的符号表
    struct SymbolTable *children; // 指向第一个子作用域
    struct SymbolTable *sibling;  // 指向同一父作用域下的下一个兄弟作用域
    Symbol *symbols;              // 当前作用域的符号列表
} SymbolTable;

// 全局符号表（指向当前作用域的符号表）
SymbolTable *current_table = NULL;

// 创建一个新的符号表
SymbolTable *create_symbol_table(SymbolTable *parent) {
    SymbolTable *table = (SymbolTable *)malloc(sizeof(SymbolTable));
    if (!table) {
            fprintf(stderr, "Error: Failed to allocate memory for symbol table\n");
            exit(1);
        }
    table->parent = parent;
    table->children = NULL;
    table->sibling = NULL;
    table->symbols = NULL;
    if (parent) {
        // 将新表添加到父作用域的 children 链表
        table->sibling = parent->children;
        parent->children = table;
    }
    return table;
}

// 插入一个符号到当前符号表
void insert_symbol(const char *name, const char *kind, const char *type, const char *value) {
    Symbol *symbol = (Symbol *)malloc(sizeof(Symbol));
    if (!symbol) {
        fprintf(stderr, "Error: Failed to allocate memory for symbol\n");
        exit(1);
    }
    symbol->name = strdup(name);
    if (!symbol->name) {
        fprintf(stderr, "Error: Failed to allocate memory for symbol name\n");
        free(symbol);
        exit(1);
    }
    symbol->kind = strdup(kind);
    if (!symbol->kind) {
        fprintf(stderr, "Error: Failed to allocate memory for symbol kind\n");
        free(symbol->name);
        free(symbol);
        exit(1);
    }
    symbol->type = type ? strdup(type) : NULL;
    if (type && !symbol->type) {
        fprintf(stderr, "Error: Failed to allocate memory for symbol type\n");
        free(symbol->name);
        free(symbol->kind);
        free(symbol);
        exit(1);
    }
    symbol->value = value ? strdup(value) : NULL;
    if (value && !symbol->value) {
        fprintf(stderr, "Error: Failed to allocate memory for symbol value\n");
        free(symbol->name);
        free(symbol->kind);
        if (symbol->type) free(symbol->type);
        free(symbol);
        exit(1);
    }
    symbol->next = NULL; // 新符号的 next 指针初始化为 NULL

    // 如果符号表为空，直接将新符号作为第一个符号
    if (!current_table->symbols) {
        current_table->symbols = symbol;
    } else {
        // 遍历到链表尾部
        Symbol *current = current_table->symbols;
        while (current->next) {
            current = current->next;
        }
        // 将新符号追加到尾部
        current->next = symbol;
    }
}

// 查找符号（从当前作用域开始，向上查找）
Symbol *lookup_symbol(const char *name) {
    if (!name) {
        fprintf(stderr, "Error: lookup_symbol called with NULL name\n");
        return NULL;
    }
    fprintf(stderr, "Looking up symbol: %s\n", name);
    SymbolTable *table = current_table;
    if (!table) {
        fprintf(stderr, "Error: current_table is NULL in lookup_symbol\n");
        return NULL;
    }
    while (table != NULL) {
        fprintf(stderr, "Checking symbol table at level %p\n", (void *)table);
        for (Symbol *symbol = table->symbols; symbol != NULL; symbol = symbol->next) {
            fprintf(stderr, "Checking symbol: %s\n", symbol->name ? symbol->name : "NULL");
            if (symbol->name && strcmp(symbol->name, name) == 0) {
                fprintf(stderr, "Found symbol: %s\n", name);
                return symbol;
            }
        }
        table = table->parent;
    }
    fprintf(stderr, "Symbol not found: %s\n", name);
    return NULL;
}

// 打印符号表（递归打印所有作用域）
void print_symbol_table(SymbolTable *table, int level, FILE *file) {
    if (table == NULL) return;

    // 打印当前作用域的符号
    for (int i = 0; i < level; i++) fprintf(file, "  ");
    fprintf(file, "Scope (Level %d):\n", level);
    for (Symbol *symbol = table->symbols; symbol != NULL; symbol = symbol->next) {
        for (int i = 0; i < level; i++) fprintf(file, "  ");
        fprintf(file, "  Name: %s, Kind: %s", symbol->name, symbol->kind);
        if (symbol->type) fprintf(file, ", Type: %s", symbol->type);
        if (symbol->value) fprintf(file, ", Value: %s", symbol->value);
        fprintf(file, "\n");
    }

    // 递归打印子作用域
    for (SymbolTable *child = table->children; child != NULL; child = child->sibling) {
        print_symbol_table(child, level + 1, file);
    }
}

// 释放符号表内存
void free_symbol_table(SymbolTable *table) {
    if (table == NULL) return;

    // 释放子作用域
    SymbolTable *child = table->children;
    while (child != NULL) {
        SymbolTable *next_child = child->sibling;
        free_symbol_table(child);
        child = next_child;
    }

    // 释放当前作用域的符号
    Symbol *symbol = table->symbols;
    while (symbol != NULL) {
        Symbol *next = symbol->next;
        free(symbol->name);
        free(symbol->kind);
        if (symbol->type) free(symbol->type);
        if (symbol->value) free(symbol->value);
        free(symbol);
        symbol = next;
    }

    free(table);
}


%}

// 将 Node 结构体的定义放入 y.tab.h
%code requires {
    typedef struct Node {
        char *type;           // 节点类型（例如 "program", "var_declaration"）
        char *value;          // 节点值（如果有，例如标识符的值）
        struct Node **children; // 子节点数组
        int child_count;      // 子节点数量
    } Node;
}

// 将全局变量和函数声明放入 y.tab.h
%code provides {
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
}

%union {
    struct Node *node;
    char *str;
    int int_val;
    float float_val;
    char char_val;
    int bool_val;
}

%left OR
%left PLUS MINUS
%left MULTIPLY DIVIDE DIV MOD AND
%nonassoc NOT
%nonassoc EQUAL NOT_EQUAL LESS LESS_EQUAL GREATER GREATER_EQUAL
%nonassoc THEN
%nonassoc ELSE
%nonassoc LPAREN
%nonassoc ASSIGN

%expect 1

%token <str> IDENTIFIER
%token <int_val> INTEGER_CONST
%token <float_val> REAL_CONST
%token <char_val> CHAR_CONST
%token <bool_val> BOOLEAN_CONST
%token PROGRAM VAR CONST PROCEDURE FUNCTION TOKEN_BEGIN END IF THEN ELSE FOR TO DO READ WRITE
%token ARRAY OF INTEGER REAL BOOLEAN CHAR DIV MOD AND OR NOT
%token EQUAL NOT_EQUAL LESS LESS_EQUAL GREATER GREATER_EQUAL
%token PLUS MINUS MULTIPLY DIVIDE ASSIGN
%token SEMICOLON COLON COMMA LPAREN RPAREN LBRACKET RBRACKET DOT DOTDOT
%token EOF_TOKEN

%type <node> program program_head program_body idlist const_declarations const_declaration
%type <node> const_value var_declarations var_declaration type basic_type period
%type <node> subprogram_declarations subprogram subprogram_head formal_parameter
%type <node> parameter_list parameter var_parameter value_parameter subprogram_body
%type <node> compound_statement statement_list statement variable_list variable
%type <node> id_varpart expression_list function_call else_part expression
%type <node> simple_expression term factor
%start program
%%

/* 语法规则 */

/* program_head -> program id ( idlist ) | program id
 * 定义 Pascal 程序头的文法规则，支持两种形式：
 * 1. program id ( idlist )：带参数列表的程序头，例如 program main(input, output)
 * 2. program id：不带参数的程序头，例如 program main
 */
program_head:
    PROGRAM IDENTIFIER LPAREN idlist RPAREN
    {
        // 创建全局符号表
        current_table = create_symbol_table(NULL);  // 调用 create_symbol_table(NULL) 创建一个新的符号表，作为全局作用域，传入 NULL 表示没有父作用域（这是最顶层作用域），并赋值给全局变量 current_table
        fprintf(stderr, "Created symbol table for program: %s\n", $2);  // 打印调试信息，显示创建符号表时的程序名（$2 是 IDENTIFIER 的值，例如 "main"），输出到 stderr 便于调试

        // 插入程序名到符号表
        insert_symbol($2, "program", NULL, NULL);  // 调用 insert_symbol 将程序名插入符号表
        // 参数说明：
        // $2：程序名（IDENTIFIER 的值，例如 "main"）
        // "program"：符号种类，表示这是一个程序名
        // NULL：类型，程序名没有类型，设为 NULL
        // NULL：值，程序名没有值，设为 NULL

        // 构建语法树节点
        Node *node = create_node("program_head", NULL);  // 创建一个类型为 "program_head" 的节点，value 设为 NULL（program_head 本身没有值）
        add_child(node, create_node("program", "program"));  // 添加子节点：类型为 "program"，值设为 "program"，表示关键字 program
        add_child(node, create_node("id", $2));  // 添加子节点：类型为 "id"，值设为程序名（$2，例如 "main"）
        add_child(node, $4);  // 添加子节点：$4 是 idlist，表示参数列表（例如 input, output）
        $$ = node;  // 设置当前规则的返回值 $$ 为创建的节点，供上层规则（如 program）使用
    }
    | PROGRAM IDENTIFIER
    {
        // 创建全局符号表
        current_table = create_symbol_table(NULL);  // 同上，创建全局符号表并赋值给 current_table
        fprintf(stderr, "Created symbol table for program: %s\n", $2);  // 同上，打印调试信息

        // 插入程序名到符号表
        insert_symbol($2, "program", NULL, NULL);  // 同上，插入程序名到符号表

        // 构建语法树节点
        Node *node = create_node("program_head", NULL);  // 创建 "program_head" 节点，value 设为 NULL
        add_child(node, create_node("program", "program"));  // 添加子节点：表示关键字 program
        add_child(node, create_node("id", $2));  // 添加子节点：表示程序名（$2）
        $$ = node;  // 设置返回值 $$ 为创建的节点
    }
    ;

/* program_body -> const_declarations var_declarations subprogram_declarations compound_statement
 * 定义 Pascal 程序体的文法规则，程序体由以下部分组成：
 * 1. const_declarations：常量声明部分（可以为空）
 * 2. var_declarations：变量声明部分（可以为空）
 * 3. subprogram_declarations：子程序声明部分（可以为空）
 * 4. compound_statement：复合语句（主程序体，例如 begin ... end）
 */
program_body:
    const_declarations var_declarations subprogram_declarations compound_statement
    {
        // 构建语法树节点
        Node *node = create_node("program_body", NULL);  // 创建一个类型为 "program_body" 的节点，value 设为 NULL（program_body 本身没有值）
        add_child(node, $1);  // 添加子节点：$1 是 const_declarations，表示常量声明部分
        add_child(node, $2);  // 添加子节点：$2 是 var_declarations，表示变量声明部分
        add_child(node, $3);  // 添加子节点：$3 是 subprogram_declarations，表示子程序声明部分
        add_child(node, $4);  // 添加子节点：$4 是 compound_statement，表示主程序体
        $$ = node;  // 设置当前规则的返回值 $$ 为创建的节点，供上层规则（如 program）使用
    }
    ;

/* program -> program_head ; program_body .
 * 定义 Pascal 程序的顶层文法规则，程序由以下部分组成：
 * 1. program_head：程序头（例如 program main;）
 * 2. SEMICOLON：分号 ;（分隔符）
 * 3. program_body：程序体（包含声明和语句）
 * 4. DOT：结束点 .（表示程序结束）
 */
program:
    program_head SEMICOLON program_body DOT
    {
        // 构建语法树根节点
        Node *node = create_node("program", NULL);  // 创建一个类型为 "program" 的节点，作为整个语法树的根节点，value 设为 NULL（program 本身没有值）
        add_child(node, $1);  // 添加子节点：$1 是 program_head，表示程序头
        add_child(node, $3);  // 添加子节点：$3 是 program_body，表示程序体
        // 注意：SEMICOLON 和 DOT 是分隔符，未添加到语法树中，因为它们不携带语义信息
        root = node;  // 将创建的节点赋值给全局变量 root，作为语法树的根节点，后续可通过 root 打印或释放语法树
        $$ = node;  // 设置当前规则的返回值 $$ 为创建的节点（program 是起始符号，$$ 通常不会被使用，但仍需设置）
    }
    ;


/* const_value -> + num | - num | num | ' letter '
 * 定义 Pascal 语言中常量值的文法规则，支持以下形式：
 * 1. + num：带正号的数值（整数或实数），例如 +42 或 +3.14
 * 2. - num：带负号的数值（整数或实数），例如 -42 或 -3.14
 * 3. num：无符号的数值（整数或实数），例如 42 或 3.14
 * 4. ' letter '：字符常量，例如 'A'
 * 其中 num 可以是 INTEGER_CONST（整数常量）或 REAL_CONST（实数常量）
 */
const_value:
    PLUS INTEGER_CONST
    {
        // 将正号和整数值格式化为字符串
        char value[32];  // 定义一个字符数组，用于存储格式化后的字符串值
        snprintf(value, sizeof(value), "+%d", $2);  // 使用 snprintf 格式化正整数值，$2 是 INTEGER_CONST 的值（整数，例如 42），格式为 "+42"
        $$ = create_node("const_value", value);  // 创建一个类型为 "const_value" 的语法树节点，value 字段设为格式化后的字符串（例如 "+42"）
    }
    | PLUS REAL_CONST
    {
        // 将正号和实数值格式化为字符串
        char value[32];  // 定义字符数组，用于存储格式化后的字符串值
        snprintf(value, sizeof(value), "+%.6f", $2);  // 使用 snprintf 格式化正实数值，$2 是 REAL_CONST 的值（浮点数，例如 3.14），格式为 "+3.140000"（保留 6 位小数）
        $$ = create_node("const_value", value);  // 创建 "const_value" 节点，value 字段设为格式化后的字符串（例如 "+3.140000"）
    }
    | MINUS INTEGER_CONST
    {
        // 将负号和整数值格式化为字符串
        char value[32];  // 定义字符数组，用于存储格式化后的字符串值
        snprintf(value, sizeof(value), "-%d", $2);  // 使用 snprintf 格式化负整数值，$2 是 INTEGER_CONST 的值（整数，例如 42），格式为 "-42"
        $$ = create_node("const_value", value);  // 创建 "const_value" 节点，value 字段设为格式化后的字符串（例如 "-42"）
    }
    | MINUS REAL_CONST
    {
        // 将负号和实数值格式化为字符串
        char value[32];  // 定义字符数组，用于存储格式化后的字符串值
        snprintf(value, sizeof(value), "-%.6f", $2);  // 使用 snprintf 格式化负实数值，$2 是 REAL_CONST 的值（浮点数，例如 3.14），格式为 "-3.140000"（保留 6 位小数）
        $$ = create_node("const_value", value);  // 创建 "const_value" 节点，value 字段设为格式化后的字符串（例如 "-3.140000"）
    }
    | INTEGER_CONST
    {
        // 将无符号整数值格式化为字符串
        char value[32];  // 定义字符数组，用于存储格式化后的字符串值
        snprintf(value, sizeof(value), "%d", $1);  // 使用 snprintf 格式化整数值，$1 是 INTEGER_CONST 的值（整数，例如 42），格式为 "42"
        $$ = create_node("const_value", value);  // 创建 "const_value" 节点，value 字段设为格式化后的字符串（例如 "42"）
    }
    | REAL_CONST
    {
        // 将无符号实数值格式化为字符串
        char value[32];  // 定义字符数组，用于存储格式化后的字符串值
        snprintf(value, sizeof(value), "%.6f", $1);  // 使用 snprintf 格式化实数值，$1 是 REAL_CONST 的值（浮点数，例如 3.14），格式为 "3.140000"（保留 6 位小数）
        $$ = create_node("const_value", value);  // 创建 "const_value" 节点，value 字段设为格式化后的字符串（例如 "3.140000"）
    }
    | CHAR_CONST
    {
        // 将字符常量格式化为字符串
        char value[2] = { $1, '\0' };  // 定义字符数组，$1 是 CHAR_CONST 的值（单个字符，例如 'A'），添加终止符 '\0' 形成字符串
        $$ = create_node("const_value", value);  // 创建 "const_value" 节点，value 字段设为字符值（例如 "A"）
    }
    ;

/* basic_type -> integer | real | boolean | char
 * 定义 Pascal 语言中基本类型的文法规则，支持以下四种基本类型：
 * 1. integer：整数类型，例如 integer
 * 2. real：实数类型，例如 real
 * 3. boolean：布尔类型，例如 boolean
 * 4. char：字符类型，例如 char
 * 这些基本类型用于变量声明或类型定义（例如 var x: integer;）
 */
basic_type:
    INTEGER
    {
        // 创建表示整数类型的语法树节点
        $$ = create_node("basic_type", "integer");  // 创建一个类型为 "basic_type" 的语法树节点，value 字段设为 "integer"，表示基本类型是整数类型
        // $$ 是当前规则的返回值，将该节点传递给上层规则（例如 var_declaration）
    }
    | REAL
    {
        // 创建表示实数类型的语法树节点
        $$ = create_node("basic_type", "real");  // 创建一个类型为 "basic_type" 的语法树节点，value 字段设为 "real"，表示基本类型是实数类型
    }
    | BOOLEAN
    {
        // 创建表示布尔类型的语法树节点
        $$ = create_node("basic_type", "boolean");  // 创建一个类型为 "basic_type" 的语法树节点，value 字段设为 "boolean"，表示基本类型是布尔类型
    }
    | CHAR
    {
        // 创建表示字符类型的语法树节点
        $$ = create_node("basic_type", "char");  // 创建一个类型为 "basic_type" 的语法树节点，value 字段设为 "char"，表示基本类型是字符类型
    }
    ;

/* period -> digits .. digits | period , digits .. digits
 * 定义 Pascal 语言中范围（period）的文法规则，用于表示数组索引范围或子范围（subrange），支持以下两种形式：
 * 1. digits .. digits：单个范围，例如 1..10，表示从 1 到 10 的范围
 * 2. period , digits .. digits：多个范围的递归定义，例如 1..10, 20..30，表示多个范围的集合
 * 其中 digits 是 INTEGER_CONST（整数常量），DOTDOT 表示范围符号 ..
 */
period:
    INTEGER_CONST DOTDOT INTEGER_CONST
    {
        // 构建单个范围的语法树节点
        Node *node = create_node("period", NULL);  // 创建一个类型为 "period" 的语法树节点，value 设为 NULL（period 本身没有值，仅作为范围的容器）

        // 格式化范围的起始值和结束值为字符串
        char start[32], end[32];  // 定义两个字符数组，分别用于存储范围的起始值和结束值
        snprintf(start, sizeof(start), "%d", $1);  // 使用 snprintf 格式化起始值，$1 是第一个 INTEGER_CONST 的值（整数，例如 1），格式为 "1"
        snprintf(end, sizeof(end), "%d", $3);  // 使用 snprintf 格式化结束值，$3 是第二个 INTEGER_CONST 的值（整数，例如 10），格式为 "10"

        // 添加子节点表示范围的起始值和结束值
        add_child(node, create_node("start", start));  // 添加子节点：类型为 "start"，value 设为格式化后的起始值（例如 "1"）
        add_child(node, create_node("end", end));  // 添加子节点：类型为 "end"，value 设为格式化后的结束值（例如 "10"）

        $$ = node;  // 设置当前规则的返回值 $$ 为创建的节点，供上层规则（例如 array_type）使用
    }
    | period COMMA INTEGER_CONST DOTDOT INTEGER_CONST
    {
        // 构建多个范围的语法树节点（递归定义）
        Node *node = create_node("period", NULL);  // 创建一个类型为 "period" 的语法树节点，value 设为 NULL（period 本身没有值，仅作为范围的容器）

        // 添加前一个 period 节点作为子节点
        add_child(node, $1);  // 添加子节点：$1 是前一个 period，表示之前的范围集合（例如 1..10）

        // 格式化新范围的起始值和结束值为字符串
        char start[32], end[32];  // 定义两个字符数组，分别用于存储新范围的起始值和结束值
        snprintf(start, sizeof(start), "%d", $3);  // 使用 snprintf 格式化起始值，$3 是第三个 INTEGER_CONST 的值（整数，例如 20），格式为 "20"
        snprintf(end, sizeof(end), "%d", $5);  // 使用 snprintf 格式化结束值，$5 是第五个 INTEGER_CONST 的值（整数，例如 30），格式为 "30"

        // 添加子节点表示新范围的起始值和结束值
        add_child(node, create_node("start", start));  // 添加子节点：类型为 "start"，value 设为格式化后的起始值（例如 "20"）
        add_child(node, create_node("end", end));  // 添加子节点：类型为 "end"，value 设为格式化后的结束值（例如 "30"）

        $$ = node;  // 设置当前规则的返回值 $$ 为创建的节点，供上层规则或递归调用使用
    }
    ;

/* type -> basic_type | array [ period ] of basic_type
 * 定义 Pascal 语言中类型（type）的文法规则，支持以下两种形式：
 * 1. basic_type：基本类型，例如 integer、real、boolean、char
 * 2. array [ period ] of basic_type：数组类型，例如 array [1..10] of integer，表示一个元素类型为 integer、索引范围为 1..10 的数组
 * 该规则用于变量声明或类型定义（例如 var x: array [1..10] of integer;）
 */
type:
    basic_type
    {
        // 直接使用基本类型的语法树节点
        $$ = $1;  // 将 basic_type 的节点直接作为 type 的返回值，$1 是 basic_type 的语法树节点（例如 basic_type (integer)）
        // 这种形式表示类型是一个基本类型，无需额外构建新节点
    }
    | ARRAY LBRACKET period RBRACKET OF basic_type
    {
        // 构建数组类型的语法树节点
        Node *node = create_node("array_type", NULL);  // 创建一个类型为 "array_type" 的语法树节点，value 设为 NULL（array_type 本身没有值，仅作为数组类型的容器）

        // 添加子节点表示数组的索引范围和元素类型
        add_child(node, $3);  // 添加子节点：$3 是 period，表示数组的索引范围（例如 1..10）
        add_child(node, $6);  // 添加子节点：$6 是 basic_type，表示数组元素的类型（例如 integer）

        $$ = node;  // 设置当前规则的返回值 $$ 为创建的节点，供上层规则（例如 var_declaration）使用
    }
    ;

/* idlist -> id | idlist , id
 * 定义 Pascal 语言中标识符列表（idlist）的文法规则，支持以下两种形式：
 * 1. id：单个标识符，例如 x
 * 2. idlist , id：标识符列表，递归定义，例如 x, y, z
 * 该规则用于声明中需要列出多个标识符的场景（例如 var x, y, z: integer;）
 */
idlist:
    IDENTIFIER
    {
        // 打印调试信息，显示正在创建的标识符列表
        fprintf(stderr, "Creating idlist with IDENTIFIER: %s\n", $1);  // 输出调试信息到 stderr，显示当前处理的标识符（$1 是 IDENTIFIER 的值，例如 "x"）

        // 复制标识符字符串
        char *id_copy = strdup($1);  // 使用 strdup 复制 $1（IDENTIFIER 的值，例如 "x"），以确保内存安全，防止后续修改原始字符串
        if (!id_copy) {  // 检查内存分配是否成功
            fprintf(stderr, "Error: Failed to allocate memory for id_copy\n");  // 如果 strdup 失败，打印错误信息
            exit(1);  // 退出程序，表示内存分配错误
        }

        // 构建语法树节点
        Node *node = create_node("idlist", NULL);  // 创建一个类型为 "idlist" 的语法树节点，value 设为 NULL（idlist 本身没有值，仅作为标识符列表的容器）
        add_child(node, create_node("id", id_copy));  // 添加子节点：类型为 "id"，value 设为复制的标识符（例如 "x"）

        $$ = node;  // 设置当前规则的返回值 $$ 为创建的节点，供上层规则（例如 var_declarations 或 program_head）使用
    }
    | idlist COMMA IDENTIFIER
    {
        // 打印调试信息，显示正在添加的标识符
        fprintf(stderr, "Adding IDENTIFIER to idlist: %s\n", $3);  // 输出调试信息到 stderr，显示当前添加的标识符（$3 是 IDENTIFIER 的值，例如 "y"）

        // 复制标识符字符串
        char *id_copy = strdup($3);  // 使用 strdup 复制 $3（IDENTIFIER 的值，例如 "y"），确保内存安全
        if (!id_copy) {  // 检查内存分配是否成功
            fprintf(stderr, "Error: Failed to allocate memory for id_copy\n");  // 如果 strdup 失败，打印错误信息
            exit(1);  // 退出程序，表示内存分配错误
        }

        // 构建新的 idlist 节点
        Node *node = create_node("idlist", NULL);  // 创建一个类型为 "idlist" 的语法树节点，value 设为 NULL（idlist 本身没有值，仅作为标识符列表的容器）

        // 将前一个 idlist 的子节点（id 节点）转移到新的 idlist 节点
        Node *prev_idlist = $1;  // $1 是前一个 idlist 的语法树节点，包含之前的标识符（例如 "x"）
        for (int i = 0; i < prev_idlist->child_count; i++) {  // 遍历前一个 idlist 的所有子节点
            add_child(node, prev_idlist->children[i]);  // 将前一个 idlist 的子节点（id 节点）添加到新节点中
        }

        // 避免内存释放问题
        prev_idlist->child_count = 0;  // 清空前一个 idlist 的子节点数量，因为子节点已被转移
        prev_idlist->children = NULL;  // 清空前一个 idlist 的子节点数组，避免后续释放时重复释放内存

        // 添加新的 id 节点
        add_child(node, create_node("id", id_copy));  // 添加子节点：类型为 "id"，value 设为复制的标识符（例如 "y"）

        $$ = node;  // 设置当前规则的返回值 $$ 为创建的节点，供上层规则或递归调用使用
    }
    ;

/* const_declaration -> id = const_value | const_declaration ; id = const_value
 * 定义 Pascal 语言中常量声明（const_declaration）的文法规则，支持以下两种形式：
 * 1. id = const_value：单个常量声明，例如 const x = 42
 * 2. const_declaration ; id = const_value：多个常量声明，递归定义，例如 const x = 42; y = 3.14
 * 该规则用于常量声明部分（例如 const x = 42; y = 3.14;），声明的常量会插入符号表并记录到语法树中
 */
const_declaration:
    IDENTIFIER EQUAL const_value
    {
        // 插入常量到符号表
        insert_symbol($1, "const", NULL, $3->value);  // 调用 insert_symbol 将常量插入符号表
        // 参数说明：
        // $1：常量名（IDENTIFIER 的值，例如 "x"）
        // "const"：符号种类，表示这是一个常量
        // NULL：类型，常量在此处未指定类型（后续可能由 const_value 推导），设为 NULL
        // $3->value：常量值，$3 是 const_value 的节点，其 value 字段存储常量值（例如 "42"）

        // 打印常量声明信息
        fprintf(output_file, "Inserted constant '%s' with value '%s'\n", $1, $3->value);  // 输出常量声明信息到 output_file，显示插入的常量名（$1）和值（$3->value），便于调试或记录

        // 构建语法树节点
        Node *node = create_node("const_declaration", NULL);  // 创建一个类型为 "const_declaration" 的语法树节点，value 设为 NULL（const_declaration 本身没有值，仅作为常量声明的容器）
        add_child(node, create_node("id", $1));  // 添加子节点：类型为 "id"，value 设为常量名（例如 "x"）
        add_child(node, $3);  // 添加子节点：$3 是 const_value，表示常量的值（例如 const_value (42)）

        $$ = node;  // 设置当前规则的返回值 $$ 为创建的节点，供上层规则（例如 const_declarations）使用
    }
    | const_declaration SEMICOLON IDENTIFIER EQUAL const_value
    {
        // 插入常量到符号表
        insert_symbol($3, "const", NULL, $5->value);  // 调用 insert_symbol 将常量插入符号表
        // 参数说明：
        // $3：常量名（IDENTIFIER 的值，例如 "y"）
        // "const"：符号种类，表示这是一个常量
        // NULL：类型，常量未指定类型，设为 NULL
        // $5->value：常量值，$5 是 const_value 的节点，其 value 字段存储常量值（例如 "3.14"）

        // 打印常量声明信息
        fprintf(output_file, "Inserted constant '%s' with value '%s'\n", $3, $5->value);  // 输出常量声明信息到 output_file，显示插入的常量名（$3）和值（$5->value）

        // 构建语法树节点
        Node *node = create_node("const_declaration", NULL);  // 创建一个类型为 "const_declaration" 的语法树节点，value 设为 NULL
        add_child(node, $1);  // 添加子节点：$1 是前一个 const_declaration，表示之前的常量声明（例如 const x = 42）
        add_child(node, create_node("id", $3));  // 添加子节点：类型为 "id"，value 设为新的常量名（例如 "y"）
        add_child(node, $5);  // 添加子节点：$5 是 const_value，表示新常量的值（例如 const_value (3.14)）
        // 注意：SEMICOLON 是分隔符，未添加到语法树中，因为它不携带语义信息

        $$ = node;  // 设置当前规则的返回值 $$ 为创建的节点，供上层规则或递归调用使用
    }
    ;

/* const_declarations -> ε | const const_declaration ;
 * 定义 Pascal 语言中常量声明部分的文法规则，支持以下两种形式：
 * 1. ε（空）：表示没有常量声明，例如直接进入变量声明部分（var ...）
 * 2. const const_declaration ;：常量声明部分，例如 const x = 42; y = 3.14;
 * 该规则用于程序体的常量声明部分（program_body -> const_declarations var_declarations ...）
 */
const_declarations:
    /* Empty */
    {
        // 处理空常量声明
        $$ = create_node("const_declarations", "empty");  // 创建一个类型为 "const_declarations" 的语法树节点，value 设为 "empty"，表示没有常量声明
        // $$ 是当前规则的返回值，将该节点传递给上层规则（例如 program_body）
    }
    | CONST const_declaration SEMICOLON
    {
        // 构建常量声明部分的语法树节点
        Node *node = create_node("const_declarations", NULL);  // 创建一个类型为 "const_declarations" 的语法树节点，value 设为 NULL（const_declarations 本身没有值，仅作为常量声明的容器）

        // 添加子节点
        add_child(node, $2);  // 添加子节点：$2 是 const_declaration，表示具体的常量声明（例如 const x = 42; y = 3.14;）
        // 注意：CONST 关键字和 SEMICOLON 分隔符未添加到语法树中，因为它们不携带语义信息

        $$ = node;  // 设置当前规则的返回值 $$ 为创建的节点，供上层规则（例如 program_body）使用
    }
    ;

/* value_parameter -> idlist : basic_type
 * 定义 Pascal 语言中值参数（value_parameter）的文法规则，支持以下形式：
 * idlist : basic_type：值参数声明，例如 x, y: integer，表示函数或过程的值参数
 * 该规则用于子程序（函数或过程）的参数声明中，值参数按值传递（传入参数值的副本）
 */
value_parameter:
    idlist COLON basic_type
    {
        // 构建值参数的语法树节点
        Node *node = create_node("value_parameter", NULL);  // 创建一个类型为 "value_parameter" 的语法树节点，value 设为 NULL（value_parameter 本身没有值，仅作为值参数声明的容器）

        // 添加子节点
        add_child(node, $1);  // 添加子节点：$1 是 idlist，表示参数名列表（例如 x, y）
        add_child(node, $3);  // 添加子节点：$3 是 basic_type，表示参数的类型（例如 integer）
        // 注意：COLON 是分隔符（:），未添加到语法树中，因为它不携带语义信息

        $$ = node;  // 设置当前规则的返回值 $$ 为创建的节点，供上层规则（例如 parameter 或 parameter_list）使用
    }
    ;

/* var_parameter -> var value_parameter
 * 定义 Pascal 语言中变量参数（var_parameter）的文法规则，支持以下形式：
 * var value_parameter：变量参数声明，例如 var x, y: integer，表示函数或过程的变量参数
 * 该规则用于子程序（函数或过程）的参数声明中，变量参数按引用传递（传入参数的地址，可以修改原变量的值）
 */
var_parameter:
    VAR value_parameter
    {
        // 构建变量参数的语法树节点
        Node *node = create_node("var_parameter", NULL);  // 创建一个类型为 "var_parameter" 的语法树节点，value 设为 NULL（var_parameter 本身没有值，仅作为变量参数声明的容器）

        // 添加子节点
        add_child(node, $2);  // 添加子节点：$2 是 value_parameter，表示具体的参数声明（例如 x, y: integer）
        // 注意：VAR 关键字未添加到语法树中，因为它不携带语义信息，仅用于区分值参数和变量参数

        $$ = node;  // 设置当前规则的返回值 $$ 为创建的节点，供上层规则（例如 parameter 或 parameter_list）使用
    }
    ;

/* parameter -> var_parameter | value_parameter
 * 定义 Pascal 语言中参数（parameter）的文法规则，支持以下两种形式：
 * 1. var_parameter：变量参数，例如 var x: integer，表示按引用传递的参数
 * 2. value_parameter：值参数，例如 x: integer，表示按值传递的参数
 * 该规则用于子程序（函数或过程）的参数声明中，表示单个参数（可以是值参数或变量参数）
 */
parameter:
    var_parameter
    {
        // 直接使用变量参数的语法树节点
        $$ = $1;  // 将 var_parameter 的节点直接作为 parameter 的返回值，$1 是 var_parameter 的语法树节点（例如 var_parameter 包含 value_parameter(x: integer)）
        // 这种形式表示参数是一个变量参数，无需额外构建新节点
    }
    | value_parameter
    {
        // 直接使用值参数的语法树节点
        $$ = $1;  // 将 value_parameter 的节点直接作为 parameter 的返回值，$1 是 value_parameter 的语法树节点（例如 value_parameter(x: integer)）
        // 这种形式表示参数是一个值参数，无需额外构建新节点
    }
    ;

/* parameter_list -> parameter | parameter_list ; parameter
 * 定义 Pascal 语言中参数列表（parameter_list）的文法规则，支持以下两种形式：
 * 1. parameter：单个参数，例如 x: integer
 * 2. parameter_list ; parameter：多个参数的递归定义，例如 x: integer; var y: real
 * 该规则用于子程序（函数或过程）的参数声明中，表示一个或多个参数的集合
 */
parameter_list:
    parameter
    {
        // 构建单个参数的参数列表语法树节点
        Node *node = create_node("parameter_list", NULL);  // 创建一个类型为 "parameter_list" 的语法树节点，value 设为 NULL（parameter_list 本身没有值，仅作为参数列表的容器）

        // 添加子节点
        add_child(node, $1);  // 添加子节点：$1 是 parameter，表示单个参数（例如 parameter 包含 value_parameter(x: integer)）

        $$ = node;  // 设置当前规则的返回值 $$ 为创建的节点，供上层规则（例如 subprogram_head）使用
    }
    | parameter_list SEMICOLON parameter
    {
        // 构建多个参数的参数列表语法树节点（递归定义）
        Node *node = create_node("parameter_list", NULL);  // 创建一个类型为 "parameter_list" 的语法树节点，value 设为 NULL（parameter_list 本身没有值，仅作为参数列表的容器）

        // 添加子节点
        add_child(node, $1);  // 添加子节点：$1 是前一个 parameter_list，表示之前的参数列表（例如 parameter_list 包含 parameter(x: integer)）
        add_child(node, $3);  // 添加子节点：$3 是 parameter，表示新的参数（例如 parameter 包含 var_parameter(y: real)）
        // 注意：SEMICOLON 是分隔符（;），未添加到语法树中，因为它不携带语义信息

        $$ = node;  // 设置当前规则的返回值 $$ 为创建的节点，供上层规则或递归调用使用
    }
    ;

/* formal_parameter -> ε | ( parameter_list )
 * 定义 Pascal 语言中形式参数（formal_parameter）的文法规则，支持以下两种形式：
 * 1. ε（空）：表示没有形式参数，例如 procedure proc;
 * 2. ( parameter_list )：带参数列表的形式参数，例如 (x: integer; var y: real)，表示子程序的参数声明
 * 该规则用于子程序（函数或过程）的头部，表示形式参数部分
 */
formal_parameter:
    /* Empty */
    {
        // 处理空形式参数
        $$ = create_node("formal_parameter", "empty");  // 创建一个类型为 "formal_parameter" 的语法树节点，value 设为 "empty"，表示没有形式参数
        // $$ 是当前规则的返回值，将该节点传递给上层规则（例如 subprogram_head）
    }
    | LPAREN parameter_list RPAREN
    {
        // 构建带参数列表的形式参数语法树节点
        Node *node = create_node("formal_parameter", NULL);  // 创建一个类型为 "formal_parameter" 的语法树节点，value 设为 NULL（formal_parameter 本身没有值，仅作为形式参数的容器）

        // 添加子节点
        add_child(node, $2);  // 添加子节点：$2 是 parameter_list，表示参数列表（例如 parameter_list 包含 parameter(x: integer)）
        // 注意：LPAREN 和 RPAREN 是括号分隔符（( 和 )），未添加到语法树中，因为它们不携带语义信息

        $$ = node;  // 设置当前规则的返回值 $$ 为创建的节点，供上层规则（例如 subprogram_head）使用
    }
    ;

/* var_declaration -> idlist : type | var_declaration ; idlist : type
 * 定义 Pascal 语言中变量声明（var_declaration）的文法规则，支持以下两种形式：
 * 1. idlist : type：单个变量声明，例如 x, y: integer，表示声明一组变量
 * 2. var_declaration ; idlist : type：多个变量声明的递归定义，例如 x, y: integer; z: real
 * 该规则用于变量声明部分（var_declarations），声明的变量会插入符号表并记录到语法树中
 */
var_declaration:
    idlist COLON type
    {
        // 打印调试信息，表示开始插入变量
        fprintf(stderr, "Inserting variables into symbol table\n");  // 输出调试信息到 stderr，表示正在处理变量插入

        // 检查符号表是否有效
        if (!current_table) {  // 检查全局符号表 current_table 是否为 NULL
            fprintf(stderr, "Error: current_table is NULL in var_declaration\n");  // 如果 current_table 为 NULL，打印错误信息
            exit(1);  // 退出程序，表示符号表未正确初始化
        }

        // 确定变量的类型
        char *var_type = $3->type;  // 初始将 var_type 设为 type 节点的 type 字段（例如 "basic_type" 或 "array_type"）
        if (strcmp($3->type, "array_type") == 0) {  // 如果 type 是数组类型（array_type）
            char array_type[128];  // 定义字符数组，用于存储格式化后的数组类型字符串
            snprintf(array_type, sizeof(array_type), "array[%s..%s] of %s",
                     $3->children[0]->children[0]->value,  // 数组范围的起始值（period -> start，例如 "1"）
                     $3->children[0]->children[1]->value,  // 数组范围的结束值（period -> end，例如 "10"）
                     $3->children[1]->value);  // 数组元素类型（basic_type，例如 "integer"）
            var_type = array_type;  // 更新 var_type 为格式化后的数组类型字符串（例如 "array[1..10] of integer"）
        } else {
            var_type = $3->value;  // 如果是基本类型（basic_type），直接使用 type 节点的 value 字段（例如 "integer"）
        }

        // 遍历 idlist，插入每个变量到符号表
        Node *idlist_node = $1;  // $1 是 idlist，表示变量名列表（例如 x, y）
        fprintf(stderr, "idlist has %d children\n", idlist_node->child_count);  // 打印调试信息，显示 idlist 中变量名的数量
        for (int i = 0; i < idlist_node->child_count; i++) {  // 遍历 idlist 的每个子节点（id 节点）
            Node *id_node = idlist_node->children[i];  // 获取当前变量名节点（id 节点）
            fprintf(stderr, "Child %d: type = %s, value = %s\n", i, id_node->type, id_node->value ? id_node->value : "NULL");  // 打印子节点信息，显示类型和值
            fprintf(stderr, "Inserting variable: %s, type: %s\n", id_node->value ? id_node->value : "NULL", var_type);  // 打印插入信息，显示变量名和类型

            // 插入变量到符号表
            insert_symbol(id_node->value, "var", var_type, NULL);  // 调用 insert_symbol 将变量插入符号表
            // 参数说明：
            // id_node->value：变量名（例如 "x"）
            // "var"：符号种类，表示这是一个变量
            // var_type：变量类型（例如 "integer" 或 "array[1..10] of integer"）
            // NULL：变量值，未初始化，设为 NULL
        }

        // 构建语法树节点
        Node *node = create_node("var_declaration", NULL);  // 创建一个类型为 "var_declaration" 的语法树节点，value 设为 NULL（var_declaration 本身没有值，仅作为变量声明的容器）
        add_child(node, $1);  // 添加子节点：$1 是 idlist，表示变量名列表
        add_child(node, $3);  // 添加子节点：$3 是 type，表示变量的类型（例如 basic_type(integer) 或 array_type）
        // 注意：COLON 是分隔符（:），未添加到语法树中，因为它不携带语义信息

        $$ = node;  // 设置当前规则的返回值 $$ 为创建的节点，供上层规则（例如 var_declarations）使用
    }
    | var_declaration SEMICOLON idlist COLON type
    {
        // 打印调试信息，表示继续插入变量（多个声明）
        fprintf(stderr, "Inserting variables into symbol table (continued)\n");  // 输出调试信息，表示处理后续变量声明

        // 检查符号表是否有效
        if (!current_table) {  // 检查全局符号表 current_table 是否为 NULL
            fprintf(stderr, "Error: current_table is NULL in var_declaration\n");  // 如果 current_table 为 NULL，打印错误信息
            exit(1);  // 退出程序，表示符号表未正确初始化
        }

        // 确定变量的类型
        char *var_type = $5->type;  // 初始将 var_type 设为 type 节点的 type 字段（例如 "basic_type" 或 "array_type"）
        if (strcmp($5->type, "array_type") == 0) {  // 如果 type 是数组类型（array_type）
            char array_type[128];  // 定义字符数组，用于存储格式化后的数组类型字符串
            snprintf(array_type, sizeof(array_type), "array[%s..%s] of %s",
                     $5->children[0]->children[0]->value,  // 数组范围的起始值（period -> start，例如 "20"）
                     $5->children[0]->children[1]->value,  // 数组范围的结束值（period -> end，例如 "30"）
                     $5->children[1]->value);  // 数组元素类型（basic_type，例如 "real"）
            var_type = array_type;  // 更新 var_type 为格式化后的数组类型字符串（例如 "array[20..30] of real"）
        } else {
            var_type = $5->value;  // 如果是基本类型（basic_type），直接使用 type 节点的 value 字段（例如 "real"）
        }

        // 遍历 idlist，插入每个变量到符号表
        Node *idlist_node = $3;  // $3 是 idlist，表示新的变量名列表（例如 z）
        fprintf(stderr, "idlist has %d children\n", idlist_node->child_count);  // 打印调试信息，显示 idlist 中变量名的数量
        for (int i = 0; i < idlist_node->child_count; i++) {  // 遍历 idlist 的每个子节点（id 节点）
            Node *id_node = idlist_node->children[i];  // 获取当前变量名节点（id 节点）
            fprintf(stderr, "Child %d: type = %s, value = %s\n", i, id_node->type, id_node->value ? id_node->value : "NULL");  // 打印子节点信息
            fprintf(stderr, "Inserting variable: %s, type: %s\n", id_node->value ? id_node->value : "NULL", var_type);  // 打印插入信息

            // 插入变量到符号表
            insert_symbol(id_node->value, "var", var_type, NULL);  // 调用 insert_symbol 将变量插入符号表
        }

        // 构建语法树节点
        Node *node = create_node("var_declaration", NULL);  // 创建一个类型为 "var_declaration" 的语法树节点，value 设为 NULL
        add_child(node, $1);  // 添加子节点：$1 是前一个 var_declaration，表示之前的变量声明（例如 x, y: integer）
        add_child(node, $3);  // 添加子节点：$3 是 idlist，表示新的变量名列表（例如 z）
        add_child(node, $5);  // 添加子节点：$5 是 type，表示新变量的类型（例如 basic_type(real)）
        // 注意：SEMICOLON 和 COLON 是分隔符，未添加到语法树中

        $$ = node;  // 设置当前规则的返回值 $$ 为创建的节点，供上层规则或递归调用使用
    }
    ;

/* var_declarations -> ε | var var_declaration ;
 * 定义 Pascal 语言中变量声明部分的文法规则，支持以下两种形式：
 * 1. ε（空）：表示没有变量声明，例如直接进入子程序声明或主程序体（begin ... end）
 * 2. var var_declaration ;：变量声明部分，例如 var x, y: integer; z: real;
 * 该规则用于程序体的变量声明部分（program_body -> const_declarations var_declarations ...）
 */
var_declarations:
    /* Empty */
    {
        // 处理空变量声明
        $$ = create_node("var_declarations", "empty");  // 创建一个类型为 "var_declarations" 的语法树节点，value 设为 "empty"，表示没有变量声明
        // $$ 是当前规则的返回值，将该节点传递给上层规则（例如 program_body）
    }
    | VAR var_declaration SEMICOLON
    {
        // 构建变量声明部分的语法树节点
        Node *node = create_node("var_declarations", NULL);  // 创建一个类型为 "var_declarations" 的语法树节点，value 设为 NULL（var_declarations 本身没有值，仅作为变量声明的容器）

        // 添加子节点
        add_child(node, $2);  // 添加子节点：$2 是 var_declaration，表示具体的变量声明（例如 var x, y: integer;）
        // 注意：VAR 关键字和 SEMICOLON 分隔符未添加到语法树中，因为它们不携带语义信息

        $$ = node;  // 设置当前规则的返回值 $$ 为创建的节点，供上层规则（例如 program_body）使用
    }
    ;

/* subprogram_declarations -> ε | subprogram_declarations subprogram ;
 * 定义 Pascal 语言中子程序声明部分的文法规则，支持以下两种形式：
 * 1. ε（空）：表示没有子程序声明，例如直接进入主程序体（begin ... end）
 * 2. subprogram_declarations subprogram ;：子程序声明部分，递归定义，例如 procedure proc1; procedure proc2;
 * 该规则用于程序体的子程序声明部分（program_body -> const_declarations var_declarations subprogram_declarations ...）
 */
subprogram_declarations:
    /* Empty */
    {
        // 处理空子程序声明
        $$ = create_node("subprogram_declarations", "empty");  // 创建一个类型为 "subprogram_declarations" 的语法树节点，value 设为 "empty"，表示没有子程序声明
        // $$ 是当前规则的返回值，将该节点传递给上层规则（例如 program_body）
    }
    | subprogram_declarations subprogram SEMICOLON
    {
        // 构建子程序声明部分的语法树节点（递归定义）
        Node *node = create_node("subprogram_declarations", NULL);  // 创建一个类型为 "subprogram_declarations" 的语法树节点，value 设为 NULL（subprogram_declarations 本身没有值，仅作为子程序声明的容器）

        // 添加子节点
        add_child(node, $1);  // 添加子节点：$1 是前一个 subprogram_declarations，表示之前的子程序声明（例如 procedure proc1;）
        add_child(node, $2);  // 添加子节点：$2 是 subprogram，表示新的子程序声明（例如 procedure proc2;）
        // 注意：SEMICOLON 是分隔符（;），未添加到语法树中，因为它不携带语义信息

        $$ = node;  // 设置当前规则的返回值 $$ 为创建的节点，供上层规则（例如 program_body）或递归调用使用
    }
    ;

/* subprogram_head -> procedure id formal_parameter | function id formal_parameter : basic_type
 * 定义 Pascal 语言中子程序头部（subprogram_head）的文法规则，支持以下两种形式：
 * 1. procedure id formal_parameter：过程声明，例如 procedure proc(x: integer);
 * 2. function id formal_parameter : basic_type：函数声明，例如 function func(x: integer): integer;
 * 该规则用于子程序声明（subprogram）的头部部分，定义过程或函数的名称、参数和返回值类型（仅函数）
 */
subprogram_head:
    PROCEDURE IDENTIFIER formal_parameter
    {
        // 插入过程名到符号表
        insert_symbol($2, "procedure", NULL, NULL);  // 调用 insert_symbol 将过程名插入符号表
        // 参数说明：
        // $2：过程名（IDENTIFIER 的值，例如 "proc"）
        // "procedure"：符号种类，表示这是一个过程
        // NULL：类型，过程没有返回值类型，设为 NULL
        // NULL：值，过程没有初始值，设为 NULL

        // 创建新的作用域
        SymbolTable *new_table = create_symbol_table(current_table);  // 调用 create_symbol_table 创建一个新的符号表，作为子程序的作用域，current_table 是父作用域
        current_table = new_table;  // 更新全局变量 current_table 为新的子程序作用域，后续符号插入将在此作用域中进行

        // 插入参数到符号表
        Node *param_list = $3;  // $3 是 formal_parameter，表示形式参数（例如 formal_parameter 包含 parameter_list(x: integer)）
        if (strcmp(param_list->type, "formal_parameter") == 0 && param_list->child_count > 0) {  // 检查是否有参数（formal_parameter 不为空）
            Node *param_list_node = param_list->children[0];  // 获取 parameter_list 节点（formal_parameter 的第一个子节点）
            for (int i = 0; i < param_list_node->child_count; i++) {  // 遍历 parameter_list 中的每个参数
                Node *param = param_list_node->children[i];  // 获取当前 parameter 节点
                char *param_type = strcmp(param->type, "var_parameter") == 0 ? "var " : "";  // 判断参数类型：如果是 var_parameter，则前缀为 "var "，否则为空
                Node *value_param = strcmp(param->type, "var_parameter") == 0 ? param->children[0] : param;  // 获取 value_parameter 节点（var_parameter 包含 value_parameter）
                char *basic_type = value_param->children[1]->value;  // 获取参数的基本类型（basic_type 的 value，例如 "integer"）
                char full_type[128];  // 定义字符数组，用于存储完整的参数类型
                snprintf(full_type, sizeof(full_type), "%s%s", param_type, basic_type);  // 格式化参数类型，例如 "integer" 或 "var integer"
                Node *idlist_node = value_param->children[0];  // 获取 idlist 节点（参数名列表）
                for (int j = 0; j < idlist_node->child_count; j++) {  // 遍历 idlist 中的每个参数名
                    Node *id_node = idlist_node->children[j];  // 获取当前参数名节点（id 节点）
                    insert_symbol(id_node->value, "var", full_type, NULL);  // 插入参数到符号表
                    // 参数说明：
                    // id_node->value：参数名（例如 "x"）
                    // "var"：符号种类，表示这是一个变量（参数按变量处理）
                    // full_type：参数类型（例如 "integer" 或 "var integer"）
                    // NULL：参数值，未初始化，设为 NULL
                }
            }
        }

        // 构建语法树节点
        Node *node = create_node("subprogram_head", "procedure");  // 创建一个类型为 "subprogram_head" 的语法树节点，value 设为 "procedure"，表示这是一个过程
        add_child(node, create_node("id", $2));  // 添加子节点：类型为 "id"，value 设为过程名（例如 "proc"）
        add_child(node, $3);  // 添加子节点：$3 是 formal_parameter，表示形式参数
        // 注意：PROCEDURE 关键字未添加到语法树中，因为它不携带语义信息

        $$ = node;  // 设置当前规则的返回值 $$ 为创建的节点，供上层规则（例如 subprogram）使用
    }
    | FUNCTION IDENTIFIER formal_parameter COLON basic_type
    {
        // 插入函数名到符号表
        insert_symbol($2, "function", $5->value, NULL);  // 调用 insert_symbol 将函数名插入符号表
        // 参数说明：
        // $2：函数名（IDENTIFIER 的值，例如 "func"）
        // "function"：符号种类，表示这是一个函数
        // $5->value：返回值类型（basic_type 的 value，例如 "integer"）
        // NULL：值，函数没有初始值，设为 NULL

        // 创建新的作用域
        SymbolTable *new_table = create_symbol_table(current_table);  // 创建新的子程序作用域，current_table 是父作用域
        current_table = new_table;  // 更新全局变量 current_table 为新的子程序作用域

        // 插入参数到符号表
        Node *param_list = $3;  // $3 是 formal_parameter，表示形式参数
        if (strcmp(param_list->type, "formal_parameter") == 0 && param_list->child_count > 0) {  // 检查是否有参数
            Node *param_list_node = param_list->children[0];  // 获取 parameter_list 节点
            for (int i = 0; i < param_list_node->child_count; i++) {  // 遍历 parameter_list 中的每个参数
                Node *param = param_list_node->children[i];  // 获取当前 parameter 节点
                char *param_type = strcmp(param->type, "var_parameter") == 0 ? "var " : "";  // 判断参数类型
                Node *value_param = strcmp(param->type, "var_parameter") == 0 ? param->children[0] : param;  // 获取 value_parameter 节点
                char *basic_type = value_param->children[1]->value;  // 获取参数的基本类型（例如 "integer"）
                char full_type[128];  // 定义字符数组，用于存储完整的参数类型
                snprintf(full_type, sizeof(full_type), "%s%s", param_type, basic_type);  // 格式化参数类型
                Node *idlist_node = value_param->children[0];  // 获取 idlist 节点
                for (int j = 0; j < idlist_node->child_count; j++) {  // 遍历 idlist 中的每个参数名
                    Node *id_node = idlist_node->children[j];  // 获取当前参数名节点
                    insert_symbol(id_node->value, "var", full_type, NULL);  // 插入参数到符号表
                }
            }
        }

        // 插入函数返回值变量（函数名作为返回值变量）
        insert_symbol($2, "var", $5->value, NULL);  // 将函数名作为变量插入符号表，用于存储返回值
        // 参数说明：
        // $2：函数名（例如 "func"）
        // "var"：符号种类，表示这是一个变量（用于返回值）
        // $5->value：变量类型（即函数返回值类型，例如 "integer"）
        // NULL：初始值，设为 NULL

        // 构建语法树节点
        Node *node = create_node("subprogram_head", "function");  // 创建一个类型为 "subprogram_head" 的语法树节点，value 设为 "function"，表示这是一个函数
        add_child(node, create_node("id", $2));  // 添加子节点：类型为 "id"，value 设为函数名（例如 "func"）
        add_child(node, $3);  // 添加子节点：$3 是 formal_parameter，表示形式参数
        add_child(node, $5);  // 添加子节点：$5 是 basic_type，表示返回值类型（例如 basic_type(integer)）
        // 注意：FUNCTION 关键字和 COLON 分隔符未添加到语法树中

        $$ = node;  // 设置当前规则的返回值 $$ 为创建的节点，供上层规则（例如 subprogram）使用
    }
    ;

/* subprogram_body -> const_declarations var_declarations compound_statement
 * 定义 Pascal 语言中子程序体（subprogram_body）的文法规则，由以下部分组成：
 * 1. const_declarations：常量声明部分（可以为空），例如 const x = 42;
 * 2. var_declarations：变量声明部分（可以为空），例如 var y: integer;
 * 3. compound_statement：复合语句（主程序体，例如 begin ... end）
 * 该规则用于子程序（函数或过程）的主体部分，定义子程序的局部声明和执行语句
 */
subprogram_body:
    const_declarations var_declarations compound_statement
    {
        // 构建子程序体的语法树节点
        Node *node = create_node("subprogram_body", NULL);  // 创建一个类型为 "subprogram_body" 的语法树节点，value 设为 NULL（subprogram_body 本身没有值，仅作为子程序体的容器）

        // 添加子节点
        add_child(node, $1);  // 添加子节点：$1 是 const_declarations，表示常量声明部分（可以是 empty）
        add_child(node, $2);  // 添加子节点：$2 是 var_declarations，表示变量声明部分（可以是 empty）
        add_child(node, $3);  // 添加子节点：$3 是 compound_statement，表示子程序的执行语句（例如 begin ... end）

        $$ = node;  // 设置当前规则的返回值 $$ 为创建的节点，供上层规则（例如 subprogram）使用
    }
    ;

/* subprogram -> subprogram_head ; subprogram_body
 * 定义 Pascal 语言中子程序（subprogram）的文法规则，由以下部分组成：
 * 1. subprogram_head：子程序头部，例如 procedure proc(x: integer); 或 function func(x: integer): integer;
 * 2. SEMICOLON：分号分隔符（;）
 * 3. subprogram_body：子程序体，包含局部声明和执行语句
 * 该规则用于子程序声明部分（subprogram_declarations），定义一个完整的子程序（过程或函数）
 */
subprogram:
    subprogram_head SEMICOLON subprogram_body
    {
        // 构建子程序的语法树节点
        Node *node = create_node("subprogram", NULL);  // 创建一个类型为 "subprogram" 的语法树节点，value 设为 NULL（subprogram 本身没有值，仅作为子程序的容器）

        // 添加子节点
        add_child(node, $1);  // 添加子节点：$1 是 subprogram_head，表示子程序头部（包含名称、参数等）
        add_child(node, $3);  // 添加子节点：$3 是 subprogram_body，表示子程序体（包含声明和语句）
        // 注意：SEMICOLON 是分隔符，未添加到语法树中，因为它不携带语义信息

        // 切换回父作用域
        current_table = current_table->parent;  // 将全局变量 current_table 切换回父作用域，表示子程序作用域的结束
        // 在 subprogram_head 中创建了子程序作用域（new_table），此处恢复到父作用域（例如全局作用域），以便后续声明或子程序使用正确的符号表

        $$ = node;  // 设置当前规则的返回值 $$ 为创建的节点，供上层规则（例如 subprogram_declarations）使用
    }
    ;

/* compound_statement -> begin statement_list end
 * 定义 Pascal 语言中复合语句（compound_statement）的文法规则，支持以下两种形式：
 * 1. begin statement_list end：复合语句，例如 begin x := 1 end
 * 2. begin statement_list ; end：复合语句，末尾带分号，例如 begin x := 1; end
 * 该规则用于程序体或子程序体的执行部分，表示一组语句的集合
 */
compound_statement:
    TOKEN_BEGIN statement_list END
    {
        // 构建复合语句的语法树节点
        Node *node = create_node("compound_statement", NULL);  // 创建一个类型为 "compound_statement" 的语法树节点，value 设为 NULL（compound_statement 本身没有值，仅作为复合语句的容器）

        // 添加子节点
        add_child(node, $2);  // 添加子节点：$2 是 statement_list，表示语句列表（例如 x := 1）
        // 注意：TOKEN_BEGIN 和 END 是关键字（begin 和 end），未添加到语法树中，因为它们不携带语义信息

        $$ = node;  // 设置当前规则的返回值 $$ 为创建的节点，供上层规则（例如 subprogram_body 或 program_body）使用
    }
    | TOKEN_BEGIN statement_list SEMICOLON END
    {
        // 构建复合语句的语法树节点（带分号形式）
        Node *node = create_node("compound_statement", NULL);  // 创建一个类型为 "compound_statement" 的语法树节点，value 设为 NULL

        // 添加子节点
        add_child(node, $2);  // 添加子节点：$2 是 statement_list，表示语句列表
        // 注意：SEMICOLON 是分隔符（;），未添加到语法树中，符合 Pascal 语法中可选分号的设计

        $$ = node;  // 设置当前规则的返回值 $$ 为创建的节点，供上层规则使用
    }
    ;

/* statement_list -> statement | statement_list ; statement
 * 定义 Pascal 语言中语句列表（statement_list）的文法规则，支持以下两种形式：
 * 1. statement：单个语句，例如 x := 1
 * 2. statement_list ; statement：多个语句的递归定义，例如 x := 1; y := 2
 * 该规则用于复合语句中，表示一组语句的集合
 */
statement_list:
    statement
    {
        // 构建单个语句的语句列表
        $$ = create_node("statement_list", NULL);  // 创建一个类型为 "statement_list" 的语法树节点，value 设为 NULL（statement_list 本身没有值，仅作为语句列表的容器）
        add_child($$, $1);  // 添加子节点：$1 是 statement，表示单个语句（例如 x := 1）
    }
    | statement_list SEMICOLON statement
    {
        // 扩展现有的语句列表（递归定义）
        $$ = $1;  // 复用 $1（现有的 statement_list 节点），避免创建新节点以提高效率
        add_child($$, $3);  // 添加子节点：$3 是 statement，表示新的语句（例如 y := 2）
        // 注意：SEMICOLON 是分隔符（;），未添加到语法树中，因为它不携带语义信息
    }
    ;

/* variable -> id id_varpart
 * 定义 Pascal 语言中变量（variable）的文法规则，支持以下形式：
 * id id_varpart：变量引用，例如 x 或 x[1]（数组访问）
 * 该规则用于表达式或赋值语句中，表示一个变量（可以是简单变量或数组元素）
 */
variable:
    IDENTIFIER id_varpart
    {
        // 构建变量的语法树节点
        Node *node = create_node("variable", NULL);  // 创建一个类型为 "variable" 的语法树节点，value 设为 NULL（variable 本身没有值，仅作为变量引用的容器）

        // 添加子节点
        add_child(node, create_node("id", $1));  // 添加子节点：类型为 "id"，value 设为变量名（$1 是 IDENTIFIER，例如 "x"）
        add_child(node, $2);  // 添加子节点：$2 是 id_varpart，表示变量的附加部分（例如数组索引，可以是 empty）

        $$ = node;  // 设置当前规则的返回值 $$ 为创建的节点，供上层规则（例如赋值语句或表达式）使用
    }
    ;

/* id_varpart -> ε | [ expression_list ]
 * 定义 Pascal 语言中变量附加部分（id_varpart）的文法规则，支持以下两种形式：
 * 1. ε（空）：表示简单变量，例如 x
 * 2. [ expression_list ]：表示数组访问，例如 x[1] 或 x[i, j]
 * 该规则用于变量引用中，指定变量是否为数组元素及其索引
 */
id_varpart:
    /* Empty */
    {
        // 处理简单变量（无数组索引）
        $$ = create_node("id_varpart", "empty");  // 创建一个类型为 "id_varpart" 的语法树节点，value 设为 "empty"，表示没有数组访问
    }
    | LBRACKET expression_list RBRACKET
    {
        // 构建数组访问的附加部分
        Node *node = create_node("id_varpart", "array_access");  // 创建一个类型为 "id_varpart" 的语法树节点，value 设为 "array_access"，表示这是一个数组访问
        add_child(node, $2);  // 添加子节点：$2 是 expression_list，表示数组索引表达式列表（例如 1 或 i, j）
        // 注意：LBRACKET 和 RBRACKET 是括号（[ 和 ]），未添加到语法树中，因为它们不携带语义信息

        $$ = node;  // 设置当前规则的返回值 $$ 为创建的节点，供上层规则（例如 variable）使用
    }
    ;

/* factor -> num | variable | ( expression ) | id ( expression_list ) | not factor | uminus factor
 * 定义 Pascal 语言中因子（factor）的文法规则，支持以下形式：
 * 1. num：数值常量，例如 42（INTEGER_CONST）、3.14（REAL_CONST）、'a'（CHAR_CONST）、true/false（BOOLEAN_CONST）
 * 2. variable：变量引用，例如 x 或 x[1]
 * 3. ( expression )：括号表达式，例如 (x + 1)
 * 4. id ( expression_list )：函数调用，例如 func(1, 2)
 * 5. not factor：逻辑非，例如 not true
 * 6. uminus factor：一元负号，例如 -5（通过 %prec UMINUS 解决优先级冲突）
 * 该规则用于表达式（expression）的最小单元，表示一个因子（可以是常量、变量、函数调用等）
 */
factor:
    INTEGER_CONST
    {
        // 处理整数常量
        char value[32];  // 定义字符数组，用于存储格式化后的整数值
        snprintf(value, sizeof(value), "%d", $1);  // 使用 snprintf 格式化 INTEGER_CONST 的值（$1，例如 42），存储为字符串 "42"
        $$ = create_node("factor", value);  // 创建一个类型为 "factor" 的语法树节点，value 设为格式化后的字符串（例如 "42"）
        $$->value = strdup(value);  // 复制字符串到节点的 value 字段，确保内存安全（防止 value 数组被覆盖）
    }
    | REAL_CONST
    {
        // 处理实数常量
        char value[32];  // 定义字符数组，用于存储格式化后的实数值
        snprintf(value, sizeof(value), "%.6f", $1);  // 使用 snprintf 格式化 REAL_CONST 的值（$1，例如 3.14），保留 6 位小数，存储为字符串 "3.140000"
        $$ = create_node("factor", value);  // 创建 factor 节点，value 设为格式化后的字符串
        $$->value = strdup(value);  // 复制字符串到节点的 value 字段
    }
    | CHAR_CONST
    {
        // 处理字符常量
        char value[2] = { $1, '\0' };  // 创建字符数组，存储 CHAR_CONST 的值（$1，例如 'a'），并以空字符结尾，格式为 "a"
        $$ = create_node("factor", value);  // 创建 factor 节点，value 设为字符字符串
        $$->value = strdup(value);  // 复制字符串到节点的 value 字段
    }
    | BOOLEAN_CONST
    {
        // 处理布尔常量
        $$ = create_node("factor", $1 ? "true" : "false");  // 创建 factor 节点，value 设为 "true" 或 "false"（根据 $1 的值，$1 为 1 表示 true，为 0 表示 false）
        $$->value = strdup($1 ? "true" : "false");  // 复制字符串到节点的 value 字段
    }
    | variable
    {
        // 处理变量引用
        Node *node = create_node("factor", "variable");  // 创建 factor 节点，value 设为 "variable"，表示这是一个变量因子
        add_child(node, $1);  // 添加子节点：$1 是 variable，表示变量引用（例如 variable(x) 或 variable(x[1])）
        $$ = node;  // 设置当前规则的返回值 $$ 为创建的节点
    }
    | function_call
    {
        // 处理函数调用
        Node *node = create_node("factor", "function_call");  // 创建 factor 节点，value 设为 "function_call"，表示这是一个函数调用因子
        add_child(node, $1);  // 添加子节点：$1 是 function_call，表示函数调用（例如 func(1, 2)）
        $$ = node;  // 设置当前规则的返回值 $$ 为创建的节点
    }
    | LPAREN expression RPAREN
    {
        // 处理括号表达式
        Node *node = create_node("factor", "expression");  // 创建 factor 节点，value 设为 "expression"，表示这是一个括号表达式因子
        add_child(node, $2);  // 添加子节点：$2 是 expression，表示括号内的表达式（例如 x + 1）
        // 注意：LPAREN 和 RPAREN 是括号（( 和 )），未添加到语法树中，因为它们不携带语义信息
        $$ = node;  // 设置当前规则的返回值 $$ 为创建的节点
    }
    | NOT factor
    {
        // 处理逻辑非操作
        Node *node = create_node("factor", "not");  // 创建 factor 节点，value 设为 "not"，表示这是一个逻辑非操作
        add_child(node, $2);  // 添加子节点：$2 是 factor，表示被取非的因子（例如 true）
        $$ = node;  // 设置当前规则的返回值 $$ 为创建的节点
    }
    | MINUS factor %prec UMINUS
    {
        // 处理一元负号操作
        Node *node = create_node("factor", "uminus");  // 创建 factor 节点，value 设为 "uminus"，表示这是一个一元负号操作
        add_child(node, $2);  // 添加子节点：$2 是 factor，表示被取负的因子（例如 5）
        // 注意：%prec UMINUS 用于解决优先级冲突，确保 MINUS 被解析为一元负号（而不是减法运算符）
        $$ = node;  // 设置当前规则的返回值 $$ 为创建的节点
    }
    ;

/* term -> factor | term MULTIPLY factor | term DIVIDE factor | term DIV factor | term MOD factor | term AND factor
 * 定义 Pascal 语言中项（term）的文法规则，支持以下形式：
 * 1. factor：单个因子，例如 42 或 x
 * 2. term MULTIPLY factor：乘法，例如 x * 2
 * 3. term DIVIDE factor：除法（实数除法），例如 x / 2.0
 * 4. term DIV factor：整除，例如 x div 2
 * 5. term MOD factor：取模，例如 x mod 2
 * 6. term AND factor：逻辑与，例如 true and false
 * 该规则用于表达式（expression）中，表示一个项（由因子通过乘法、除法、取模或逻辑与操作组成）
 */
term:
    factor
    {
        // 构建单个因子的项
        $$ = create_node("term", NULL);  // 创建一个类型为 "term" 的语法树节点，value 初始设为 NULL（term 本身没有值，仅作为项的容器）
        add_child($$, $1);  // 添加子节点：$1 是 factor，表示单个因子（例如 42 或 x）

        // 设置 term 的值（用于后续计算或调试）
        if (strcmp($1->type, "factor") == 0) {  // 确保 $1 是 factor 节点
            if (strcmp($1->value, "variable") == 0) {  // 如果 factor 是一个变量（factor 的 value 为 "variable"）
                // 从符号表获取变量的值
                Node *var_node = $1->children[0];  // 获取 factor 的子节点 variable
                Node *id_node = var_node->children[0];  // 获取 variable 的子节点 id（变量名，例如 "x"）
                Symbol *sym = lookup_symbol(id_node->value);  // 在符号表中查找变量名
                if (sym && sym->value) {  // 如果变量存在且有值
                    $$->value = strdup(sym->value);  // 复制变量的值到 term 节点的 value 字段（例如 "42"）
                } else {  // 如果变量未定义或无值
                    $$->value = strdup("unknown");  // 设置 term 的值为 "unknown"
                }
            } else {  // 如果 factor 是常数（例如 42、3.14、true）
                // 直接使用 factor 的值
                $$->value = strdup($1->value);  // 复制 factor 的值到 term 节点的 value 字段（例如 "42"）
            }
        } else {  // 如果 $1 不是 factor 节点（理论上不会发生，但作为防御性编程）
            $$->value = strdup("unknown");  // 设置 term 的值为 "unknown"
        }

        // 检查内存分配是否成功
        if (!$$->value) {  // 如果 strdup 失败（内存分配失败）
            fprintf(stderr, "Error: Failed to allocate memory for term value\n");  // 打印错误信息
            exit(1);  // 退出程序，表示内存分配错误
        }
    }
    | term MULTIPLY factor
    {
        // 构建乘法项
        Node *node = create_node("term", "multiply");  // 创建 term 节点，value 设为 "multiply"，表示这是一个乘法操作
        add_child(node, $1);  // 添加子节点：$1 是 term，表示左侧项（例如 x）
        add_child(node, $3);  // 添加子节点：$3 是 factor，表示右侧因子（例如 2）
        $$ = node;  // 设置当前规则的返回值 $$ 为创建的节点
        // 注释：计算逻辑已移除，value 仅用于标识操作类型，后续可以在语义分析或代码生成阶段处理
    }
    | term DIVIDE factor
    {
        // 构建除法项（实数除法）
        Node *node = create_node("term", "divide");  // 创建 term 节点，value 设为 "divide"，表示这是一个实数除法操作
        add_child(node, $1);  // 添加子节点：$1 是 term，表示左侧项
        add_child(node, $3);  // 添加子节点：$3 是 factor，表示右侧因子
        $$ = node;  // 设置当前规则的返回值 $$ 为创建的节点
    }
    | term DIV factor
    {
        // 构建整除项
        Node *node = create_node("term", "div");  // 创建 term 节点，value 设为 "div"，表示这是一个整除操作
        add_child(node, $1);  // 添加子节点：$1 是 term，表示左侧项
        add_child(node, $3);  // 添加子节点：$3 是 factor，表示右侧因子
        $$ = node;  // 设置当前规则的返回值 $$ 为创建的节点
    }
    | term MOD factor
    {
        // 构建取模项
        Node *node = create_node("term", "mod");  // 创建 term 节点，value 设为 "mod"，表示这是一个取模操作
        add_child(node, $1);  // 添加子节点：$1 是 term，表示左侧项
        add_child(node, $3);  // 添加子节点：$3 是 factor，表示右侧因子
        $$ = node;  // 设置当前规则的返回值 $$ 为创建的节点
    }
    | term AND factor
    {
        // 构建逻辑与项
        Node *node = create_node("term", "and");  // 创建 term 节点，value 设为 "and"，表示这是一个逻辑与操作
        add_child(node, $1);  // 添加子节点：$1 是 term，表示左侧项
        add_child(node, $3);  // 添加子节点：$3 是 factor，表示右侧因子
        $$ = node;  // 设置当前规则的返回值 $$ 为创建的节点
    }
    ;

/* simple_expression -> term | simple_expression PLUS term | simple_expression MINUS term | simple_expression OR term
 * 定义 Pascal 语言中简单表达式（simple_expression）的文法规则，支持以下形式：
 * 1. term：单个项，例如 42 或 x * 2
 * 2. simple_expression PLUS term：加法，例如 x + 2
 * 3. simple_expression MINUS term：减法，例如 x - 2
 * 4. simple_expression OR term：逻辑或，例如 true or false
 * 该规则用于表达式（expression）中，表示一个简单表达式（由项通过加法、减法或逻辑或操作组成）
 */
simple_expression:
    term
    {
        // 构建单个项的简单表达式
        $$ = create_node("simple_expression", NULL);  // 创建一个类型为 "simple_expression" 的语法树节点，value 初始设为 NULL（simple_expression 本身没有值，仅作为简单表达式的容器）
        add_child($$, $1);  // 添加子节点：$1 是 term，表示单个项（例如 42 或 x * 2）

        // 从 term 继承 value
        $$->value = $1->value ? strdup($1->value) : NULL;  // 如果 term 有值（例如 "42" 或 "unknown"），则复制到 simple_expression 的 value 字段；否则设为 NULL
        // 注意：strdup 用于内存安全，确保 value 字段的字符串是独立的副本
    }
    | simple_expression PLUS term
    {
        // 构建加法简单表达式
        Node *node = create_node("simple_expression", "plus");  // 创建 simple_expression 节点，value 设为 "plus"，表示这是一个加法操作
        add_child(node, $1);  // 添加子节点：$1 是 simple_expression，表示左侧简单表达式（例如 x）
        add_child(node, $3);  // 添加子节点：$3 是 term，表示右侧项（例如 2）
        $$ = node;  // 设置当前规则的返回值 $$ 为创建的节点
        // 注释：计算逻辑已移除，value 仅用于标识操作类型，后续可以在语义分析或代码生成阶段处理
    }
    | simple_expression MINUS term
    {
        // 构建减法简单表达式
        Node *node = create_node("simple_expression", "minus");  // 创建 simple_expression 节点，value 设为 "minus"，表示这是一个减法操作
        add_child(node, $1);  // 添加子节点：$1 是 simple_expression，表示左侧简单表达式
        add_child(node, $3);  // 添加子节点：$3 是 term，表示右侧项
        $$ = node;  // 设置当前规则的返回值 $$ 为创建的节点
    }
    | simple_expression OR term
    {
        // 构建逻辑或简单表达式
        Node *node = create_node("simple_expression", "or");  // 创建 simple_expression 节点，value 设为 "or"，表示这是一个逻辑或操作
        add_child(node, $1);  // 添加子节点：$1 是 simple_expression，表示左侧简单表达式
        add_child(node, $3);  // 添加子节点：$3 是 term，表示右侧项
        $$ = node;  // 设置当前规则的返回值 $$ 为创建的节点
    }
    ;

/* expression -> simple_expression | simple_expression relop simple_expression
 * 定义 Pascal 语言中表达式（expression）的文法规则，支持以下形式：
 * 1. simple_expression：单个简单表达式，例如 x + 2
 * 2. simple_expression relop simple_expression：关系运算表达式，例如 x + 2 = y
 *    - relop 包括：EQUAL (=), NOT_EQUAL (<>), LESS (<), LESS_EQUAL (<=), GREATER (>), GREATER_EQUAL (>=)
 * 该规则用于赋值语句、条件语句等，表示一个完整的表达式（可以是简单表达式或关系运算表达式）
 */
expression:
    simple_expression
    {
        // 构建单个简单表达式的表达式
        $$ = create_node("expression", NULL);  // 创建一个类型为 "expression" 的语法树节点，value 初始设为 NULL（expression 本身没有值，仅作为表达式的容器）
        add_child($$, $1);  // 添加子节点：$1 是 simple_expression，表示单个简单表达式（例如 x + 2）
    }
    | simple_expression EQUAL simple_expression
    {
        // 构建相等关系表达式
        Node *node = create_node("expression", "equal");  // 创建 expression 节点，value 设为 "equal"，表示这是一个相等关系运算
        add_child(node, $1);  // 添加子节点：$1 是 simple_expression，表示左侧简单表达式（例如 x + 2）
        add_child(node, $3);  // 添加子节点：$3 是 simple_expression，表示右侧简单表达式（例如 y）
        // 注意：EQUAL 是关系运算符（=），未添加到语法树中，因为它不携带语义信息（已通过 value="equal" 表示）
        $$ = node;  // 设置当前规则的返回值 $$ 为创建的节点
    }
    | simple_expression NOT_EQUAL simple_expression
    {
        // 构建不相等关系表达式
        Node *node = create_node("expression", "not_equal");  // 创建 expression 节点，value 设为 "not_equal"，表示这是一个不相等关系运算
        add_child(node, $1);  // 添加子节点：$1 是 simple_expression，表示左侧简单表达式
        add_child(node, $3);  // 添加子节点：$3 是 simple_expression，表示右侧简单表达式
        $$ = node;  // 设置当前规则的返回值 $$ 为创建的节点
    }
    | simple_expression LESS simple_expression
    {
        // 构建小于关系表达式
        Node *node = create_node("expression", "less");  // 创建 expression 节点，value 设为 "less"，表示这是一个小于关系运算
        add_child(node, $1);  // 添加子节点：$1 是 simple_expression，表示左侧简单表达式
        add_child(node, $3);  // 添加子节点：$3 是 simple_expression，表示右侧简单表达式
        $$ = node;  // 设置当前规则的返回值 $$ 为创建的节点
    }
    | simple_expression LESS_EQUAL simple_expression
    {
        // 构建小于等于关系表达式
        Node *node = create_node("expression", "less_equal");  // 创建 expression 节点，value 设为 "less_equal"，表示这是一个小于等于关系运算
        add_child(node, $1);  // 添加子节点：$1 是 simple_expression，表示左侧简单表达式
        add_child(node, $3);  // 添加子节点：$3 是 simple_expression，表示右侧简单表达式
        $$ = node;  // 设置当前规则的返回值 $$ 为创建的节点
    }
    | simple_expression GREATER simple_expression
    {
        // 构建大于关系表达式
        Node *node = create_node("expression", "greater");  // 创建 expression 节点，value 设为 "greater"，表示这是一个大于关系运算
        add_child(node, $1);  // 添加子节点：$1 是 simple_expression，表示左侧简单表达式
        add_child(node, $3);  // 添加子节点：$3 是 simple_expression，表示右侧简单表达式
        $$ = node;  // 设置当前规则的返回值 $$ 为创建的节点
    }
    | simple_expression GREATER_EQUAL simple_expression
    {
        // 构建大于等于关系表达式
        Node *node = create_node("expression", "greater_equal");  // 创建 expression 节点，value 设为 "greater_equal"，表示这是一个大于等于关系运算
        add_child(node, $1);  // 添加子节点：$1 是 simple_expression，表示左侧简单表达式
        add_child(node, $3);  // 添加子节点：$3 是 simple_expression，表示右侧简单表达式
        $$ = node;  // 设置当前规则的返回值 $$ 为创建的节点
    }
    ;

/* expression_list -> expression | expression_list , expression
 * 定义 Pascal 语言中表达式列表（expression_list）的文法规则，支持以下两种形式：
 * 1. expression：单个表达式，例如 x + 1
 * 2. expression_list , expression：多个表达式的递归定义，例如 x + 1, y * 2
 * 该规则用于函数调用（function_call）或数组访问（id_varpart）中，表示一组表达式（例如函数参数或数组索引）
 */
expression_list:
    expression
    {
        // 构建单个表达式的表达式列表
        $$ = create_node("expression_list", NULL);  // 创建一个类型为 "expression_list" 的语法树节点，value 设为 NULL（expression_list 本身没有值，仅作为表达式列表的容器）
        add_child($$, $1);  // 添加子节点：$1 是 expression，表示单个表达式（例如 x + 1）
    }
    | expression_list COMMA expression
    {
        // 扩展现有的表达式列表（递归定义）
        $$ = $1;  // 复用 $1（现有的 expression_list 节点），避免创建新节点以提高效率
        add_child($$, $3);  // 添加子节点：$3 是 expression，表示新的表达式（例如 y * 2）
        // 注意：COMMA 是分隔符（,），未添加到语法树中，因为它不携带语义信息
    }
    ;

/* function_call -> id ( expression_list ) | id ( )
 * 定义 Pascal 语言中函数调用（function_call）的文法规则，支持以下两种形式：
 * 1. id ( expression_list )：带参数的函数调用，例如 func(x + 1, y * 2)
 * 2. id ( )：无参数的函数调用，例如 func()
 * 该规则用于因子（factor）中，表示一个函数调用（作为表达式的一部分）
 */
function_call:
    IDENTIFIER LPAREN expression_list RPAREN
    {
        // 构建带参数的函数调用
        Node *node = create_node("function_call", NULL);  // 创建一个类型为 "function_call" 的语法树节点，value 设为 NULL（function_call 本身没有值，仅作为函数调用的容器）
        add_child(node, create_node("id", $1));  // 添加子节点：类型为 "id"，value 设为函数名（$1 是 IDENTIFIER，例如 "func"）
        add_child(node, $3);  // 添加子节点：$3 是 expression_list，表示参数列表（例如 x + 1, y * 2）
        // 注意：LPAREN 和 RPAREN 是括号（( 和 )），未添加到语法树中
        $$ = node;  // 设置当前规则的返回值 $$ 为创建的节点
    }
    | IDENTIFIER LPAREN RPAREN
    {
        // 构建无参数的函数调用
        Node *node = create_node("function_call", NULL);  // 创建 function_call 节点
        add_child(node, create_node("id", $1));  // 添加子节点：类型为 "id"，value 设为函数名（例如 "func"）
        add_child(node, create_node("expression_list", "empty"));  // 添加子节点：expression_list 设为 "empty"，表示无参数
        $$ = node;  // 设置当前规则的返回值 $$ 为创建的节点
    }
    ;

/* variable_list -> variable | variable_list , variable
 * 定义 Pascal 语言中变量列表（variable_list）的文法规则，支持以下两种形式：
 * 1. variable：单个变量，例如 x
 * 2. variable_list , variable：多个变量的递归定义，例如 x, y[1]
 * 该规则用于输入输出语句（例如 read(x, y) 或 write(x, y[1])），表示一组变量
 */
variable_list:
    variable
    {
        // 构建单个变量的变量列表
        Node *node = create_node("variable_list", NULL);  // 创建一个类型为 "variable_list" 的语法树节点，value 设为 NULL（variable_list 本身没有值，仅作为变量列表的容器）
        add_child(node, $1);  // 添加子节点：$1 是 variable，表示单个变量（例如 x）
        $$ = node;  // 设置当前规则的返回值 $$ 为创建的节点
    }
    | variable_list COMMA variable
    {
        // 构建多个变量的变量列表（递归定义）
        Node *node = create_node("variable_list", NULL);  // 创建 variable_list 节点
        add_child(node, $1);  // 添加子节点：$1 是 variable_list，表示之前的变量列表（例如 x）
        add_child(node, $3);  // 添加子节点：$3 是 variable，表示新的变量（例如 y[1]）
        // 注意：COMMA 是分隔符（,），未添加到语法树中
        $$ = node;  // 设置当前规则的返回值 $$ 为创建的节点
    }
    ;

/* else_part -> ε | else statement
 * 定义 Pascal 语言中 else 部分（else_part）的文法规则，支持以下两种形式：
 * 1. ε（空）：表示没有 else 分支，例如 if x > 0 then y := 1
 * 2. else statement：else 分支，例如 else y := 0
 * 该规则用于 if 语句（if_statement），表示可选的 else 分支
 */
else_part:
    /* Empty */
    {
        // 处理空的 else 部分
        $$ = create_node("else_part", "empty");  // 创建一个类型为 "else_part" 的语法树节点，value 设为 "empty"，表示没有 else 分支
    }
    | ELSE statement
    {
        // 构建 else 分支
        Node *node = create_node("else_part", NULL);  // 创建 else_part 节点，value 设为 NULL（else_part 本身没有值，仅作为 else 分支的容器）
        add_child(node, $2);  // 添加子节点：$2 是 statement，表示 else 分支的语句（例如 y := 0）
        // 注意：ELSE 关键字未添加到语法树中
        $$ = node;  // 设置当前规则的返回值 $$ 为创建的节点
    }
    ;

/* statement -> ε | variable assignop expression | func_id assignop expression | procedure_call | compound_statement | if expression then statement else_part | for id assignop expression to expression do statement | read ( variable_list ) | write ( expression_list )
 * 定义 Pascal 语言中语句（statement）的文法规则，支持以下形式：
 * 1. variable assignop expression：赋值语句，例如 x := 1
 * 2. function_call：过程调用，例如 proc(x, y)（作为语句）
 * 3. compound_statement：复合语句，例如 begin ... end
 * 4. if expression then statement else_part：if 语句，例如 if x > 0 then y := 1 else y := 0
 * 5. for id assignop expression to expression do statement：for 循环，例如 for i := 1 to 10 do x := x + i
 * 6. read ( variable_list )：读语句，例如 read(x, y)
 * 7. write ( expression_list )：写语句，例如 write(x + 1, y)
 * 该规则用于程序体或子程序体的执行部分，表示一条语句
 */
statement:
    IDENTIFIER id_varpart ASSIGN expression
    {
        // 打印调试信息
        fprintf(stderr, "Processing assignment: IDENTIFIER = %s\n", $1 ? $1 : "NULL");  // 输出正在处理的赋值语句的变量名

        // 检查符号表是否有效
        if (!current_table) {  // 检查全局符号表 current_table 是否为 NULL
            fprintf(stderr, "Error: current_table is NULL in statement\n");  // 如果为 NULL，打印错误信息
            exit(1);  // 退出程序，表示符号表未正确初始化
        }

        // 查找变量符号
        Symbol *symbol = lookup_symbol($1);  // 在符号表中查找变量名（$1 是 IDENTIFIER，例如 "x"）
        fprintf(stderr, "lookup_symbol returned: %p\n", (void *)symbol);  // 打印查找结果的指针地址（用于调试）
        if (!symbol) {  // 如果变量未定义
            fprintf(stderr, "Symbol not found: %s\n", $1);  // 打印调试信息
            fprintf(output_file, "Error: Undefined identifier '%s' at line %d\n", $1, line_number);  // 输出错误信息到文件
        } else {  // 如果变量存在
            fprintf(stderr, "Symbol found: %s, kind: %s\n", $1, symbol->kind ? symbol->kind : "NULL");  // 打印变量信息
            if (strcmp(symbol->kind, "var") != 0) {  // 检查符号是否为变量（kind 应为 "var"）
                fprintf(output_file, "Error: '%s' is not a variable at line %d\n", $1, line_number);  // 如果不是变量，输出错误
            } else {  // 如果是变量
                // 检查右侧表达式的变量（如果存在）
                if ($4->child_count > 0 && strcmp($4->children[0]->type, "simple_expression") == 0 &&
                    $4->children[0]->child_count > 0 && strcmp($4->children[0]->children[0]->type, "term") == 0 &&
                    $4->children[0]->children[0]->child_count > 0 && strcmp($4->children[0]->children[0]->children[0]->type, "factor") == 0) {
                    Node *factor = $4->children[0]->children[0]->children[0];  // 获取右侧表达式的第一个因子（factor）
                    if (strcmp(factor->type, "variable") == 0) {  // 如果因子是一个变量
                        Symbol *rhs_symbol = lookup_symbol(factor->children[0]->children[0]->value);  // 查找右侧变量
                        if (!rhs_symbol) {  // 如果右侧变量未定义
                            fprintf(output_file, "Error: Undefined identifier '%s' in expression at line %d\n", factor->children[0]->children[0]->value, line_number);
                        }
                    }
                }

                // 更新符号表中的变量值
                if (symbol->value) free(symbol->value);  // 释放符号表中变量的旧值（避免内存泄漏）
                if ($4->child_count > 0 && strcmp($4->children[0]->type, "simple_expression") == 0 &&
                    $4->children[0]->child_count > 0 && strcmp($4->children[0]->children[0]->type, "term") == 0 &&
                    $4->children[0]->children[0]->child_count > 0 && strcmp($4->children[0]->children[0]->children[0]->type, "factor") == 0) {
                    Node *factor = $4->children[0]->children[0]->children[0];  // 获取右侧表达式的第一个因子
                    if (factor->value) {  // 如果因子有值（例如常数）
                        symbol->value = strdup(factor->value);  // 复制因子的值到符号表（例如 "1"）
                        if (!symbol->value) {  // 检查内存分配是否成功
                            fprintf(stderr, "Error: Failed to allocate memory for symbol value\n");
                            exit(1);
                        }
                    } else if (strcmp(factor->type, "variable") == 0) {  // 如果因子是一个变量
                        Symbol *rhs_symbol = lookup_symbol(factor->children[0]->children[0]->value);  // 查找右侧变量
                        if (rhs_symbol && rhs_symbol->value) {  // 如果右侧变量有值
                            symbol->value = strdup(rhs_symbol->value);  // 复制右侧变量的值
                            if (!symbol->value) {
                                fprintf(stderr, "Error: Failed to allocate memory for symbol value\n");
                                exit(1);
                            }
                        } else {  // 如果右侧变量无值
                            symbol->value = strdup("unknown");  // 设置为 "unknown"
                            if (!symbol->value) {
                                fprintf(stderr, "Error: Failed to allocate memory for symbol value\n");
                                exit(1);
                            }
                        }
                    } else {  // 如果因子无值（例如复杂表达式）
                        symbol->value = strdup("unknown");
                        if (!symbol->value) {
                            fprintf(stderr, "Error: Failed to allocate memory for symbol value\n");
                            exit(1);
                        }
                    }
                } else {  // 如果右侧表达式不是简单形式
                    symbol->value = strdup("unknown");
                    if (!symbol->value) {
                        fprintf(stderr, "Error: Failed to allocate memory for symbol value\n");
                        exit(1);
                    }
                }
            }
        }

        // 构建语法树节点
        Node *node = create_node("statement", "assign");  // 创建 statement 节点，value 设为 "assign"，表示这是一个赋值语句
        Node *var_node = create_node("variable", NULL);  // 创建 variable 节点
        add_child(var_node, create_node("id", $1));  // 添加变量名（例如 "x"）
        add_child(var_node, $2);  // 添加 id_varpart（例如数组索引或 empty）
        add_child(node, var_node);  // 将 variable 节点添加到 statement 节点
        add_child(node, $4);  // 添加 expression 节点（右侧表达式，例如 1）
        $$ = node;  // 设置当前规则的返回值 $$ 为创建的节点
    }
    | function_call
    {
        // 构建过程调用语句
        Node *node = create_node("statement", "procedure_call");  // 创建 statement 节点，value 设为 "procedure_call"
        add_child(node, $1);  // 添加子节点：$1 是 function_call，表示过程调用（例如 proc(x, y)）

        // 检查过程是否定义
        Symbol *symbol = lookup_symbol($1->children[0]->value);  // 查找过程名（function_call 的 id 节点）
        if (!symbol) {  // 如果未定义
            fprintf(output_file, "Error: Undefined identifier '%s' at line %d\n", $1->children[0]->value, line_number);
        }

        $$ = node;  // 设置当前规则的返回值 $$ 为创建的节点
    }
    | compound_statement
    {
        // 构建复合语句
        Node *node = create_node("statement", "compound");  // 创建 statement 节点，value 设为 "compound"
        add_child(node, $1);  // 添加子节点：$1 是 compound_statement（例如 begin ... end）
        $$ = node;  // 设置当前规则的返回值 $$ 为创建的节点
    }
    | IF expression THEN statement else_part
    {
        // 构建 if 语句
        Node *node = create_node("statement", "if");  // 创建 statement 节点，value 设为 "if"
        add_child(node, $2);  // 添加子节点：$2 是 expression，表示条件（例如 x > 0）
        add_child(node, $4);  // 添加子节点：$4 是 statement，表示 then 分支（例如 y := 1）
        add_child(node, $5);  // 添加子节点：$5 是 else_part，表示 else 分支（可以是 empty）
        $$ = node;  // 设置当前规则的返回值 $$ 为创建的节点
    }
    | FOR IDENTIFIER ASSIGN expression TO expression DO statement
    {
        // 检查循环变量
        Symbol *symbol = lookup_symbol($2);  // 查找循环变量（$2 是 IDENTIFIER，例如 "i"）
        if (!symbol) {  // 如果未定义
            fprintf(output_file, "Error: Undefined identifier '%s' at line %d\n", $2, line_number);
        } else if (strcmp(symbol->kind, "var") != 0) {  // 如果不是变量
            fprintf(output_file, "Error: '%s' is not a variable at line %d\n", $2, line_number);
        } else {  // 如果是变量
            // 更新循环变量的值
            if (symbol->value) free(symbol->value);  // 释放旧值
            if ($4->child_count > 0 && strcmp($4->children[0]->type, "simple_expression") == 0 &&
                $4->children[0]->child_count > 0 && strcmp($4->children[0]->children[0]->type, "term") == 0 &&
                $4->children[0]->children[0]->child_count > 0 && strcmp($4->children[0]->children[0]->children[0]->type, "factor") == 0) {
                Node *factor = $4->children[0]->children[0]->children[0];  // 获取初始表达式（$4）的第一个因子
                if (strcmp(factor->type, "variable") == 0) {  // 如果因子是一个变量
                    Symbol *rhs_symbol = lookup_symbol(factor->children[0]->children[0]->value);  // 查找变量
                    if (!rhs_symbol) {  // 如果未定义
                        fprintf(output_file, "Error: Undefined identifier '%s' in expression at line %d\n", factor->children[0]->children[0]->value, line_number);
                    }
                }
                if (factor->value) {  // 如果因子有值
                    symbol->value = strdup(factor->value);  // 复制值到循环变量
                } else if (strcmp(factor->type, "variable") == 0) {  // 如果因子是一个变量
                    Symbol *rhs_symbol = lookup_symbol(factor->children[0]->children[0]->value);
                    if (rhs_symbol && rhs_symbol->value) {  // 如果变量有值
                        symbol->value = strdup(rhs_symbol->value);
                    } else {  // 如果无值
                        symbol->value = strdup("unknown");
                    }
                } else {  // 如果因子无值
                    symbol->value = strdup("unknown");
                }
            } else {  // 如果初始表达式不是简单形式
                symbol->value = strdup("unknown");
            }
        }

        // 构建 for 循环语句
        Node *node = create_node("statement", "for");  // 创建 statement 节点，value 设为 "for"
        add_child(node, create_node("id", $2));  // 添加循环变量（例如 "i"）
        add_child(node, $4);  // 添加初始表达式（例如 1）
        add_child(node, $6);  // 添加终止表达式（例如 10）
        add_child(node, $8);  // 添加循环体语句（例如 x := x + i）
        $$ = node;  // 设置当前规则的返回值 $$ 为创建的节点
    }
    | READ LPAREN variable_list RPAREN
    {
        // 构建读语句
        Node *node = create_node("statement", "read");  // 创建 statement 节点，value 设为 "read"
        add_child(node, $3);  // 添加子节点：$3 是 variable_list，表示变量列表（例如 x, y）
        $$ = node;  // 设置当前规则的返回值 $$ 为创建的节点
    }
    | WRITE LPAREN expression_list RPAREN
    {
        // 构建写语句
        Node *node = create_node("statement", "write");  // 创建 statement 节点，value 设为 "write"
        add_child(node, $3);  // 添加子节点：$3 是 expression_list，表示表达式列表（例如 x + 1, y）
        $$ = node;  // 设置当前规则的返回值 $$ 为创建的节点
    }
    ;
%%



// 创建语法树节点
// 参数：
//   type - 节点的类型（例如 "statement"、"expression"），用于标识节点的语法类别
//   value - 节点的值（例如 "assign"、"42"），可以为 NULL，表示节点没有值
// 返回值：
//   指向新创建的 Node 结构体的指针
// 功能：
//   分配内存并初始化一个新的语法树节点，设置其类型和值，并初始化子节点列表为空
Node* create_node(const char *type, const char *value) {
    // 分配 Node 结构体内存
    Node *node = (Node *)malloc(sizeof(Node));
    if (!node) {  // 检查内存分配是否成功
        fprintf(stderr, "Error: Failed to allocate memory for node\n");
        exit(1);  // 如果失败，打印错误信息并退出程序
    }

    // 分配并复制节点类型
    node->type = strdup(type);
    if (!node->type) {  // 检查 strdup 是否成功
        fprintf(stderr, "Error: Failed to allocate memory for node type\n");
        free(node);  // 释放已分配的 Node 内存
        exit(1);  // 退出程序
    }

    // 分配并复制节点值（如果存在）
    node->value = value ? strdup(value) : NULL;  // 如果 value 不为 NULL，则复制；否则设为 NULL
    if (value && !node->value) {  // 检查 value 复制是否成功
        fprintf(stderr, "Error: Failed to allocate memory for node value: %s\n", value);
        free(node->type);  // 释放 type 内存
        free(node);  // 释放 Node 内存
        exit(1);  // 退出程序
    }

    // 打印调试信息
    fprintf(stderr, "Created node: type = %s, value = %s\n", node->type, node->value ? node->value : "NULL");

    // 初始化子节点列表
    node->child_count = 0;  // 初始子节点数量为 0
    node->children = NULL;  // 初始子节点数组为 NULL
    return node;  // 返回新创建的节点
}

// 添加子节点
// 参数：
//   parent - 父节点，子节点将被添加到此节点
//   child - 要添加的子节点
// 功能：
//   动态扩展父节点的子节点数组，并将新的子节点添加到数组末尾
void add_child(Node *parent, Node *child) {
    // 重新分配子节点数组内存，增加一个元素空间
    parent->children = (Node **)realloc(parent->children, (parent->child_count + 1) * sizeof(Node *));
    if (!parent->children) {  // 检查内存分配是否成功
        fprintf(stderr, "Error: Failed to allocate memory for children\n");
        exit(1);  // 如果失败，打印错误信息并退出程序
    }

    // 添加子节点
    parent->children[parent->child_count] = child;  // 将子节点添加到数组末尾
    parent->child_count++;  // 增加子节点计数
}

// 打印语法树（输出到 output_file）
// 参数：
//   node - 当前处理的语法树节点
//   level - 当前节点的缩进级别（用于表示树层次结构）
// 功能：
//   以树形结构将语法树输出到 output_file 文件，包含节点类型和值（如果有），并递归打印所有子节点
void print_tree(Node *node, int level) {
    if (!node) return;  // 如果节点为 NULL，直接返回（递归终止条件）

    // 打印缩进
    for (int i = 0; i < level; i++) fprintf(output_file, "  ");  // 根据层级打印空格（每层 2 个空格）

    // 打印节点类型和值
    fprintf(output_file, "%s", node->type);  // 打印节点类型（例如 "statement"）
    if (node->value) fprintf(output_file, " (%s)", node->value);  // 如果节点有值，打印值（例如 "(assign)"）
    fprintf(output_file, "\n");  // 换行

    // 递归打印子节点
    for (int i = 0; i < node->child_count; i++) {
        print_tree(node->children[i], level + 1);  // 递归调用，层级加 1
    }
}

// 打印详细的语法树（输出到指定文件）
// 参数：
//   node - 当前处理的语法树节点
//   parent - 当前节点的父节点（根节点的父节点为 NULL）
//   file - 输出文件指针
// 功能：
//   将语法树输出到指定文件，显式标识节点之间的父子关系
void print_detailed_tree(Node *node, Node *parent, FILE *file) {
    if (!node) return;  // 如果节点为 NULL，直接返回（递归终止条件）

    // 打印当前节点信息
    fprintf(file, "Node(type: %s, value: %s, child_count: %d, parent: ",
            node->type ? node->type : "NULL",
            node->value ? node->value : "NULL",
            node->child_count);

    // 打印父节点信息
    if (parent == NULL) {
        fprintf(file, "none");
    } else {
        fprintf(file, "[type: %s, value: %s]",
                parent->type ? parent->type : "NULL",
                parent->value ? parent->value : "NULL");
    }
    fprintf(file, ")\n");

    // 递归打印子节点，当前节点作为子节点的父节点
    for (int i = 0; i < node->child_count; i++) {
        print_detailed_tree(node->children[i], node, file);
    }
}

// 释放语法树内存
// 参数：
//   node - 要释放的语法树节点
// 功能：
//   递归释放语法树节点及其所有子节点的内存，包括 type、value 和 children 数组
void free_tree(Node *node) {
    if (!node) return;  // 如果节点为 NULL，直接返回（递归终止条件）

    // 释放节点类型
    free(node->type);  // 释放 type 字符串内存（由 strdup 分配）

    // 释放节点值（如果存在）
    if (node->value) free(node->value);  // 释放 value 字符串内存（由 strdup 分配）

    // 递归释放子节点
    for (int i = 0; i < node->child_count; i++) {
        free_tree(node->children[i]);  // 递归调用，释放子节点
    }

    // 释放子节点数组
    free(node->children);  // 释放 children 数组内存（由 realloc 分配）

    // 释放节点本身
    free(node);  // 释放 Node 结构体内存（由 malloc 分配）
}

