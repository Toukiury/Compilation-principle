
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C
   
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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.4.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Copy the first part of user declarations.  */

/* Line 189 of yacc.c  */
#line 1 "parser.y"

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




/* Line 189 of yacc.c  */
#line 256 "y.tab.c"

/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif

/* "%code requires" blocks.  */

/* Line 209 of yacc.c  */
#line 184 "parser.y"

    typedef struct Node {
        char *type;           // 节点类型（例如 "program", "var_declaration"）
        char *value;          // 节点值（如果有，例如标识符的值）
        struct Node **children; // 子节点数组
        int child_count;      // 子节点数量
    } Node;



/* Line 209 of yacc.c  */
#line 291 "y.tab.c"

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

/* Line 214 of yacc.c  */
#line 216 "parser.y"

    struct Node *node;
    char *str;
    int int_val;
    float float_val;
    char char_val;
    int bool_val;



/* Line 214 of yacc.c  */
#line 429 "y.tab.c"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

/* "%code provides" blocks.  */

/* Line 261 of yacc.c  */
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



/* Line 261 of yacc.c  */
#line 464 "y.tab.c"

/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 470 "y.tab.c"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  6
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   187

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  56
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  35
/* YYNRULES -- Number of rules.  */
#define YYNRULES  92
/* YYNRULES -- Number of states.  */
#define YYNSTATES  189

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   310

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     9,    12,    17,    22,    25,    28,    31,
      34,    36,    38,    40,    42,    44,    46,    48,    52,    58,
      60,    67,    69,    73,    77,    83,    84,    88,    92,    95,
      97,    99,   101,   105,   106,   110,   114,   120,   121,   125,
     126,   130,   134,   140,   144,   148,   152,   157,   159,   163,
     166,   167,   171,   173,   175,   177,   179,   181,   183,   187,
     190,   193,   195,   199,   203,   207,   211,   215,   217,   221,
     225,   229,   231,   235,   239,   243,   247,   251,   255,   257,
     261,   266,   270,   272,   276,   277,   280,   285,   287,   289,
     295,   304,   309
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      59,     0,    -1,    27,    22,    20,    64,    49,    -1,    27,
      22,    -1,    66,    73,    74,    78,    -1,    57,    46,    58,
      52,    -1,     5,    23,    -1,     5,    24,    -1,     4,    23,
      -1,     4,    24,    -1,    23,    -1,    24,    -1,    25,    -1,
      42,    -1,    43,    -1,    44,    -1,    45,    -1,    23,    53,
      23,    -1,    62,    48,    23,    53,    23,    -1,    61,    -1,
      40,    50,    62,    51,    41,    61,    -1,    22,    -1,    64,
      48,    22,    -1,    22,    17,    60,    -1,    65,    46,    22,
      17,    60,    -1,    -1,    29,    65,    46,    -1,    64,    47,
      61,    -1,    28,    67,    -1,    68,    -1,    67,    -1,    69,
      -1,    70,    46,    69,    -1,    -1,    20,    70,    49,    -1,
      64,    47,    63,    -1,    72,    46,    64,    47,    63,    -1,
      -1,    28,    72,    46,    -1,    -1,    74,    77,    46,    -1,
      30,    22,    71,    -1,    31,    22,    71,    47,    61,    -1,
      66,    73,    78,    -1,    75,    46,    76,    -1,    32,    79,
      33,    -1,    32,    79,    46,    33,    -1,    90,    -1,    79,
      46,    90,    -1,    22,    81,    -1,    -1,    50,    86,    51,
      -1,    23,    -1,    24,    -1,    25,    -1,    26,    -1,    80,
      -1,    87,    -1,    20,    85,    49,    -1,    11,    82,    -1,
       4,    82,    -1,    82,    -1,    83,    10,    82,    -1,    83,
       9,    82,    -1,    83,     8,    82,    -1,    83,     7,    82,
      -1,    83,     6,    82,    -1,    83,    -1,    84,     5,    83,
      -1,    84,     4,    83,    -1,    84,     3,    83,    -1,    84,
      -1,    84,    17,    84,    -1,    84,    16,    84,    -1,    84,
      15,    84,    -1,    84,    14,    84,    -1,    84,    13,    84,
      -1,    84,    12,    84,    -1,    85,    -1,    86,    48,    85,
      -1,    22,    20,    86,    49,    -1,    22,    20,    49,    -1,
      80,    -1,    88,    48,    80,    -1,    -1,    19,    90,    -1,
      22,    81,    21,    85,    -1,    87,    -1,    78,    -1,    34,
      85,    18,    90,    89,    -1,    35,    22,    21,    85,    36,
      85,    37,    90,    -1,    38,    20,    88,    49,    -1,    39,
      20,    86,    49,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   267,   267,   288,   313,   333,   355,   362,   369,   376,
     383,   390,   397,   414,   420,   425,   430,   444,   460,   488,
     494,   514,   532,   571,   591,   623,   628,   647,   667,   687,
     693,   708,   718,   740,   745,   765,   814,   869,   874,   895,
     900,   921,   967,  1027,  1049,  1074,  1085,  1105,  1111,  1126,
    1147,  1151,  1173,  1181,  1189,  1196,  1202,  1209,  1216,  1224,
    1231,  1252,  1284,  1293,  1301,  1309,  1317,  1336,  1346,  1355,
    1363,  1381,  1387,  1396,  1404,  1412,  1420,  1428,  1445,  1451,
    1467,  1476,  1493,  1500,  1519,  1523,  1545,  1633,  1647,  1654,
    1663,  1709,  1716
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "OR", "MINUS", "PLUS", "AND", "MOD",
  "DIV", "DIVIDE", "MULTIPLY", "NOT", "GREATER_EQUAL", "GREATER",
  "LESS_EQUAL", "LESS", "NOT_EQUAL", "EQUAL", "THEN", "ELSE", "LPAREN",
  "ASSIGN", "IDENTIFIER", "INTEGER_CONST", "REAL_CONST", "CHAR_CONST",
  "BOOLEAN_CONST", "PROGRAM", "VAR", "CONST", "PROCEDURE", "FUNCTION",
  "TOKEN_BEGIN", "END", "IF", "FOR", "TO", "DO", "READ", "WRITE", "ARRAY",
  "OF", "INTEGER", "REAL", "BOOLEAN", "CHAR", "SEMICOLON", "COLON",
  "COMMA", "RPAREN", "LBRACKET", "RBRACKET", "DOT", "DOTDOT", "EOF_TOKEN",
  "UMINUS", "$accept", "program_head", "program_body", "program",
  "const_value", "basic_type", "period", "type", "idlist",
  "const_declaration", "const_declarations", "value_parameter",
  "var_parameter", "parameter", "parameter_list", "formal_parameter",
  "var_declaration", "var_declarations", "subprogram_declarations",
  "subprogram_head", "subprogram_body", "subprogram", "compound_statement",
  "statement_list", "variable", "id_varpart", "factor", "term",
  "simple_expression", "expression", "expression_list", "function_call",
  "variable_list", "else_part", "statement", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    56,    57,    57,    58,    59,    60,    60,    60,    60,
      60,    60,    60,    61,    61,    61,    61,    62,    62,    63,
      63,    64,    64,    65,    65,    66,    66,    67,    68,    69,
      69,    70,    70,    71,    71,    72,    72,    73,    73,    74,
      74,    75,    75,    76,    77,    78,    78,    79,    79,    80,
      81,    81,    82,    82,    82,    82,    82,    82,    82,    82,
      82,    83,    83,    83,    83,    83,    83,    84,    84,    84,
      84,    85,    85,    85,    85,    85,    85,    85,    86,    86,
      87,    87,    88,    88,    89,    89,    90,    90,    90,    90,
      90,    90,    90
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     5,     2,     4,     4,     2,     2,     2,     2,
       1,     1,     1,     1,     1,     1,     1,     3,     5,     1,
       6,     1,     3,     3,     5,     0,     3,     3,     2,     1,
       1,     1,     3,     0,     3,     3,     5,     0,     3,     0,
       3,     3,     5,     3,     3,     3,     4,     1,     3,     2,
       0,     3,     1,     1,     1,     1,     1,     1,     3,     2,
       2,     1,     3,     3,     3,     3,     3,     1,     3,     3,
       3,     1,     3,     3,     3,     3,     3,     3,     1,     3,
       4,     3,     1,     3,     0,     2,     4,     1,     1,     5,
       8,     4,     4
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     0,     0,     0,     3,    25,     1,     0,     0,     0,
      37,    21,     0,     0,     0,     5,     0,    39,     0,     2,
       0,    26,     0,     0,     0,    22,     0,     0,    10,    11,
      12,    23,     0,     0,    38,     0,     0,     0,     0,     0,
       4,     8,     9,     6,     7,     0,     0,    13,    14,    15,
      16,    19,    35,     0,    33,    33,    50,     0,     0,     0,
       0,    88,     0,    87,    47,    25,    40,    24,     0,     0,
       0,    41,     0,     0,     0,     0,     0,     0,     0,    50,
      52,    53,    54,    55,    56,    61,    67,    71,     0,    57,
       0,     0,     0,    45,     0,    37,    44,     0,     0,    36,
       0,     0,    30,    29,    31,     0,     0,    81,    78,     0,
       0,     0,    60,    59,     0,    49,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    50,    82,     0,     0,    46,    48,     0,     0,
       0,     0,    28,     0,     0,    34,    42,     0,    80,    51,
      86,    58,    66,    65,    64,    63,    62,    70,    69,    68,
      77,    76,    75,    74,    73,    72,    84,     0,     0,    91,
      92,    43,    17,     0,     0,    27,    32,    79,     0,    89,
       0,    83,     0,    20,    85,     0,    18,     0,    90
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     2,     9,     3,    31,    51,    98,    52,   101,    14,
      10,   102,   103,   104,   105,    71,    23,    17,    24,    38,
      96,    39,    61,    62,    84,   115,    85,    86,    87,   108,
     109,    89,   134,   179,    64
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -102
static const yytype_int16 yypact[] =
{
     -18,    -6,   -26,    24,    20,    35,  -102,    52,    64,    40,
      73,  -102,    -4,   106,    79,  -102,    52,  -102,    93,  -102,
       7,   118,    23,   107,    67,  -102,    71,    81,  -102,  -102,
    -102,  -102,   134,     6,    52,   130,   132,    68,   109,   110,
    -102,  -102,  -102,  -102,  -102,     7,   108,  -102,  -102,  -102,
    -102,  -102,  -102,    91,   137,   137,   -14,    43,   138,   139,
     141,  -102,   -12,  -102,  -102,    35,  -102,  -102,   140,     6,
      15,  -102,   115,     3,    43,   143,    43,    43,    43,   -14,
    -102,  -102,  -102,  -102,  -102,  -102,    70,   105,   147,  -102,
     145,   146,    43,  -102,    50,    73,  -102,   114,   -38,  -102,
      52,    95,  -102,  -102,  -102,    -8,    69,  -102,  -102,    96,
       5,    43,  -102,  -102,   120,  -102,    43,    43,    43,    43,
      43,    43,    43,    43,    43,    43,    43,    43,    43,    43,
      68,    43,   121,  -102,    98,   100,  -102,  -102,   142,   149,
     150,   135,  -102,    69,    15,  -102,  -102,    43,  -102,  -102,
    -102,  -102,  -102,  -102,  -102,  -102,  -102,    70,    70,    70,
     129,   129,   129,   129,   129,   129,   151,   144,   146,  -102,
    -102,  -102,  -102,   122,    69,  -102,  -102,  -102,    68,  -102,
      43,  -102,   154,  -102,  -102,   148,  -102,    68,  -102
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -102,  -102,  -102,  -102,   133,  -101,  -102,   112,     1,  -102,
     117,    83,  -102,    42,  -102,   124,  -102,    89,  -102,  -102,
    -102,  -102,   -22,  -102,   -87,   131,   -58,    14,     2,   -56,
     -59,   -37,  -102,  -102,   -91
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_uint8 yytable[] =
{
      63,    88,    40,   137,   133,   146,    73,    76,    12,     1,
     140,    26,    27,   141,    77,   110,     4,    22,   112,   113,
       5,    93,   114,    78,     6,    79,    80,    81,    82,    83,
      28,    29,    30,   135,    94,    53,    74,    11,   144,   166,
       7,   145,   175,   100,    18,    19,    46,    76,    47,    48,
      49,    50,   107,   147,    77,   150,   149,    63,   152,   153,
     154,   155,   156,    78,     8,    79,    80,    81,    82,    83,
      33,    18,    56,   183,    11,   167,   116,   117,   118,   119,
     120,   181,    37,   136,    57,    58,    13,   184,    59,    60,
      56,   177,    15,    63,    41,    42,   188,    35,    36,    37,
      37,    16,    57,    58,    43,    44,    59,    60,   121,   122,
     123,    47,    48,    49,    50,    25,   171,   124,   125,   126,
     127,   128,   129,    20,   185,    21,   160,   161,   162,   163,
     164,   165,   121,   122,   123,   157,   158,   159,    69,    18,
      32,    63,   143,    18,   147,   148,   168,   169,   147,   170,
      63,    45,    54,    34,    55,    65,    66,    70,    68,    91,
      90,    92,   106,    97,   111,   130,   131,   139,   132,   151,
     178,    74,   172,   173,    37,   182,   174,   186,    67,    72,
     180,    99,    95,   142,   138,   187,   176,    75
};

static const yytype_uint8 yycheck[] =
{
      37,    57,    24,    94,    91,   106,    20,     4,     7,    27,
      48,     4,     5,    51,    11,    74,    22,    16,    76,    77,
      46,    33,    78,    20,     0,    22,    23,    24,    25,    26,
      23,    24,    25,    92,    46,    34,    50,    22,    46,   130,
      20,    49,   143,    28,    48,    49,    40,     4,    42,    43,
      44,    45,    49,    48,    11,   111,    51,    94,   116,   117,
     118,   119,   120,    20,    29,    22,    23,    24,    25,    26,
      47,    48,    22,   174,    22,   131,     6,     7,     8,     9,
      10,   168,    32,    33,    34,    35,    22,   178,    38,    39,
      22,   147,    52,   130,    23,    24,   187,    30,    31,    32,
      32,    28,    34,    35,    23,    24,    38,    39,     3,     4,
       5,    42,    43,    44,    45,    22,   138,    12,    13,    14,
      15,    16,    17,    17,   180,    46,   124,   125,   126,   127,
     128,   129,     3,     4,     5,   121,   122,   123,    47,    48,
      22,   178,    47,    48,    48,    49,    48,    49,    48,    49,
     187,    17,    22,    46,    22,    46,    46,    20,    50,    20,
      22,    20,    47,    23,    21,    18,    21,    53,    22,    49,
      19,    50,    23,    23,    32,    53,    41,    23,    45,    55,
      36,    69,    65,   100,    95,    37,   144,    56
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    27,    57,    59,    22,    46,     0,    20,    29,    58,
      66,    22,    64,    22,    65,    52,    28,    73,    48,    49,
      17,    46,    64,    72,    74,    22,     4,     5,    23,    24,
      25,    60,    22,    47,    46,    30,    31,    32,    75,    77,
      78,    23,    24,    23,    24,    17,    40,    42,    43,    44,
      45,    61,    63,    64,    22,    22,    22,    34,    35,    38,
      39,    78,    79,    87,    90,    46,    46,    60,    50,    47,
      20,    71,    71,    20,    50,    81,     4,    11,    20,    22,
      23,    24,    25,    26,    80,    82,    83,    84,    85,    87,
      22,    20,    20,    33,    46,    66,    76,    23,    62,    63,
      28,    64,    67,    68,    69,    70,    47,    49,    85,    86,
      86,    21,    82,    82,    85,    81,     6,     7,     8,     9,
      10,     3,     4,     5,    12,    13,    14,    15,    16,    17,
      18,    21,    22,    80,    88,    86,    33,    90,    73,    53,
      48,    51,    67,    47,    46,    49,    61,    48,    49,    51,
      85,    49,    82,    82,    82,    82,    82,    83,    83,    83,
      84,    84,    84,    84,    84,    84,    90,    85,    48,    49,
      49,    78,    23,    23,    41,    61,    69,    85,    19,    89,
      36,    80,    53,    61,    90,    85,    23,    37,    90
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}

/* Prevent warnings from -Wmissing-prototypes.  */
#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */


/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*-------------------------.
| yyparse or yypush_parse.  |
`-------------------------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{


    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.

       Refer to the stacks thru separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yytoken = 0;
  yyss = yyssa;
  yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */
  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:

/* Line 1455 of yacc.c  */
#line 268 "parser.y"
    {
        // 创建全局符号表
        current_table = create_symbol_table(NULL);  // 调用 create_symbol_table(NULL) 创建一个新的符号表，作为全局作用域，传入 NULL 表示没有父作用域（这是最顶层作用域），并赋值给全局变量 current_table
        fprintf(stderr, "Created symbol table for program: %s\n", (yyvsp[(2) - (5)].str));  // 打印调试信息，显示创建符号表时的程序名（$2 是 IDENTIFIER 的值，例如 "main"），输出到 stderr 便于调试

        // 插入程序名到符号表
        insert_symbol((yyvsp[(2) - (5)].str), "program", NULL, NULL);  // 调用 insert_symbol 将程序名插入符号表
        // 参数说明：
        // $2：程序名（IDENTIFIER 的值，例如 "main"）
        // "program"：符号种类，表示这是一个程序名
        // NULL：类型，程序名没有类型，设为 NULL
        // NULL：值，程序名没有值，设为 NULL

        // 构建语法树节点
        Node *node = create_node("program_head", NULL);  // 创建一个类型为 "program_head" 的节点，value 设为 NULL（program_head 本身没有值）
        add_child(node, create_node("program", "program"));  // 添加子节点：类型为 "program"，值设为 "program"，表示关键字 program
        add_child(node, create_node("id", (yyvsp[(2) - (5)].str)));  // 添加子节点：类型为 "id"，值设为程序名（$2，例如 "main"）
        add_child(node, (yyvsp[(4) - (5)].node));  // 添加子节点：$4 是 idlist，表示参数列表（例如 input, output）
        (yyval.node) = node;  // 设置当前规则的返回值 $$ 为创建的节点，供上层规则（如 program）使用
    }
    break;

  case 3:

/* Line 1455 of yacc.c  */
#line 289 "parser.y"
    {
        // 创建全局符号表
        current_table = create_symbol_table(NULL);  // 同上，创建全局符号表并赋值给 current_table
        fprintf(stderr, "Created symbol table for program: %s\n", (yyvsp[(2) - (2)].str));  // 同上，打印调试信息

        // 插入程序名到符号表
        insert_symbol((yyvsp[(2) - (2)].str), "program", NULL, NULL);  // 同上，插入程序名到符号表

        // 构建语法树节点
        Node *node = create_node("program_head", NULL);  // 创建 "program_head" 节点，value 设为 NULL
        add_child(node, create_node("program", "program"));  // 添加子节点：表示关键字 program
        add_child(node, create_node("id", (yyvsp[(2) - (2)].str)));  // 添加子节点：表示程序名（$2）
        (yyval.node) = node;  // 设置返回值 $$ 为创建的节点
    }
    break;

  case 4:

/* Line 1455 of yacc.c  */
#line 314 "parser.y"
    {
        // 构建语法树节点
        Node *node = create_node("program_body", NULL);  // 创建一个类型为 "program_body" 的节点，value 设为 NULL（program_body 本身没有值）
        add_child(node, (yyvsp[(1) - (4)].node));  // 添加子节点：$1 是 const_declarations，表示常量声明部分
        add_child(node, (yyvsp[(2) - (4)].node));  // 添加子节点：$2 是 var_declarations，表示变量声明部分
        add_child(node, (yyvsp[(3) - (4)].node));  // 添加子节点：$3 是 subprogram_declarations，表示子程序声明部分
        add_child(node, (yyvsp[(4) - (4)].node));  // 添加子节点：$4 是 compound_statement，表示主程序体
        (yyval.node) = node;  // 设置当前规则的返回值 $$ 为创建的节点，供上层规则（如 program）使用
    }
    break;

  case 5:

/* Line 1455 of yacc.c  */
#line 334 "parser.y"
    {
        // 构建语法树根节点
        Node *node = create_node("program", NULL);  // 创建一个类型为 "program" 的节点，作为整个语法树的根节点，value 设为 NULL（program 本身没有值）
        add_child(node, (yyvsp[(1) - (4)].node));  // 添加子节点：$1 是 program_head，表示程序头
        add_child(node, (yyvsp[(3) - (4)].node));  // 添加子节点：$3 是 program_body，表示程序体
        // 注意：SEMICOLON 和 DOT 是分隔符，未添加到语法树中，因为它们不携带语义信息
        root = node;  // 将创建的节点赋值给全局变量 root，作为语法树的根节点，后续可通过 root 打印或释放语法树
        (yyval.node) = node;  // 设置当前规则的返回值 $$ 为创建的节点（program 是起始符号，$$ 通常不会被使用，但仍需设置）
    }
    break;

  case 6:

/* Line 1455 of yacc.c  */
#line 356 "parser.y"
    {
        // 将正号和整数值格式化为字符串
        char value[32];  // 定义一个字符数组，用于存储格式化后的字符串值
        snprintf(value, sizeof(value), "+%d", (yyvsp[(2) - (2)].int_val));  // 使用 snprintf 格式化正整数值，$2 是 INTEGER_CONST 的值（整数，例如 42），格式为 "+42"
        (yyval.node) = create_node("const_value", value);  // 创建一个类型为 "const_value" 的语法树节点，value 字段设为格式化后的字符串（例如 "+42"）
    }
    break;

  case 7:

/* Line 1455 of yacc.c  */
#line 363 "parser.y"
    {
        // 将正号和实数值格式化为字符串
        char value[32];  // 定义字符数组，用于存储格式化后的字符串值
        snprintf(value, sizeof(value), "+%.6f", (yyvsp[(2) - (2)].float_val));  // 使用 snprintf 格式化正实数值，$2 是 REAL_CONST 的值（浮点数，例如 3.14），格式为 "+3.140000"（保留 6 位小数）
        (yyval.node) = create_node("const_value", value);  // 创建 "const_value" 节点，value 字段设为格式化后的字符串（例如 "+3.140000"）
    }
    break;

  case 8:

/* Line 1455 of yacc.c  */
#line 370 "parser.y"
    {
        // 将负号和整数值格式化为字符串
        char value[32];  // 定义字符数组，用于存储格式化后的字符串值
        snprintf(value, sizeof(value), "-%d", (yyvsp[(2) - (2)].int_val));  // 使用 snprintf 格式化负整数值，$2 是 INTEGER_CONST 的值（整数，例如 42），格式为 "-42"
        (yyval.node) = create_node("const_value", value);  // 创建 "const_value" 节点，value 字段设为格式化后的字符串（例如 "-42"）
    }
    break;

  case 9:

/* Line 1455 of yacc.c  */
#line 377 "parser.y"
    {
        // 将负号和实数值格式化为字符串
        char value[32];  // 定义字符数组，用于存储格式化后的字符串值
        snprintf(value, sizeof(value), "-%.6f", (yyvsp[(2) - (2)].float_val));  // 使用 snprintf 格式化负实数值，$2 是 REAL_CONST 的值（浮点数，例如 3.14），格式为 "-3.140000"（保留 6 位小数）
        (yyval.node) = create_node("const_value", value);  // 创建 "const_value" 节点，value 字段设为格式化后的字符串（例如 "-3.140000"）
    }
    break;

  case 10:

/* Line 1455 of yacc.c  */
#line 384 "parser.y"
    {
        // 将无符号整数值格式化为字符串
        char value[32];  // 定义字符数组，用于存储格式化后的字符串值
        snprintf(value, sizeof(value), "%d", (yyvsp[(1) - (1)].int_val));  // 使用 snprintf 格式化整数值，$1 是 INTEGER_CONST 的值（整数，例如 42），格式为 "42"
        (yyval.node) = create_node("const_value", value);  // 创建 "const_value" 节点，value 字段设为格式化后的字符串（例如 "42"）
    }
    break;

  case 11:

/* Line 1455 of yacc.c  */
#line 391 "parser.y"
    {
        // 将无符号实数值格式化为字符串
        char value[32];  // 定义字符数组，用于存储格式化后的字符串值
        snprintf(value, sizeof(value), "%.6f", (yyvsp[(1) - (1)].float_val));  // 使用 snprintf 格式化实数值，$1 是 REAL_CONST 的值（浮点数，例如 3.14），格式为 "3.140000"（保留 6 位小数）
        (yyval.node) = create_node("const_value", value);  // 创建 "const_value" 节点，value 字段设为格式化后的字符串（例如 "3.140000"）
    }
    break;

  case 12:

/* Line 1455 of yacc.c  */
#line 398 "parser.y"
    {
        // 将字符常量格式化为字符串
        char value[2] = { (yyvsp[(1) - (1)].char_val), '\0' };  // 定义字符数组，$1 是 CHAR_CONST 的值（单个字符，例如 'A'），添加终止符 '\0' 形成字符串
        (yyval.node) = create_node("const_value", value);  // 创建 "const_value" 节点，value 字段设为字符值（例如 "A"）
    }
    break;

  case 13:

/* Line 1455 of yacc.c  */
#line 415 "parser.y"
    {
        // 创建表示整数类型的语法树节点
        (yyval.node) = create_node("basic_type", "integer");  // 创建一个类型为 "basic_type" 的语法树节点，value 字段设为 "integer"，表示基本类型是整数类型
        // $$ 是当前规则的返回值，将该节点传递给上层规则（例如 var_declaration）
    }
    break;

  case 14:

/* Line 1455 of yacc.c  */
#line 421 "parser.y"
    {
        // 创建表示实数类型的语法树节点
        (yyval.node) = create_node("basic_type", "real");  // 创建一个类型为 "basic_type" 的语法树节点，value 字段设为 "real"，表示基本类型是实数类型
    }
    break;

  case 15:

/* Line 1455 of yacc.c  */
#line 426 "parser.y"
    {
        // 创建表示布尔类型的语法树节点
        (yyval.node) = create_node("basic_type", "boolean");  // 创建一个类型为 "basic_type" 的语法树节点，value 字段设为 "boolean"，表示基本类型是布尔类型
    }
    break;

  case 16:

/* Line 1455 of yacc.c  */
#line 431 "parser.y"
    {
        // 创建表示字符类型的语法树节点
        (yyval.node) = create_node("basic_type", "char");  // 创建一个类型为 "basic_type" 的语法树节点，value 字段设为 "char"，表示基本类型是字符类型
    }
    break;

  case 17:

/* Line 1455 of yacc.c  */
#line 445 "parser.y"
    {
        // 构建单个范围的语法树节点
        Node *node = create_node("period", NULL);  // 创建一个类型为 "period" 的语法树节点，value 设为 NULL（period 本身没有值，仅作为范围的容器）

        // 格式化范围的起始值和结束值为字符串
        char start[32], end[32];  // 定义两个字符数组，分别用于存储范围的起始值和结束值
        snprintf(start, sizeof(start), "%d", (yyvsp[(1) - (3)].int_val));  // 使用 snprintf 格式化起始值，$1 是第一个 INTEGER_CONST 的值（整数，例如 1），格式为 "1"
        snprintf(end, sizeof(end), "%d", (yyvsp[(3) - (3)].int_val));  // 使用 snprintf 格式化结束值，$3 是第二个 INTEGER_CONST 的值（整数，例如 10），格式为 "10"

        // 添加子节点表示范围的起始值和结束值
        add_child(node, create_node("start", start));  // 添加子节点：类型为 "start"，value 设为格式化后的起始值（例如 "1"）
        add_child(node, create_node("end", end));  // 添加子节点：类型为 "end"，value 设为格式化后的结束值（例如 "10"）

        (yyval.node) = node;  // 设置当前规则的返回值 $$ 为创建的节点，供上层规则（例如 array_type）使用
    }
    break;

  case 18:

/* Line 1455 of yacc.c  */
#line 461 "parser.y"
    {
        // 构建多个范围的语法树节点（递归定义）
        Node *node = create_node("period", NULL);  // 创建一个类型为 "period" 的语法树节点，value 设为 NULL（period 本身没有值，仅作为范围的容器）

        // 添加前一个 period 节点作为子节点
        add_child(node, (yyvsp[(1) - (5)].node));  // 添加子节点：$1 是前一个 period，表示之前的范围集合（例如 1..10）

        // 格式化新范围的起始值和结束值为字符串
        char start[32], end[32];  // 定义两个字符数组，分别用于存储新范围的起始值和结束值
        snprintf(start, sizeof(start), "%d", (yyvsp[(3) - (5)].int_val));  // 使用 snprintf 格式化起始值，$3 是第三个 INTEGER_CONST 的值（整数，例如 20），格式为 "20"
        snprintf(end, sizeof(end), "%d", (yyvsp[(5) - (5)].int_val));  // 使用 snprintf 格式化结束值，$5 是第五个 INTEGER_CONST 的值（整数，例如 30），格式为 "30"

        // 添加子节点表示新范围的起始值和结束值
        add_child(node, create_node("start", start));  // 添加子节点：类型为 "start"，value 设为格式化后的起始值（例如 "20"）
        add_child(node, create_node("end", end));  // 添加子节点：类型为 "end"，value 设为格式化后的结束值（例如 "30"）

        (yyval.node) = node;  // 设置当前规则的返回值 $$ 为创建的节点，供上层规则或递归调用使用
    }
    break;

  case 19:

/* Line 1455 of yacc.c  */
#line 489 "parser.y"
    {
        // 直接使用基本类型的语法树节点
        (yyval.node) = (yyvsp[(1) - (1)].node);  // 将 basic_type 的节点直接作为 type 的返回值，$1 是 basic_type 的语法树节点（例如 basic_type (integer)）
        // 这种形式表示类型是一个基本类型，无需额外构建新节点
    }
    break;

  case 20:

/* Line 1455 of yacc.c  */
#line 495 "parser.y"
    {
        // 构建数组类型的语法树节点
        Node *node = create_node("array_type", NULL);  // 创建一个类型为 "array_type" 的语法树节点，value 设为 NULL（array_type 本身没有值，仅作为数组类型的容器）

        // 添加子节点表示数组的索引范围和元素类型
        add_child(node, (yyvsp[(3) - (6)].node));  // 添加子节点：$3 是 period，表示数组的索引范围（例如 1..10）
        add_child(node, (yyvsp[(6) - (6)].node));  // 添加子节点：$6 是 basic_type，表示数组元素的类型（例如 integer）

        (yyval.node) = node;  // 设置当前规则的返回值 $$ 为创建的节点，供上层规则（例如 var_declaration）使用
    }
    break;

  case 21:

/* Line 1455 of yacc.c  */
#line 515 "parser.y"
    {
        // 打印调试信息，显示正在创建的标识符列表
        fprintf(stderr, "Creating idlist with IDENTIFIER: %s\n", (yyvsp[(1) - (1)].str));  // 输出调试信息到 stderr，显示当前处理的标识符（$1 是 IDENTIFIER 的值，例如 "x"）

        // 复制标识符字符串
        char *id_copy = strdup((yyvsp[(1) - (1)].str));  // 使用 strdup 复制 $1（IDENTIFIER 的值，例如 "x"），以确保内存安全，防止后续修改原始字符串
        if (!id_copy) {  // 检查内存分配是否成功
            fprintf(stderr, "Error: Failed to allocate memory for id_copy\n");  // 如果 strdup 失败，打印错误信息
            exit(1);  // 退出程序，表示内存分配错误
        }

        // 构建语法树节点
        Node *node = create_node("idlist", NULL);  // 创建一个类型为 "idlist" 的语法树节点，value 设为 NULL（idlist 本身没有值，仅作为标识符列表的容器）
        add_child(node, create_node("id", id_copy));  // 添加子节点：类型为 "id"，value 设为复制的标识符（例如 "x"）

        (yyval.node) = node;  // 设置当前规则的返回值 $$ 为创建的节点，供上层规则（例如 var_declarations 或 program_head）使用
    }
    break;

  case 22:

/* Line 1455 of yacc.c  */
#line 533 "parser.y"
    {
        // 打印调试信息，显示正在添加的标识符
        fprintf(stderr, "Adding IDENTIFIER to idlist: %s\n", (yyvsp[(3) - (3)].str));  // 输出调试信息到 stderr，显示当前添加的标识符（$3 是 IDENTIFIER 的值，例如 "y"）

        // 复制标识符字符串
        char *id_copy = strdup((yyvsp[(3) - (3)].str));  // 使用 strdup 复制 $3（IDENTIFIER 的值，例如 "y"），确保内存安全
        if (!id_copy) {  // 检查内存分配是否成功
            fprintf(stderr, "Error: Failed to allocate memory for id_copy\n");  // 如果 strdup 失败，打印错误信息
            exit(1);  // 退出程序，表示内存分配错误
        }

        // 构建新的 idlist 节点
        Node *node = create_node("idlist", NULL);  // 创建一个类型为 "idlist" 的语法树节点，value 设为 NULL（idlist 本身没有值，仅作为标识符列表的容器）

        // 将前一个 idlist 的子节点（id 节点）转移到新的 idlist 节点
        Node *prev_idlist = (yyvsp[(1) - (3)].node);  // $1 是前一个 idlist 的语法树节点，包含之前的标识符（例如 "x"）
        for (int i = 0; i < prev_idlist->child_count; i++) {  // 遍历前一个 idlist 的所有子节点
            add_child(node, prev_idlist->children[i]);  // 将前一个 idlist 的子节点（id 节点）添加到新节点中
        }

        // 避免内存释放问题
        prev_idlist->child_count = 0;  // 清空前一个 idlist 的子节点数量，因为子节点已被转移
        prev_idlist->children = NULL;  // 清空前一个 idlist 的子节点数组，避免后续释放时重复释放内存

        // 添加新的 id 节点
        add_child(node, create_node("id", id_copy));  // 添加子节点：类型为 "id"，value 设为复制的标识符（例如 "y"）

        (yyval.node) = node;  // 设置当前规则的返回值 $$ 为创建的节点，供上层规则或递归调用使用
    }
    break;

  case 23:

/* Line 1455 of yacc.c  */
#line 572 "parser.y"
    {
        // 插入常量到符号表
        insert_symbol((yyvsp[(1) - (3)].str), "const", NULL, (yyvsp[(3) - (3)].node)->value);  // 调用 insert_symbol 将常量插入符号表
        // 参数说明：
        // $1：常量名（IDENTIFIER 的值，例如 "x"）
        // "const"：符号种类，表示这是一个常量
        // NULL：类型，常量在此处未指定类型（后续可能由 const_value 推导），设为 NULL
        // $3->value：常量值，$3 是 const_value 的节点，其 value 字段存储常量值（例如 "42"）

        // 打印常量声明信息
        fprintf(output_file, "Inserted constant '%s' with value '%s'\n", (yyvsp[(1) - (3)].str), (yyvsp[(3) - (3)].node)->value);  // 输出常量声明信息到 output_file，显示插入的常量名（$1）和值（$3->value），便于调试或记录

        // 构建语法树节点
        Node *node = create_node("const_declaration", NULL);  // 创建一个类型为 "const_declaration" 的语法树节点，value 设为 NULL（const_declaration 本身没有值，仅作为常量声明的容器）
        add_child(node, create_node("id", (yyvsp[(1) - (3)].str)));  // 添加子节点：类型为 "id"，value 设为常量名（例如 "x"）
        add_child(node, (yyvsp[(3) - (3)].node));  // 添加子节点：$3 是 const_value，表示常量的值（例如 const_value (42)）

        (yyval.node) = node;  // 设置当前规则的返回值 $$ 为创建的节点，供上层规则（例如 const_declarations）使用
    }
    break;

  case 24:

/* Line 1455 of yacc.c  */
#line 592 "parser.y"
    {
        // 插入常量到符号表
        insert_symbol((yyvsp[(3) - (5)].str), "const", NULL, (yyvsp[(5) - (5)].node)->value);  // 调用 insert_symbol 将常量插入符号表
        // 参数说明：
        // $3：常量名（IDENTIFIER 的值，例如 "y"）
        // "const"：符号种类，表示这是一个常量
        // NULL：类型，常量未指定类型，设为 NULL
        // $5->value：常量值，$5 是 const_value 的节点，其 value 字段存储常量值（例如 "3.14"）

        // 打印常量声明信息
        fprintf(output_file, "Inserted constant '%s' with value '%s'\n", (yyvsp[(3) - (5)].str), (yyvsp[(5) - (5)].node)->value);  // 输出常量声明信息到 output_file，显示插入的常量名（$3）和值（$5->value）

        // 构建语法树节点
        Node *node = create_node("const_declaration", NULL);  // 创建一个类型为 "const_declaration" 的语法树节点，value 设为 NULL
        add_child(node, (yyvsp[(1) - (5)].node));  // 添加子节点：$1 是前一个 const_declaration，表示之前的常量声明（例如 const x = 42）
        add_child(node, create_node("id", (yyvsp[(3) - (5)].str)));  // 添加子节点：类型为 "id"，value 设为新的常量名（例如 "y"）
        add_child(node, (yyvsp[(5) - (5)].node));  // 添加子节点：$5 是 const_value，表示新常量的值（例如 const_value (3.14)）
        // 注意：SEMICOLON 是分隔符，未添加到语法树中，因为它不携带语义信息

        (yyval.node) = node;  // 设置当前规则的返回值 $$ 为创建的节点，供上层规则或递归调用使用
    }
    break;

  case 25:

/* Line 1455 of yacc.c  */
#line 623 "parser.y"
    {
        // 处理空常量声明
        (yyval.node) = create_node("const_declarations", "empty");  // 创建一个类型为 "const_declarations" 的语法树节点，value 设为 "empty"，表示没有常量声明
        // $$ 是当前规则的返回值，将该节点传递给上层规则（例如 program_body）
    }
    break;

  case 26:

/* Line 1455 of yacc.c  */
#line 629 "parser.y"
    {
        // 构建常量声明部分的语法树节点
        Node *node = create_node("const_declarations", NULL);  // 创建一个类型为 "const_declarations" 的语法树节点，value 设为 NULL（const_declarations 本身没有值，仅作为常量声明的容器）

        // 添加子节点
        add_child(node, (yyvsp[(2) - (3)].node));  // 添加子节点：$2 是 const_declaration，表示具体的常量声明（例如 const x = 42; y = 3.14;）
        // 注意：CONST 关键字和 SEMICOLON 分隔符未添加到语法树中，因为它们不携带语义信息

        (yyval.node) = node;  // 设置当前规则的返回值 $$ 为创建的节点，供上层规则（例如 program_body）使用
    }
    break;

  case 27:

/* Line 1455 of yacc.c  */
#line 648 "parser.y"
    {
        // 构建值参数的语法树节点
        Node *node = create_node("value_parameter", NULL);  // 创建一个类型为 "value_parameter" 的语法树节点，value 设为 NULL（value_parameter 本身没有值，仅作为值参数声明的容器）

        // 添加子节点
        add_child(node, (yyvsp[(1) - (3)].node));  // 添加子节点：$1 是 idlist，表示参数名列表（例如 x, y）
        add_child(node, (yyvsp[(3) - (3)].node));  // 添加子节点：$3 是 basic_type，表示参数的类型（例如 integer）
        // 注意：COLON 是分隔符（:），未添加到语法树中，因为它不携带语义信息

        (yyval.node) = node;  // 设置当前规则的返回值 $$ 为创建的节点，供上层规则（例如 parameter 或 parameter_list）使用
    }
    break;

  case 28:

/* Line 1455 of yacc.c  */
#line 668 "parser.y"
    {
        // 构建变量参数的语法树节点
        Node *node = create_node("var_parameter", NULL);  // 创建一个类型为 "var_parameter" 的语法树节点，value 设为 NULL（var_parameter 本身没有值，仅作为变量参数声明的容器）

        // 添加子节点
        add_child(node, (yyvsp[(2) - (2)].node));  // 添加子节点：$2 是 value_parameter，表示具体的参数声明（例如 x, y: integer）
        // 注意：VAR 关键字未添加到语法树中，因为它不携带语义信息，仅用于区分值参数和变量参数

        (yyval.node) = node;  // 设置当前规则的返回值 $$ 为创建的节点，供上层规则（例如 parameter 或 parameter_list）使用
    }
    break;

  case 29:

/* Line 1455 of yacc.c  */
#line 688 "parser.y"
    {
        // 直接使用变量参数的语法树节点
        (yyval.node) = (yyvsp[(1) - (1)].node);  // 将 var_parameter 的节点直接作为 parameter 的返回值，$1 是 var_parameter 的语法树节点（例如 var_parameter 包含 value_parameter(x: integer)）
        // 这种形式表示参数是一个变量参数，无需额外构建新节点
    }
    break;

  case 30:

/* Line 1455 of yacc.c  */
#line 694 "parser.y"
    {
        // 直接使用值参数的语法树节点
        (yyval.node) = (yyvsp[(1) - (1)].node);  // 将 value_parameter 的节点直接作为 parameter 的返回值，$1 是 value_parameter 的语法树节点（例如 value_parameter(x: integer)）
        // 这种形式表示参数是一个值参数，无需额外构建新节点
    }
    break;

  case 31:

/* Line 1455 of yacc.c  */
#line 709 "parser.y"
    {
        // 构建单个参数的参数列表语法树节点
        Node *node = create_node("parameter_list", NULL);  // 创建一个类型为 "parameter_list" 的语法树节点，value 设为 NULL（parameter_list 本身没有值，仅作为参数列表的容器）

        // 添加子节点
        add_child(node, (yyvsp[(1) - (1)].node));  // 添加子节点：$1 是 parameter，表示单个参数（例如 parameter 包含 value_parameter(x: integer)）

        (yyval.node) = node;  // 设置当前规则的返回值 $$ 为创建的节点，供上层规则（例如 subprogram_head）使用
    }
    break;

  case 32:

/* Line 1455 of yacc.c  */
#line 719 "parser.y"
    {
        // 构建多个参数的参数列表语法树节点（递归定义）
        Node *node = create_node("parameter_list", NULL);  // 创建一个类型为 "parameter_list" 的语法树节点，value 设为 NULL（parameter_list 本身没有值，仅作为参数列表的容器）

        // 添加子节点
        add_child(node, (yyvsp[(1) - (3)].node));  // 添加子节点：$1 是前一个 parameter_list，表示之前的参数列表（例如 parameter_list 包含 parameter(x: integer)）
        add_child(node, (yyvsp[(3) - (3)].node));  // 添加子节点：$3 是 parameter，表示新的参数（例如 parameter 包含 var_parameter(y: real)）
        // 注意：SEMICOLON 是分隔符（;），未添加到语法树中，因为它不携带语义信息

        (yyval.node) = node;  // 设置当前规则的返回值 $$ 为创建的节点，供上层规则或递归调用使用
    }
    break;

  case 33:

/* Line 1455 of yacc.c  */
#line 740 "parser.y"
    {
        // 处理空形式参数
        (yyval.node) = create_node("formal_parameter", "empty");  // 创建一个类型为 "formal_parameter" 的语法树节点，value 设为 "empty"，表示没有形式参数
        // $$ 是当前规则的返回值，将该节点传递给上层规则（例如 subprogram_head）
    }
    break;

  case 34:

/* Line 1455 of yacc.c  */
#line 746 "parser.y"
    {
        // 构建带参数列表的形式参数语法树节点
        Node *node = create_node("formal_parameter", NULL);  // 创建一个类型为 "formal_parameter" 的语法树节点，value 设为 NULL（formal_parameter 本身没有值，仅作为形式参数的容器）

        // 添加子节点
        add_child(node, (yyvsp[(2) - (3)].node));  // 添加子节点：$2 是 parameter_list，表示参数列表（例如 parameter_list 包含 parameter(x: integer)）
        // 注意：LPAREN 和 RPAREN 是括号分隔符（( 和 )），未添加到语法树中，因为它们不携带语义信息

        (yyval.node) = node;  // 设置当前规则的返回值 $$ 为创建的节点，供上层规则（例如 subprogram_head）使用
    }
    break;

  case 35:

/* Line 1455 of yacc.c  */
#line 766 "parser.y"
    {
        // 打印调试信息，表示开始插入变量
        fprintf(stderr, "Inserting variables into symbol table\n");  // 输出调试信息到 stderr，表示正在处理变量插入

        // 检查符号表是否有效
        if (!current_table) {  // 检查全局符号表 current_table 是否为 NULL
            fprintf(stderr, "Error: current_table is NULL in var_declaration\n");  // 如果 current_table 为 NULL，打印错误信息
            exit(1);  // 退出程序，表示符号表未正确初始化
        }

        // 确定变量的类型
        char *var_type = (yyvsp[(3) - (3)].node)->type;  // 初始将 var_type 设为 type 节点的 type 字段（例如 "basic_type" 或 "array_type"）
        if (strcmp((yyvsp[(3) - (3)].node)->type, "array_type") == 0) {  // 如果 type 是数组类型（array_type）
            char array_type[128];  // 定义字符数组，用于存储格式化后的数组类型字符串
            snprintf(array_type, sizeof(array_type), "array[%s..%s] of %s",
                     (yyvsp[(3) - (3)].node)->children[0]->children[0]->value,  // 数组范围的起始值（period -> start，例如 "1"）
                     (yyvsp[(3) - (3)].node)->children[0]->children[1]->value,  // 数组范围的结束值（period -> end，例如 "10"）
                     (yyvsp[(3) - (3)].node)->children[1]->value);  // 数组元素类型（basic_type，例如 "integer"）
            var_type = array_type;  // 更新 var_type 为格式化后的数组类型字符串（例如 "array[1..10] of integer"）
        } else {
            var_type = (yyvsp[(3) - (3)].node)->value;  // 如果是基本类型（basic_type），直接使用 type 节点的 value 字段（例如 "integer"）
        }

        // 遍历 idlist，插入每个变量到符号表
        Node *idlist_node = (yyvsp[(1) - (3)].node);  // $1 是 idlist，表示变量名列表（例如 x, y）
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
        add_child(node, (yyvsp[(1) - (3)].node));  // 添加子节点：$1 是 idlist，表示变量名列表
        add_child(node, (yyvsp[(3) - (3)].node));  // 添加子节点：$3 是 type，表示变量的类型（例如 basic_type(integer) 或 array_type）
        // 注意：COLON 是分隔符（:），未添加到语法树中，因为它不携带语义信息

        (yyval.node) = node;  // 设置当前规则的返回值 $$ 为创建的节点，供上层规则（例如 var_declarations）使用
    }
    break;

  case 36:

/* Line 1455 of yacc.c  */
#line 815 "parser.y"
    {
        // 打印调试信息，表示继续插入变量（多个声明）
        fprintf(stderr, "Inserting variables into symbol table (continued)\n");  // 输出调试信息，表示处理后续变量声明

        // 检查符号表是否有效
        if (!current_table) {  // 检查全局符号表 current_table 是否为 NULL
            fprintf(stderr, "Error: current_table is NULL in var_declaration\n");  // 如果 current_table 为 NULL，打印错误信息
            exit(1);  // 退出程序，表示符号表未正确初始化
        }

        // 确定变量的类型
        char *var_type = (yyvsp[(5) - (5)].node)->type;  // 初始将 var_type 设为 type 节点的 type 字段（例如 "basic_type" 或 "array_type"）
        if (strcmp((yyvsp[(5) - (5)].node)->type, "array_type") == 0) {  // 如果 type 是数组类型（array_type）
            char array_type[128];  // 定义字符数组，用于存储格式化后的数组类型字符串
            snprintf(array_type, sizeof(array_type), "array[%s..%s] of %s",
                     (yyvsp[(5) - (5)].node)->children[0]->children[0]->value,  // 数组范围的起始值（period -> start，例如 "20"）
                     (yyvsp[(5) - (5)].node)->children[0]->children[1]->value,  // 数组范围的结束值（period -> end，例如 "30"）
                     (yyvsp[(5) - (5)].node)->children[1]->value);  // 数组元素类型（basic_type，例如 "real"）
            var_type = array_type;  // 更新 var_type 为格式化后的数组类型字符串（例如 "array[20..30] of real"）
        } else {
            var_type = (yyvsp[(5) - (5)].node)->value;  // 如果是基本类型（basic_type），直接使用 type 节点的 value 字段（例如 "real"）
        }

        // 遍历 idlist，插入每个变量到符号表
        Node *idlist_node = (yyvsp[(3) - (5)].node);  // $3 是 idlist，表示新的变量名列表（例如 z）
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
        add_child(node, (yyvsp[(1) - (5)].node));  // 添加子节点：$1 是前一个 var_declaration，表示之前的变量声明（例如 x, y: integer）
        add_child(node, (yyvsp[(3) - (5)].node));  // 添加子节点：$3 是 idlist，表示新的变量名列表（例如 z）
        add_child(node, (yyvsp[(5) - (5)].node));  // 添加子节点：$5 是 type，表示新变量的类型（例如 basic_type(real)）
        // 注意：SEMICOLON 和 COLON 是分隔符，未添加到语法树中

        (yyval.node) = node;  // 设置当前规则的返回值 $$ 为创建的节点，供上层规则或递归调用使用
    }
    break;

  case 37:

/* Line 1455 of yacc.c  */
#line 869 "parser.y"
    {
        // 处理空变量声明
        (yyval.node) = create_node("var_declarations", "empty");  // 创建一个类型为 "var_declarations" 的语法树节点，value 设为 "empty"，表示没有变量声明
        // $$ 是当前规则的返回值，将该节点传递给上层规则（例如 program_body）
    }
    break;

  case 38:

/* Line 1455 of yacc.c  */
#line 875 "parser.y"
    {
        // 构建变量声明部分的语法树节点
        Node *node = create_node("var_declarations", NULL);  // 创建一个类型为 "var_declarations" 的语法树节点，value 设为 NULL（var_declarations 本身没有值，仅作为变量声明的容器）

        // 添加子节点
        add_child(node, (yyvsp[(2) - (3)].node));  // 添加子节点：$2 是 var_declaration，表示具体的变量声明（例如 var x, y: integer;）
        // 注意：VAR 关键字和 SEMICOLON 分隔符未添加到语法树中，因为它们不携带语义信息

        (yyval.node) = node;  // 设置当前规则的返回值 $$ 为创建的节点，供上层规则（例如 program_body）使用
    }
    break;

  case 39:

/* Line 1455 of yacc.c  */
#line 895 "parser.y"
    {
        // 处理空子程序声明
        (yyval.node) = create_node("subprogram_declarations", "empty");  // 创建一个类型为 "subprogram_declarations" 的语法树节点，value 设为 "empty"，表示没有子程序声明
        // $$ 是当前规则的返回值，将该节点传递给上层规则（例如 program_body）
    }
    break;

  case 40:

/* Line 1455 of yacc.c  */
#line 901 "parser.y"
    {
        // 构建子程序声明部分的语法树节点（递归定义）
        Node *node = create_node("subprogram_declarations", NULL);  // 创建一个类型为 "subprogram_declarations" 的语法树节点，value 设为 NULL（subprogram_declarations 本身没有值，仅作为子程序声明的容器）

        // 添加子节点
        add_child(node, (yyvsp[(1) - (3)].node));  // 添加子节点：$1 是前一个 subprogram_declarations，表示之前的子程序声明（例如 procedure proc1;）
        add_child(node, (yyvsp[(2) - (3)].node));  // 添加子节点：$2 是 subprogram，表示新的子程序声明（例如 procedure proc2;）
        // 注意：SEMICOLON 是分隔符（;），未添加到语法树中，因为它不携带语义信息

        (yyval.node) = node;  // 设置当前规则的返回值 $$ 为创建的节点，供上层规则（例如 program_body）或递归调用使用
    }
    break;

  case 41:

/* Line 1455 of yacc.c  */
#line 922 "parser.y"
    {
        // 插入过程名到符号表
        insert_symbol((yyvsp[(2) - (3)].str), "procedure", NULL, NULL);  // 调用 insert_symbol 将过程名插入符号表
        // 参数说明：
        // $2：过程名（IDENTIFIER 的值，例如 "proc"）
        // "procedure"：符号种类，表示这是一个过程
        // NULL：类型，过程没有返回值类型，设为 NULL
        // NULL：值，过程没有初始值，设为 NULL

        // 创建新的作用域
        SymbolTable *new_table = create_symbol_table(current_table);  // 调用 create_symbol_table 创建一个新的符号表，作为子程序的作用域，current_table 是父作用域
        current_table = new_table;  // 更新全局变量 current_table 为新的子程序作用域，后续符号插入将在此作用域中进行

        // 插入参数到符号表
        Node *param_list = (yyvsp[(3) - (3)].node);  // $3 是 formal_parameter，表示形式参数（例如 formal_parameter 包含 parameter_list(x: integer)）
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
        add_child(node, create_node("id", (yyvsp[(2) - (3)].str)));  // 添加子节点：类型为 "id"，value 设为过程名（例如 "proc"）
        add_child(node, (yyvsp[(3) - (3)].node));  // 添加子节点：$3 是 formal_parameter，表示形式参数
        // 注意：PROCEDURE 关键字未添加到语法树中，因为它不携带语义信息

        (yyval.node) = node;  // 设置当前规则的返回值 $$ 为创建的节点，供上层规则（例如 subprogram）使用
    }
    break;

  case 42:

/* Line 1455 of yacc.c  */
#line 968 "parser.y"
    {
        // 插入函数名到符号表
        insert_symbol((yyvsp[(2) - (5)].str), "function", (yyvsp[(5) - (5)].node)->value, NULL);  // 调用 insert_symbol 将函数名插入符号表
        // 参数说明：
        // $2：函数名（IDENTIFIER 的值，例如 "func"）
        // "function"：符号种类，表示这是一个函数
        // $5->value：返回值类型（basic_type 的 value，例如 "integer"）
        // NULL：值，函数没有初始值，设为 NULL

        // 创建新的作用域
        SymbolTable *new_table = create_symbol_table(current_table);  // 创建新的子程序作用域，current_table 是父作用域
        current_table = new_table;  // 更新全局变量 current_table 为新的子程序作用域

        // 插入参数到符号表
        Node *param_list = (yyvsp[(3) - (5)].node);  // $3 是 formal_parameter，表示形式参数
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
        insert_symbol((yyvsp[(2) - (5)].str), "var", (yyvsp[(5) - (5)].node)->value, NULL);  // 将函数名作为变量插入符号表，用于存储返回值
        // 参数说明：
        // $2：函数名（例如 "func"）
        // "var"：符号种类，表示这是一个变量（用于返回值）
        // $5->value：变量类型（即函数返回值类型，例如 "integer"）
        // NULL：初始值，设为 NULL

        // 构建语法树节点
        Node *node = create_node("subprogram_head", "function");  // 创建一个类型为 "subprogram_head" 的语法树节点，value 设为 "function"，表示这是一个函数
        add_child(node, create_node("id", (yyvsp[(2) - (5)].str)));  // 添加子节点：类型为 "id"，value 设为函数名（例如 "func"）
        add_child(node, (yyvsp[(3) - (5)].node));  // 添加子节点：$3 是 formal_parameter，表示形式参数
        add_child(node, (yyvsp[(5) - (5)].node));  // 添加子节点：$5 是 basic_type，表示返回值类型（例如 basic_type(integer)）
        // 注意：FUNCTION 关键字和 COLON 分隔符未添加到语法树中

        (yyval.node) = node;  // 设置当前规则的返回值 $$ 为创建的节点，供上层规则（例如 subprogram）使用
    }
    break;

  case 43:

/* Line 1455 of yacc.c  */
#line 1028 "parser.y"
    {
        // 构建子程序体的语法树节点
        Node *node = create_node("subprogram_body", NULL);  // 创建一个类型为 "subprogram_body" 的语法树节点，value 设为 NULL（subprogram_body 本身没有值，仅作为子程序体的容器）

        // 添加子节点
        add_child(node, (yyvsp[(1) - (3)].node));  // 添加子节点：$1 是 const_declarations，表示常量声明部分（可以是 empty）
        add_child(node, (yyvsp[(2) - (3)].node));  // 添加子节点：$2 是 var_declarations，表示变量声明部分（可以是 empty）
        add_child(node, (yyvsp[(3) - (3)].node));  // 添加子节点：$3 是 compound_statement，表示子程序的执行语句（例如 begin ... end）

        (yyval.node) = node;  // 设置当前规则的返回值 $$ 为创建的节点，供上层规则（例如 subprogram）使用
    }
    break;

  case 44:

/* Line 1455 of yacc.c  */
#line 1050 "parser.y"
    {
        // 构建子程序的语法树节点
        Node *node = create_node("subprogram", NULL);  // 创建一个类型为 "subprogram" 的语法树节点，value 设为 NULL（subprogram 本身没有值，仅作为子程序的容器）

        // 添加子节点
        add_child(node, (yyvsp[(1) - (3)].node));  // 添加子节点：$1 是 subprogram_head，表示子程序头部（包含名称、参数等）
        add_child(node, (yyvsp[(3) - (3)].node));  // 添加子节点：$3 是 subprogram_body，表示子程序体（包含声明和语句）
        // 注意：SEMICOLON 是分隔符，未添加到语法树中，因为它不携带语义信息

        // 切换回父作用域
        current_table = current_table->parent;  // 将全局变量 current_table 切换回父作用域，表示子程序作用域的结束
        // 在 subprogram_head 中创建了子程序作用域（new_table），此处恢复到父作用域（例如全局作用域），以便后续声明或子程序使用正确的符号表

        (yyval.node) = node;  // 设置当前规则的返回值 $$ 为创建的节点，供上层规则（例如 subprogram_declarations）使用
    }
    break;

  case 45:

/* Line 1455 of yacc.c  */
#line 1075 "parser.y"
    {
        // 构建复合语句的语法树节点
        Node *node = create_node("compound_statement", NULL);  // 创建一个类型为 "compound_statement" 的语法树节点，value 设为 NULL（compound_statement 本身没有值，仅作为复合语句的容器）

        // 添加子节点
        add_child(node, (yyvsp[(2) - (3)].node));  // 添加子节点：$2 是 statement_list，表示语句列表（例如 x := 1）
        // 注意：TOKEN_BEGIN 和 END 是关键字（begin 和 end），未添加到语法树中，因为它们不携带语义信息

        (yyval.node) = node;  // 设置当前规则的返回值 $$ 为创建的节点，供上层规则（例如 subprogram_body 或 program_body）使用
    }
    break;

  case 46:

/* Line 1455 of yacc.c  */
#line 1086 "parser.y"
    {
        // 构建复合语句的语法树节点（带分号形式）
        Node *node = create_node("compound_statement", NULL);  // 创建一个类型为 "compound_statement" 的语法树节点，value 设为 NULL

        // 添加子节点
        add_child(node, (yyvsp[(2) - (4)].node));  // 添加子节点：$2 是 statement_list，表示语句列表
        // 注意：SEMICOLON 是分隔符（;），未添加到语法树中，符合 Pascal 语法中可选分号的设计

        (yyval.node) = node;  // 设置当前规则的返回值 $$ 为创建的节点，供上层规则使用
    }
    break;

  case 47:

/* Line 1455 of yacc.c  */
#line 1106 "parser.y"
    {
        // 构建单个语句的语句列表
        (yyval.node) = create_node("statement_list", NULL);  // 创建一个类型为 "statement_list" 的语法树节点，value 设为 NULL（statement_list 本身没有值，仅作为语句列表的容器）
        add_child((yyval.node), (yyvsp[(1) - (1)].node));  // 添加子节点：$1 是 statement，表示单个语句（例如 x := 1）
    }
    break;

  case 48:

/* Line 1455 of yacc.c  */
#line 1112 "parser.y"
    {
        // 扩展现有的语句列表（递归定义）
        (yyval.node) = (yyvsp[(1) - (3)].node);  // 复用 $1（现有的 statement_list 节点），避免创建新节点以提高效率
        add_child((yyval.node), (yyvsp[(3) - (3)].node));  // 添加子节点：$3 是 statement，表示新的语句（例如 y := 2）
        // 注意：SEMICOLON 是分隔符（;），未添加到语法树中，因为它不携带语义信息
    }
    break;

  case 49:

/* Line 1455 of yacc.c  */
#line 1127 "parser.y"
    {
        // 构建变量的语法树节点
        Node *node = create_node("variable", NULL);  // 创建一个类型为 "variable" 的语法树节点，value 设为 NULL（variable 本身没有值，仅作为变量引用的容器）

        // 添加子节点
        add_child(node, create_node("id", (yyvsp[(1) - (2)].str)));  // 添加子节点：类型为 "id"，value 设为变量名（$1 是 IDENTIFIER，例如 "x"）
        add_child(node, (yyvsp[(2) - (2)].node));  // 添加子节点：$2 是 id_varpart，表示变量的附加部分（例如数组索引，可以是 empty）

        (yyval.node) = node;  // 设置当前规则的返回值 $$ 为创建的节点，供上层规则（例如赋值语句或表达式）使用
    }
    break;

  case 50:

/* Line 1455 of yacc.c  */
#line 1147 "parser.y"
    {
        // 处理简单变量（无数组索引）
        (yyval.node) = create_node("id_varpart", "empty");  // 创建一个类型为 "id_varpart" 的语法树节点，value 设为 "empty"，表示没有数组访问
    }
    break;

  case 51:

/* Line 1455 of yacc.c  */
#line 1152 "parser.y"
    {
        // 构建数组访问的附加部分
        Node *node = create_node("id_varpart", "array_access");  // 创建一个类型为 "id_varpart" 的语法树节点，value 设为 "array_access"，表示这是一个数组访问
        add_child(node, (yyvsp[(2) - (3)].node));  // 添加子节点：$2 是 expression_list，表示数组索引表达式列表（例如 1 或 i, j）
        // 注意：LBRACKET 和 RBRACKET 是括号（[ 和 ]），未添加到语法树中，因为它们不携带语义信息

        (yyval.node) = node;  // 设置当前规则的返回值 $$ 为创建的节点，供上层规则（例如 variable）使用
    }
    break;

  case 52:

/* Line 1455 of yacc.c  */
#line 1174 "parser.y"
    {
        // 处理整数常量
        char value[32];  // 定义字符数组，用于存储格式化后的整数值
        snprintf(value, sizeof(value), "%d", (yyvsp[(1) - (1)].int_val));  // 使用 snprintf 格式化 INTEGER_CONST 的值（$1，例如 42），存储为字符串 "42"
        (yyval.node) = create_node("factor", value);  // 创建一个类型为 "factor" 的语法树节点，value 设为格式化后的字符串（例如 "42"）
        (yyval.node)->value = strdup(value);  // 复制字符串到节点的 value 字段，确保内存安全（防止 value 数组被覆盖）
    }
    break;

  case 53:

/* Line 1455 of yacc.c  */
#line 1182 "parser.y"
    {
        // 处理实数常量
        char value[32];  // 定义字符数组，用于存储格式化后的实数值
        snprintf(value, sizeof(value), "%.6f", (yyvsp[(1) - (1)].float_val));  // 使用 snprintf 格式化 REAL_CONST 的值（$1，例如 3.14），保留 6 位小数，存储为字符串 "3.140000"
        (yyval.node) = create_node("factor", value);  // 创建 factor 节点，value 设为格式化后的字符串
        (yyval.node)->value = strdup(value);  // 复制字符串到节点的 value 字段
    }
    break;

  case 54:

/* Line 1455 of yacc.c  */
#line 1190 "parser.y"
    {
        // 处理字符常量
        char value[2] = { (yyvsp[(1) - (1)].char_val), '\0' };  // 创建字符数组，存储 CHAR_CONST 的值（$1，例如 'a'），并以空字符结尾，格式为 "a"
        (yyval.node) = create_node("factor", value);  // 创建 factor 节点，value 设为字符字符串
        (yyval.node)->value = strdup(value);  // 复制字符串到节点的 value 字段
    }
    break;

  case 55:

/* Line 1455 of yacc.c  */
#line 1197 "parser.y"
    {
        // 处理布尔常量
        (yyval.node) = create_node("factor", (yyvsp[(1) - (1)].bool_val) ? "true" : "false");  // 创建 factor 节点，value 设为 "true" 或 "false"（根据 $1 的值，$1 为 1 表示 true，为 0 表示 false）
        (yyval.node)->value = strdup((yyvsp[(1) - (1)].bool_val) ? "true" : "false");  // 复制字符串到节点的 value 字段
    }
    break;

  case 56:

/* Line 1455 of yacc.c  */
#line 1203 "parser.y"
    {
        // 处理变量引用
        Node *node = create_node("factor", "variable");  // 创建 factor 节点，value 设为 "variable"，表示这是一个变量因子
        add_child(node, (yyvsp[(1) - (1)].node));  // 添加子节点：$1 是 variable，表示变量引用（例如 variable(x) 或 variable(x[1])）
        (yyval.node) = node;  // 设置当前规则的返回值 $$ 为创建的节点
    }
    break;

  case 57:

/* Line 1455 of yacc.c  */
#line 1210 "parser.y"
    {
        // 处理函数调用
        Node *node = create_node("factor", "function_call");  // 创建 factor 节点，value 设为 "function_call"，表示这是一个函数调用因子
        add_child(node, (yyvsp[(1) - (1)].node));  // 添加子节点：$1 是 function_call，表示函数调用（例如 func(1, 2)）
        (yyval.node) = node;  // 设置当前规则的返回值 $$ 为创建的节点
    }
    break;

  case 58:

/* Line 1455 of yacc.c  */
#line 1217 "parser.y"
    {
        // 处理括号表达式
        Node *node = create_node("factor", "expression");  // 创建 factor 节点，value 设为 "expression"，表示这是一个括号表达式因子
        add_child(node, (yyvsp[(2) - (3)].node));  // 添加子节点：$2 是 expression，表示括号内的表达式（例如 x + 1）
        // 注意：LPAREN 和 RPAREN 是括号（( 和 )），未添加到语法树中，因为它们不携带语义信息
        (yyval.node) = node;  // 设置当前规则的返回值 $$ 为创建的节点
    }
    break;

  case 59:

/* Line 1455 of yacc.c  */
#line 1225 "parser.y"
    {
        // 处理逻辑非操作
        Node *node = create_node("factor", "not");  // 创建 factor 节点，value 设为 "not"，表示这是一个逻辑非操作
        add_child(node, (yyvsp[(2) - (2)].node));  // 添加子节点：$2 是 factor，表示被取非的因子（例如 true）
        (yyval.node) = node;  // 设置当前规则的返回值 $$ 为创建的节点
    }
    break;

  case 60:

/* Line 1455 of yacc.c  */
#line 1232 "parser.y"
    {
        // 处理一元负号操作
        Node *node = create_node("factor", "uminus");  // 创建 factor 节点，value 设为 "uminus"，表示这是一个一元负号操作
        add_child(node, (yyvsp[(2) - (2)].node));  // 添加子节点：$2 是 factor，表示被取负的因子（例如 5）
        // 注意：%prec UMINUS 用于解决优先级冲突，确保 MINUS 被解析为一元负号（而不是减法运算符）
        (yyval.node) = node;  // 设置当前规则的返回值 $$ 为创建的节点
    }
    break;

  case 61:

/* Line 1455 of yacc.c  */
#line 1253 "parser.y"
    {
        // 构建单个因子的项
        (yyval.node) = create_node("term", NULL);  // 创建一个类型为 "term" 的语法树节点，value 初始设为 NULL（term 本身没有值，仅作为项的容器）
        add_child((yyval.node), (yyvsp[(1) - (1)].node));  // 添加子节点：$1 是 factor，表示单个因子（例如 42 或 x）

        // 设置 term 的值（用于后续计算或调试）
        if (strcmp((yyvsp[(1) - (1)].node)->type, "factor") == 0) {  // 确保 $1 是 factor 节点
            if (strcmp((yyvsp[(1) - (1)].node)->value, "variable") == 0) {  // 如果 factor 是一个变量（factor 的 value 为 "variable"）
                // 从符号表获取变量的值
                Node *var_node = (yyvsp[(1) - (1)].node)->children[0];  // 获取 factor 的子节点 variable
                Node *id_node = var_node->children[0];  // 获取 variable 的子节点 id（变量名，例如 "x"）
                Symbol *sym = lookup_symbol(id_node->value);  // 在符号表中查找变量名
                if (sym && sym->value) {  // 如果变量存在且有值
                    (yyval.node)->value = strdup(sym->value);  // 复制变量的值到 term 节点的 value 字段（例如 "42"）
                } else {  // 如果变量未定义或无值
                    (yyval.node)->value = strdup("unknown");  // 设置 term 的值为 "unknown"
                }
            } else {  // 如果 factor 是常数（例如 42、3.14、true）
                // 直接使用 factor 的值
                (yyval.node)->value = strdup((yyvsp[(1) - (1)].node)->value);  // 复制 factor 的值到 term 节点的 value 字段（例如 "42"）
            }
        } else {  // 如果 $1 不是 factor 节点（理论上不会发生，但作为防御性编程）
            (yyval.node)->value = strdup("unknown");  // 设置 term 的值为 "unknown"
        }

        // 检查内存分配是否成功
        if (!(yyval.node)->value) {  // 如果 strdup 失败（内存分配失败）
            fprintf(stderr, "Error: Failed to allocate memory for term value\n");  // 打印错误信息
            exit(1);  // 退出程序，表示内存分配错误
        }
    }
    break;

  case 62:

/* Line 1455 of yacc.c  */
#line 1285 "parser.y"
    {
        // 构建乘法项
        Node *node = create_node("term", "multiply");  // 创建 term 节点，value 设为 "multiply"，表示这是一个乘法操作
        add_child(node, (yyvsp[(1) - (3)].node));  // 添加子节点：$1 是 term，表示左侧项（例如 x）
        add_child(node, (yyvsp[(3) - (3)].node));  // 添加子节点：$3 是 factor，表示右侧因子（例如 2）
        (yyval.node) = node;  // 设置当前规则的返回值 $$ 为创建的节点
        // 注释：计算逻辑已移除，value 仅用于标识操作类型，后续可以在语义分析或代码生成阶段处理
    }
    break;

  case 63:

/* Line 1455 of yacc.c  */
#line 1294 "parser.y"
    {
        // 构建除法项（实数除法）
        Node *node = create_node("term", "divide");  // 创建 term 节点，value 设为 "divide"，表示这是一个实数除法操作
        add_child(node, (yyvsp[(1) - (3)].node));  // 添加子节点：$1 是 term，表示左侧项
        add_child(node, (yyvsp[(3) - (3)].node));  // 添加子节点：$3 是 factor，表示右侧因子
        (yyval.node) = node;  // 设置当前规则的返回值 $$ 为创建的节点
    }
    break;

  case 64:

/* Line 1455 of yacc.c  */
#line 1302 "parser.y"
    {
        // 构建整除项
        Node *node = create_node("term", "div");  // 创建 term 节点，value 设为 "div"，表示这是一个整除操作
        add_child(node, (yyvsp[(1) - (3)].node));  // 添加子节点：$1 是 term，表示左侧项
        add_child(node, (yyvsp[(3) - (3)].node));  // 添加子节点：$3 是 factor，表示右侧因子
        (yyval.node) = node;  // 设置当前规则的返回值 $$ 为创建的节点
    }
    break;

  case 65:

/* Line 1455 of yacc.c  */
#line 1310 "parser.y"
    {
        // 构建取模项
        Node *node = create_node("term", "mod");  // 创建 term 节点，value 设为 "mod"，表示这是一个取模操作
        add_child(node, (yyvsp[(1) - (3)].node));  // 添加子节点：$1 是 term，表示左侧项
        add_child(node, (yyvsp[(3) - (3)].node));  // 添加子节点：$3 是 factor，表示右侧因子
        (yyval.node) = node;  // 设置当前规则的返回值 $$ 为创建的节点
    }
    break;

  case 66:

/* Line 1455 of yacc.c  */
#line 1318 "parser.y"
    {
        // 构建逻辑与项
        Node *node = create_node("term", "and");  // 创建 term 节点，value 设为 "and"，表示这是一个逻辑与操作
        add_child(node, (yyvsp[(1) - (3)].node));  // 添加子节点：$1 是 term，表示左侧项
        add_child(node, (yyvsp[(3) - (3)].node));  // 添加子节点：$3 是 factor，表示右侧因子
        (yyval.node) = node;  // 设置当前规则的返回值 $$ 为创建的节点
    }
    break;

  case 67:

/* Line 1455 of yacc.c  */
#line 1337 "parser.y"
    {
        // 构建单个项的简单表达式
        (yyval.node) = create_node("simple_expression", NULL);  // 创建一个类型为 "simple_expression" 的语法树节点，value 初始设为 NULL（simple_expression 本身没有值，仅作为简单表达式的容器）
        add_child((yyval.node), (yyvsp[(1) - (1)].node));  // 添加子节点：$1 是 term，表示单个项（例如 42 或 x * 2）

        // 从 term 继承 value
        (yyval.node)->value = (yyvsp[(1) - (1)].node)->value ? strdup((yyvsp[(1) - (1)].node)->value) : NULL;  // 如果 term 有值（例如 "42" 或 "unknown"），则复制到 simple_expression 的 value 字段；否则设为 NULL
        // 注意：strdup 用于内存安全，确保 value 字段的字符串是独立的副本
    }
    break;

  case 68:

/* Line 1455 of yacc.c  */
#line 1347 "parser.y"
    {
        // 构建加法简单表达式
        Node *node = create_node("simple_expression", "plus");  // 创建 simple_expression 节点，value 设为 "plus"，表示这是一个加法操作
        add_child(node, (yyvsp[(1) - (3)].node));  // 添加子节点：$1 是 simple_expression，表示左侧简单表达式（例如 x）
        add_child(node, (yyvsp[(3) - (3)].node));  // 添加子节点：$3 是 term，表示右侧项（例如 2）
        (yyval.node) = node;  // 设置当前规则的返回值 $$ 为创建的节点
        // 注释：计算逻辑已移除，value 仅用于标识操作类型，后续可以在语义分析或代码生成阶段处理
    }
    break;

  case 69:

/* Line 1455 of yacc.c  */
#line 1356 "parser.y"
    {
        // 构建减法简单表达式
        Node *node = create_node("simple_expression", "minus");  // 创建 simple_expression 节点，value 设为 "minus"，表示这是一个减法操作
        add_child(node, (yyvsp[(1) - (3)].node));  // 添加子节点：$1 是 simple_expression，表示左侧简单表达式
        add_child(node, (yyvsp[(3) - (3)].node));  // 添加子节点：$3 是 term，表示右侧项
        (yyval.node) = node;  // 设置当前规则的返回值 $$ 为创建的节点
    }
    break;

  case 70:

/* Line 1455 of yacc.c  */
#line 1364 "parser.y"
    {
        // 构建逻辑或简单表达式
        Node *node = create_node("simple_expression", "or");  // 创建 simple_expression 节点，value 设为 "or"，表示这是一个逻辑或操作
        add_child(node, (yyvsp[(1) - (3)].node));  // 添加子节点：$1 是 simple_expression，表示左侧简单表达式
        add_child(node, (yyvsp[(3) - (3)].node));  // 添加子节点：$3 是 term，表示右侧项
        (yyval.node) = node;  // 设置当前规则的返回值 $$ 为创建的节点
    }
    break;

  case 71:

/* Line 1455 of yacc.c  */
#line 1382 "parser.y"
    {
        // 构建单个简单表达式的表达式
        (yyval.node) = create_node("expression", NULL);  // 创建一个类型为 "expression" 的语法树节点，value 初始设为 NULL（expression 本身没有值，仅作为表达式的容器）
        add_child((yyval.node), (yyvsp[(1) - (1)].node));  // 添加子节点：$1 是 simple_expression，表示单个简单表达式（例如 x + 2）
    }
    break;

  case 72:

/* Line 1455 of yacc.c  */
#line 1388 "parser.y"
    {
        // 构建相等关系表达式
        Node *node = create_node("expression", "equal");  // 创建 expression 节点，value 设为 "equal"，表示这是一个相等关系运算
        add_child(node, (yyvsp[(1) - (3)].node));  // 添加子节点：$1 是 simple_expression，表示左侧简单表达式（例如 x + 2）
        add_child(node, (yyvsp[(3) - (3)].node));  // 添加子节点：$3 是 simple_expression，表示右侧简单表达式（例如 y）
        // 注意：EQUAL 是关系运算符（=），未添加到语法树中，因为它不携带语义信息（已通过 value="equal" 表示）
        (yyval.node) = node;  // 设置当前规则的返回值 $$ 为创建的节点
    }
    break;

  case 73:

/* Line 1455 of yacc.c  */
#line 1397 "parser.y"
    {
        // 构建不相等关系表达式
        Node *node = create_node("expression", "not_equal");  // 创建 expression 节点，value 设为 "not_equal"，表示这是一个不相等关系运算
        add_child(node, (yyvsp[(1) - (3)].node));  // 添加子节点：$1 是 simple_expression，表示左侧简单表达式
        add_child(node, (yyvsp[(3) - (3)].node));  // 添加子节点：$3 是 simple_expression，表示右侧简单表达式
        (yyval.node) = node;  // 设置当前规则的返回值 $$ 为创建的节点
    }
    break;

  case 74:

/* Line 1455 of yacc.c  */
#line 1405 "parser.y"
    {
        // 构建小于关系表达式
        Node *node = create_node("expression", "less");  // 创建 expression 节点，value 设为 "less"，表示这是一个小于关系运算
        add_child(node, (yyvsp[(1) - (3)].node));  // 添加子节点：$1 是 simple_expression，表示左侧简单表达式
        add_child(node, (yyvsp[(3) - (3)].node));  // 添加子节点：$3 是 simple_expression，表示右侧简单表达式
        (yyval.node) = node;  // 设置当前规则的返回值 $$ 为创建的节点
    }
    break;

  case 75:

/* Line 1455 of yacc.c  */
#line 1413 "parser.y"
    {
        // 构建小于等于关系表达式
        Node *node = create_node("expression", "less_equal");  // 创建 expression 节点，value 设为 "less_equal"，表示这是一个小于等于关系运算
        add_child(node, (yyvsp[(1) - (3)].node));  // 添加子节点：$1 是 simple_expression，表示左侧简单表达式
        add_child(node, (yyvsp[(3) - (3)].node));  // 添加子节点：$3 是 simple_expression，表示右侧简单表达式
        (yyval.node) = node;  // 设置当前规则的返回值 $$ 为创建的节点
    }
    break;

  case 76:

/* Line 1455 of yacc.c  */
#line 1421 "parser.y"
    {
        // 构建大于关系表达式
        Node *node = create_node("expression", "greater");  // 创建 expression 节点，value 设为 "greater"，表示这是一个大于关系运算
        add_child(node, (yyvsp[(1) - (3)].node));  // 添加子节点：$1 是 simple_expression，表示左侧简单表达式
        add_child(node, (yyvsp[(3) - (3)].node));  // 添加子节点：$3 是 simple_expression，表示右侧简单表达式
        (yyval.node) = node;  // 设置当前规则的返回值 $$ 为创建的节点
    }
    break;

  case 77:

/* Line 1455 of yacc.c  */
#line 1429 "parser.y"
    {
        // 构建大于等于关系表达式
        Node *node = create_node("expression", "greater_equal");  // 创建 expression 节点，value 设为 "greater_equal"，表示这是一个大于等于关系运算
        add_child(node, (yyvsp[(1) - (3)].node));  // 添加子节点：$1 是 simple_expression，表示左侧简单表达式
        add_child(node, (yyvsp[(3) - (3)].node));  // 添加子节点：$3 是 simple_expression，表示右侧简单表达式
        (yyval.node) = node;  // 设置当前规则的返回值 $$ 为创建的节点
    }
    break;

  case 78:

/* Line 1455 of yacc.c  */
#line 1446 "parser.y"
    {
        // 构建单个表达式的表达式列表
        (yyval.node) = create_node("expression_list", NULL);  // 创建一个类型为 "expression_list" 的语法树节点，value 设为 NULL（expression_list 本身没有值，仅作为表达式列表的容器）
        add_child((yyval.node), (yyvsp[(1) - (1)].node));  // 添加子节点：$1 是 expression，表示单个表达式（例如 x + 1）
    }
    break;

  case 79:

/* Line 1455 of yacc.c  */
#line 1452 "parser.y"
    {
        // 扩展现有的表达式列表（递归定义）
        (yyval.node) = (yyvsp[(1) - (3)].node);  // 复用 $1（现有的 expression_list 节点），避免创建新节点以提高效率
        add_child((yyval.node), (yyvsp[(3) - (3)].node));  // 添加子节点：$3 是 expression，表示新的表达式（例如 y * 2）
        // 注意：COMMA 是分隔符（,），未添加到语法树中，因为它不携带语义信息
    }
    break;

  case 80:

/* Line 1455 of yacc.c  */
#line 1468 "parser.y"
    {
        // 构建带参数的函数调用
        Node *node = create_node("function_call", NULL);  // 创建一个类型为 "function_call" 的语法树节点，value 设为 NULL（function_call 本身没有值，仅作为函数调用的容器）
        add_child(node, create_node("id", (yyvsp[(1) - (4)].str)));  // 添加子节点：类型为 "id"，value 设为函数名（$1 是 IDENTIFIER，例如 "func"）
        add_child(node, (yyvsp[(3) - (4)].node));  // 添加子节点：$3 是 expression_list，表示参数列表（例如 x + 1, y * 2）
        // 注意：LPAREN 和 RPAREN 是括号（( 和 )），未添加到语法树中
        (yyval.node) = node;  // 设置当前规则的返回值 $$ 为创建的节点
    }
    break;

  case 81:

/* Line 1455 of yacc.c  */
#line 1477 "parser.y"
    {
        // 构建无参数的函数调用
        Node *node = create_node("function_call", NULL);  // 创建 function_call 节点
        add_child(node, create_node("id", (yyvsp[(1) - (3)].str)));  // 添加子节点：类型为 "id"，value 设为函数名（例如 "func"）
        add_child(node, create_node("expression_list", "empty"));  // 添加子节点：expression_list 设为 "empty"，表示无参数
        (yyval.node) = node;  // 设置当前规则的返回值 $$ 为创建的节点
    }
    break;

  case 82:

/* Line 1455 of yacc.c  */
#line 1494 "parser.y"
    {
        // 构建单个变量的变量列表
        Node *node = create_node("variable_list", NULL);  // 创建一个类型为 "variable_list" 的语法树节点，value 设为 NULL（variable_list 本身没有值，仅作为变量列表的容器）
        add_child(node, (yyvsp[(1) - (1)].node));  // 添加子节点：$1 是 variable，表示单个变量（例如 x）
        (yyval.node) = node;  // 设置当前规则的返回值 $$ 为创建的节点
    }
    break;

  case 83:

/* Line 1455 of yacc.c  */
#line 1501 "parser.y"
    {
        // 构建多个变量的变量列表（递归定义）
        Node *node = create_node("variable_list", NULL);  // 创建 variable_list 节点
        add_child(node, (yyvsp[(1) - (3)].node));  // 添加子节点：$1 是 variable_list，表示之前的变量列表（例如 x）
        add_child(node, (yyvsp[(3) - (3)].node));  // 添加子节点：$3 是 variable，表示新的变量（例如 y[1]）
        // 注意：COMMA 是分隔符（,），未添加到语法树中
        (yyval.node) = node;  // 设置当前规则的返回值 $$ 为创建的节点
    }
    break;

  case 84:

/* Line 1455 of yacc.c  */
#line 1519 "parser.y"
    {
        // 处理空的 else 部分
        (yyval.node) = create_node("else_part", "empty");  // 创建一个类型为 "else_part" 的语法树节点，value 设为 "empty"，表示没有 else 分支
    }
    break;

  case 85:

/* Line 1455 of yacc.c  */
#line 1524 "parser.y"
    {
        // 构建 else 分支
        Node *node = create_node("else_part", NULL);  // 创建 else_part 节点，value 设为 NULL（else_part 本身没有值，仅作为 else 分支的容器）
        add_child(node, (yyvsp[(2) - (2)].node));  // 添加子节点：$2 是 statement，表示 else 分支的语句（例如 y := 0）
        // 注意：ELSE 关键字未添加到语法树中
        (yyval.node) = node;  // 设置当前规则的返回值 $$ 为创建的节点
    }
    break;

  case 86:

/* Line 1455 of yacc.c  */
#line 1546 "parser.y"
    {
        // 打印调试信息
        fprintf(stderr, "Processing assignment: IDENTIFIER = %s\n", (yyvsp[(1) - (4)].str) ? (yyvsp[(1) - (4)].str) : "NULL");  // 输出正在处理的赋值语句的变量名

        // 检查符号表是否有效
        if (!current_table) {  // 检查全局符号表 current_table 是否为 NULL
            fprintf(stderr, "Error: current_table is NULL in statement\n");  // 如果为 NULL，打印错误信息
            exit(1);  // 退出程序，表示符号表未正确初始化
        }

        // 查找变量符号
        Symbol *symbol = lookup_symbol((yyvsp[(1) - (4)].str));  // 在符号表中查找变量名（$1 是 IDENTIFIER，例如 "x"）
        fprintf(stderr, "lookup_symbol returned: %p\n", (void *)symbol);  // 打印查找结果的指针地址（用于调试）
        if (!symbol) {  // 如果变量未定义
            fprintf(stderr, "Symbol not found: %s\n", (yyvsp[(1) - (4)].str));  // 打印调试信息
            fprintf(output_file, "Error: Undefined identifier '%s' at line %d\n", (yyvsp[(1) - (4)].str), line_number);  // 输出错误信息到文件
        } else {  // 如果变量存在
            fprintf(stderr, "Symbol found: %s, kind: %s\n", (yyvsp[(1) - (4)].str), symbol->kind ? symbol->kind : "NULL");  // 打印变量信息
            if (strcmp(symbol->kind, "var") != 0) {  // 检查符号是否为变量（kind 应为 "var"）
                fprintf(output_file, "Error: '%s' is not a variable at line %d\n", (yyvsp[(1) - (4)].str), line_number);  // 如果不是变量，输出错误
            } else {  // 如果是变量
                // 检查右侧表达式的变量（如果存在）
                if ((yyvsp[(4) - (4)].node)->child_count > 0 && strcmp((yyvsp[(4) - (4)].node)->children[0]->type, "simple_expression") == 0 &&
                    (yyvsp[(4) - (4)].node)->children[0]->child_count > 0 && strcmp((yyvsp[(4) - (4)].node)->children[0]->children[0]->type, "term") == 0 &&
                    (yyvsp[(4) - (4)].node)->children[0]->children[0]->child_count > 0 && strcmp((yyvsp[(4) - (4)].node)->children[0]->children[0]->children[0]->type, "factor") == 0) {
                    Node *factor = (yyvsp[(4) - (4)].node)->children[0]->children[0]->children[0];  // 获取右侧表达式的第一个因子（factor）
                    if (strcmp(factor->type, "variable") == 0) {  // 如果因子是一个变量
                        Symbol *rhs_symbol = lookup_symbol(factor->children[0]->children[0]->value);  // 查找右侧变量
                        if (!rhs_symbol) {  // 如果右侧变量未定义
                            fprintf(output_file, "Error: Undefined identifier '%s' in expression at line %d\n", factor->children[0]->children[0]->value, line_number);
                        }
                    }
                }

                // 更新符号表中的变量值
                if (symbol->value) free(symbol->value);  // 释放符号表中变量的旧值（避免内存泄漏）
                if ((yyvsp[(4) - (4)].node)->child_count > 0 && strcmp((yyvsp[(4) - (4)].node)->children[0]->type, "simple_expression") == 0 &&
                    (yyvsp[(4) - (4)].node)->children[0]->child_count > 0 && strcmp((yyvsp[(4) - (4)].node)->children[0]->children[0]->type, "term") == 0 &&
                    (yyvsp[(4) - (4)].node)->children[0]->children[0]->child_count > 0 && strcmp((yyvsp[(4) - (4)].node)->children[0]->children[0]->children[0]->type, "factor") == 0) {
                    Node *factor = (yyvsp[(4) - (4)].node)->children[0]->children[0]->children[0];  // 获取右侧表达式的第一个因子
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
        add_child(var_node, create_node("id", (yyvsp[(1) - (4)].str)));  // 添加变量名（例如 "x"）
        add_child(var_node, (yyvsp[(2) - (4)].node));  // 添加 id_varpart（例如数组索引或 empty）
        add_child(node, var_node);  // 将 variable 节点添加到 statement 节点
        add_child(node, (yyvsp[(4) - (4)].node));  // 添加 expression 节点（右侧表达式，例如 1）
        (yyval.node) = node;  // 设置当前规则的返回值 $$ 为创建的节点
    }
    break;

  case 87:

/* Line 1455 of yacc.c  */
#line 1634 "parser.y"
    {
        // 构建过程调用语句
        Node *node = create_node("statement", "procedure_call");  // 创建 statement 节点，value 设为 "procedure_call"
        add_child(node, (yyvsp[(1) - (1)].node));  // 添加子节点：$1 是 function_call，表示过程调用（例如 proc(x, y)）

        // 检查过程是否定义
        Symbol *symbol = lookup_symbol((yyvsp[(1) - (1)].node)->children[0]->value);  // 查找过程名（function_call 的 id 节点）
        if (!symbol) {  // 如果未定义
            fprintf(output_file, "Error: Undefined identifier '%s' at line %d\n", (yyvsp[(1) - (1)].node)->children[0]->value, line_number);
        }

        (yyval.node) = node;  // 设置当前规则的返回值 $$ 为创建的节点
    }
    break;

  case 88:

/* Line 1455 of yacc.c  */
#line 1648 "parser.y"
    {
        // 构建复合语句
        Node *node = create_node("statement", "compound");  // 创建 statement 节点，value 设为 "compound"
        add_child(node, (yyvsp[(1) - (1)].node));  // 添加子节点：$1 是 compound_statement（例如 begin ... end）
        (yyval.node) = node;  // 设置当前规则的返回值 $$ 为创建的节点
    }
    break;

  case 89:

/* Line 1455 of yacc.c  */
#line 1655 "parser.y"
    {
        // 构建 if 语句
        Node *node = create_node("statement", "if");  // 创建 statement 节点，value 设为 "if"
        add_child(node, (yyvsp[(2) - (5)].node));  // 添加子节点：$2 是 expression，表示条件（例如 x > 0）
        add_child(node, (yyvsp[(4) - (5)].node));  // 添加子节点：$4 是 statement，表示 then 分支（例如 y := 1）
        add_child(node, (yyvsp[(5) - (5)].node));  // 添加子节点：$5 是 else_part，表示 else 分支（可以是 empty）
        (yyval.node) = node;  // 设置当前规则的返回值 $$ 为创建的节点
    }
    break;

  case 90:

/* Line 1455 of yacc.c  */
#line 1664 "parser.y"
    {
        // 检查循环变量
        Symbol *symbol = lookup_symbol((yyvsp[(2) - (8)].str));  // 查找循环变量（$2 是 IDENTIFIER，例如 "i"）
        if (!symbol) {  // 如果未定义
            fprintf(output_file, "Error: Undefined identifier '%s' at line %d\n", (yyvsp[(2) - (8)].str), line_number);
        } else if (strcmp(symbol->kind, "var") != 0) {  // 如果不是变量
            fprintf(output_file, "Error: '%s' is not a variable at line %d\n", (yyvsp[(2) - (8)].str), line_number);
        } else {  // 如果是变量
            // 更新循环变量的值
            if (symbol->value) free(symbol->value);  // 释放旧值
            if ((yyvsp[(4) - (8)].node)->child_count > 0 && strcmp((yyvsp[(4) - (8)].node)->children[0]->type, "simple_expression") == 0 &&
                (yyvsp[(4) - (8)].node)->children[0]->child_count > 0 && strcmp((yyvsp[(4) - (8)].node)->children[0]->children[0]->type, "term") == 0 &&
                (yyvsp[(4) - (8)].node)->children[0]->children[0]->child_count > 0 && strcmp((yyvsp[(4) - (8)].node)->children[0]->children[0]->children[0]->type, "factor") == 0) {
                Node *factor = (yyvsp[(4) - (8)].node)->children[0]->children[0]->children[0];  // 获取初始表达式（$4）的第一个因子
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
        add_child(node, create_node("id", (yyvsp[(2) - (8)].str)));  // 添加循环变量（例如 "i"）
        add_child(node, (yyvsp[(4) - (8)].node));  // 添加初始表达式（例如 1）
        add_child(node, (yyvsp[(6) - (8)].node));  // 添加终止表达式（例如 10）
        add_child(node, (yyvsp[(8) - (8)].node));  // 添加循环体语句（例如 x := x + i）
        (yyval.node) = node;  // 设置当前规则的返回值 $$ 为创建的节点
    }
    break;

  case 91:

/* Line 1455 of yacc.c  */
#line 1710 "parser.y"
    {
        // 构建读语句
        Node *node = create_node("statement", "read");  // 创建 statement 节点，value 设为 "read"
        add_child(node, (yyvsp[(3) - (4)].node));  // 添加子节点：$3 是 variable_list，表示变量列表（例如 x, y）
        (yyval.node) = node;  // 设置当前规则的返回值 $$ 为创建的节点
    }
    break;

  case 92:

/* Line 1455 of yacc.c  */
#line 1717 "parser.y"
    {
        // 构建写语句
        Node *node = create_node("statement", "write");  // 创建 statement 节点，value 设为 "write"
        add_child(node, (yyvsp[(3) - (4)].node));  // 添加子节点：$3 是 expression_list，表示表达式列表（例如 x + 1, y）
        (yyval.node) = node;  // 设置当前规则的返回值 $$ 为创建的节点
    }
    break;



/* Line 1455 of yacc.c  */
#line 3428 "y.tab.c"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (yymsg);
	  }
	else
	  {
	    yyerror (YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  *++yyvsp = yylval;


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined(yyoverflow) || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}



/* Line 1675 of yacc.c  */
#line 1724 "parser.y"




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


