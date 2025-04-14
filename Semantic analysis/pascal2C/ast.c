#include "ast.h"
#include <stdarg.h>

SymbolTable *current_table = NULL;
FILE *symbol_table_file = NULL;
FILE *annotated_ast_file = NULL;

Node* create_node(const char *type, const char *value, int child_count, ...) {
    Node *node = (Node*)malloc(sizeof(Node));
    node->type = type ? strdup(type) : NULL;
    node->value = value ? strdup(value) : NULL;
    node->child_count = child_count;
    node->children = child_count > 0 ? (Node**)malloc(sizeof(Node*) * child_count) : NULL;

    va_list args;
    va_start(args, child_count);
    for (int i = 0; i < child_count; i++) {
        node->children[i] = va_arg(args, Node*);
    }
    va_end(args);
    return node;
}

void free_node(Node *node) {
    if (!node) return;
    if (node->type) free(node->type);
    if (node->value) free(node->value);
    for (int i = 0; i < node->child_count; i++) {
        free_node(node->children[i]);
    }
    if (node->children) free(node->children);
    free(node);
}

SymbolTable* create_symbol_table(SymbolTable *parent) {
    SymbolTable *table = (SymbolTable*)malloc(sizeof(SymbolTable));
    table->parent = parent;
    table->symbols = NULL;
    table->scope_level = parent ? parent->scope_level + 1 : 0;
    return table;
}

void insert_symbol(SymbolTable *table, const char *name, const char *kind, const char *type, const char *value) {
    Symbol *sym = (Symbol*)malloc(sizeof(Symbol));
    sym->name = strdup(name);
    sym->kind = kind ? strdup(kind) : NULL;
    sym->type = type ? strdup(type) : NULL;
    sym->value = value ? strdup(value) : NULL;
    sym->next = table->symbols;
    table->symbols = sym;
}

Symbol* lookup_symbol(SymbolTable *table, const char *name) {
    SymbolTable *scope = table;
    while (scope) {
        Symbol *sym = scope->symbols;
        while (sym) {
            if (strcmp(sym->name, name) == 0) return sym;
            sym = sym->next;
        }
        scope = scope->parent;
    }
    return NULL;
}

void free_symbol_table(SymbolTable *table) {
    Symbol *sym = table->symbols;
    while (sym) {
        Symbol *next = sym->next;
        free(sym->name);
        if (sym->kind) free(sym->kind);
        if (sym->type) free(sym->type);
        if (sym->value) free(sym->value);
        free(sym);
        sym = next;
    }
    free(table);
}

void enter_scope() {
    current_table = create_symbol_table(current_table);
}

void exit_scope() {
    SymbolTable *old = current_table;
    current_table = current_table->parent;
    free_symbol_table(old);
}

const char* infer_expr_type(Node *expr) {
    if (!expr) return "unknown";
    if (strcmp(expr->type, "IDENTIFIER") == 0) {
        Symbol *sym = lookup_symbol(current_table, expr->value);
        return sym ? sym->type : "undeclared";
    }
    if (strcmp(expr->type, "INTEGER_CONST") == 0) return "integer";
    if (strcmp(expr->type, "REAL_CONST") == 0) return "real";
    if (strcmp(expr->type, "CHAR_CONST") == 0) return "char";
    if (strcmp(expr->type, "BOOLEAN_CONST") == 0) return "boolean";
    if (strcmp(expr->type, "PLUS") == 0 || strcmp(expr->type, "MINUS") == 0 ||
        strcmp(expr->type, "MULTIPLY") == 0 || strcmp(expr->type, "DIVIDE") == 0) {
        const char *left_type = infer_expr_type(expr->children[0]);
        const char *right_type = infer_expr_type(expr->children[1]);
        if (strcmp(left_type, "undeclared") == 0 || strcmp(right_type, "undeclared") == 0) {
            return "undeclared";
        }
        if (strcmp(left_type, "real") == 0 || strcmp(right_type, "real") == 0) {
            return "real";
        }
        return "integer";
    }
    if (strcmp(expr->type, "DIV") == 0 || strcmp(expr->type, "MOD") == 0) {
        const char *left_type = infer_expr_type(expr->children[0]);
        const char *right_type = infer_expr_type(expr->children[1]);
        if (strcmp(left_type, "undeclared") == 0 || strcmp(right_type, "undeclared") == 0) {
            return "undeclared";
        }
        if (strcmp(left_type, "integer") != 0 || strcmp(right_type, "integer") != 0) {
            fprintf(symbol_table_file, "Error: '%s' requires integer operands, got %s and %s (Scope: %d)\n",
                    expr->type, left_type, right_type, current_table->scope_level);
            return "error";
        }
        return "integer";
    }
    if (strcmp(expr->type, "AND") == 0 || strcmp(expr->type, "OR") == 0) {
        const char *left_type = infer_expr_type(expr->children[0]);
        const char *right_type = infer_expr_type(expr->children[1]);
        if (strcmp(left_type, "boolean") != 0 || strcmp(right_type, "boolean") != 0) {
            fprintf(symbol_table_file, "Error: '%s' requires boolean operands, got %s and %s (Scope: %d)\n",
                    expr->type, left_type, right_type, current_table->scope_level);
            return "error";
        }
        return "boolean";
    }
    if (strcmp(expr->type, "NOT") == 0) {
        const char *operand_type = infer_expr_type(expr->children[0]);
        if (strcmp(operand_type, "boolean") != 0) {
            fprintf(symbol_table_file, "Error: 'NOT' requires boolean operand, got %s (Scope: %d)\n",
                    operand_type, current_table->scope_level);
            return "error";
        }
        return "boolean";
    }
    if (strcmp(expr->type, "GREATER") == 0 || strcmp(expr->type, "LESS") == 0 ||
        strcmp(expr->type, "GREATER_EQUAL") == 0 || strcmp(expr->type, "LESS_EQUAL") == 0 ||
        strcmp(expr->type, "EQUAL") == 0 || strcmp(expr->type, "NOT_EQUAL") == 0) {
        const char *left_type = infer_expr_type(expr->children[0]);
        const char *right_type = infer_expr_type(expr->children[1]);
        if (strcmp(left_type, "undeclared") == 0 || strcmp(right_type, "undeclared") == 0) {
            return "undeclared";
        }
        if (strcmp(left_type, right_type) != 0) {
            fprintf(symbol_table_file, "Warning: Type mismatch in '%s': %s vs %s (Scope: %d)\n",
                    expr->type, left_type, right_type, current_table->scope_level);
        }
        return "boolean";
    }
    if (strcmp(expr->type, "UMINUS") == 0) {
        const char *operand_type = infer_expr_type(expr->children[0]);
        if (strcmp(operand_type, "integer") != 0 && strcmp(operand_type, "real") != 0) {
            fprintf(symbol_table_file, "Error: 'UMINUS' requires numeric operand, got %s (Scope: %d)\n",
                    operand_type, current_table->scope_level);
            return "error";
        }
        return operand_type;
    }
    return "unknown";
}

void check_symbol_table(Node *root) {
    if (!root) return;

    if (strcmp(root->type, "PROGRAM") == 0) {
        insert_symbol(current_table, root->value, "program", NULL, NULL);
        fprintf(symbol_table_file, "Program: %s (Scope: %d)\n", root->value, current_table->scope_level);
        for (int i = 0; i < root->child_count; i++) {
            check_symbol_table(root->children[i]);
        }
    }
    else if (strcmp(root->type, "VAR") == 0) {
        // 处理所有变量（除了最后一个子节点，它是类型）
        const char *var_type = root->children[root->child_count - 1]->value;
        for (int i = 0; i < root->child_count - 1; i++) {
            const char *var_name = root->children[i]->value;
            if (lookup_symbol(current_table, var_name)) {
                fprintf(symbol_table_file, "Error: Redeclaration of '%s' in scope %d\n", var_name, current_table->scope_level);
            } else {
                insert_symbol(current_table, var_name, "var", var_type, NULL);
                fprintf(symbol_table_file, "Declared '%s' as %s (Scope: %d)\n", var_name, var_type, current_table->scope_level);
            }
        }
    }
    else if (strcmp(root->type, "ASSIGN") == 0) {
        const char *var_name = root->children[0]->value;
        Symbol *sym = lookup_symbol(current_table, var_name);
        if (!sym) {
            fprintf(symbol_table_file, "Error: Undeclared variable '%s' in scope %d\n", var_name, current_table->scope_level);
        } else {
            const char *expr_type = infer_expr_type(root->children[1]);
            if (strcmp(sym->type, expr_type) != 0) {
                fprintf(symbol_table_file, "경고: '%s'에 대한 할당에서 타입 불일치: 예상 %s, 실제 %s (Scope: %d)\n",
                        var_name, sym->type, expr_type, current_table->scope_level);
            }
        }
        check_symbol_table(root->children[1]);
    }
    else if (strcmp(root->type, "IF") == 0) {
        const char *cond_type = infer_expr_type(root->children[0]);
        if (strcmp(cond_type, "boolean") != 0) {
            fprintf(symbol_table_file, "Error: If condition must be boolean, got %s (Scope: %d)\n",
                    cond_type, current_table->scope_level);
        }
        check_symbol_table(root->children[0]);
        check_symbol_table(root->children[1]);
        if (root->child_count > 2) check_symbol_table(root->children[2]);
    }
    else if (strcmp(root->type, "WRITE") == 0) {
        const char *arg_type = infer_expr_type(root->children[0]);
        if (strcmp(arg_type, "char") != 0 && strcmp(arg_type, "integer") != 0 && strcmp(arg_type, "real") != 0) {
            fprintf(symbol_table_file, "Warning: 'WRITE' expects char, integer, or real, got %s (Scope: %d)\n",
                    arg_type, current_table->scope_level);
        }
        check_symbol_table(root->children[0]);
    }
    else {
        for (int i = 0; i < root->child_count; i++) {
            check_symbol_table(root->children[i]);
        }
    }
}

void print_annotated_ast(Node *node, int indent) {
    if (!node) return;

    for (int i = 0; i < indent; i++) fprintf(annotated_ast_file, "    ");

    if (strcmp(node->type, "PROGRAM") == 0) {
        fprintf(annotated_ast_file, "{\n");
        for (int i = 0; i < indent + 1; i++) fprintf(annotated_ast_file, "    ");
        fprintf(annotated_ast_file, "\"type\": \"Program\",\n");
        for (int i = 0; i < indent + 1; i++) fprintf(annotated_ast_file, "    ");
        fprintf(annotated_ast_file, "\"name\": \"%s\",\n", node->value);
        for (int i = 0; i < indent + 1; i++) fprintf(annotated_ast_file, "    ");
        fprintf(annotated_ast_file, "\"declarations\": [\n");
        print_annotated_ast(node->children[0], indent + 2);
        for (int i = 0; i < indent + 1; i++) fprintf(annotated_ast_file, "    ");
        fprintf(annotated_ast_file, "],\n");
        for (int i = 0; i < indent + 1; i++) fprintf(annotated_ast_file, "    ");
        fprintf(annotated_ast_file, "\"body\": ");
        print_annotated_ast(node->children[1], indent + 1);
        for (int i = 0; i < indent; i++) fprintf(annotated_ast_file, "    ");
        fprintf(annotated_ast_file, "}\n");
    }
    else if (strcmp(node->type, "VAR") == 0) {
        fprintf(annotated_ast_file, "{\n");
        for (int i = 0; i < indent + 1; i++) fprintf(annotated_ast_file, "    ");
        fprintf(annotated_ast_file, "\"type\": \"VarDeclaration\",\n");
        for (int i = 0; i < indent + 1; i++) fprintf(annotated_ast_file, "    ");
        fprintf(annotated_ast_file, "\"names\": [");
        for (int i = 0; i < node->child_count - 1; i++) {
            fprintf(annotated_ast_file, "\"%s\"", node->children[i]->value);
            if (i < node->child_count - 2) fprintf(annotated_ast_file, ", ");
        }
        fprintf(annotated_ast_file, "],\n");
        for (int i = 0; i < indent + 1; i++) fprintf(annotated_ast_file, "    ");
        fprintf(annotated_ast_file, "\"var_type\": \"%s\",\n", node->children[node->child_count - 1]->value);
        for (int i = 0; i < indent + 1; i++) fprintf(annotated_ast_file, "    ");
        fprintf(annotated_ast_file, "\"scope\": \"%d\"\n", current_table->scope_level);
        for (int i = 0; i < indent; i++) fprintf(annotated_ast_file, "    ");
        fprintf(annotated_ast_file, "}");
    }
    else if (strcmp(node->type, "TOKEN_BEGIN") == 0) {
        fprintf(annotated_ast_file, "{\n");
        for (int i = 0; i < indent + 1; i++) fprintf(annotated_ast_file, "    ");
        fprintf(annotated_ast_file, "\"type\": \"Block\",\n");
        for (int i = 0; i < indent + 1; i++) fprintf(annotated_ast_file, "    ");
        fprintf(annotated_ast_file, "\"statements\": [\n");
        for (int i = 0; i < node->child_count; i++) {
            print_annotated_ast(node->children[i], indent + 2);
            if (i < node->child_count - 1) fprintf(annotated_ast_file, ",\n");
        }
        fprintf(annotated_ast_file, "\n");
        for (int i = 0; i < indent + 1; i++) fprintf(annotated_ast_file, "    ");
        fprintf(annotated_ast_file, "]\n");
        for (int i = 0; i < indent; i++) fprintf(annotated_ast_file, "    ");
        fprintf(annotated_ast_file, "}");
    }
    else if (strcmp(node->type, "ASSIGN") == 0) {
        fprintf(annotated_ast_file, "{\n");
        for (int i = 0; i < indent + 1; i++) fprintf(annotated_ast_file, "    ");
        fprintf(annotated_ast_file, "\"type\": \"Assignment\",\n");
        for (int i = 0; i < indent + 1; i++) fprintf(annotated_ast_file, "    ");
        fprintf(annotated_ast_file, "\"left\": ");
        print_annotated_ast(node->children[0], indent + 1);
        fprintf(annotated_ast_file, ",\n");
        for (int i = 0; i < indent + 1; i++) fprintf(annotated_ast_file, "    ");
        fprintf(annotated_ast_file, "\"right\": ");
        print_annotated_ast(node->children[1], indent + 1);
        fprintf(annotated_ast_file, "\n");
        for (int i = 0; i < indent; i++) fprintf(annotated_ast_file, "    ");
        fprintf(annotated_ast_file, "}");
    }
    else if (strcmp(node->type, "IF") == 0) {
        fprintf(annotated_ast_file, "{\n");
        for (int i = 0; i < indent + 1; i++) fprintf(annotated_ast_file, "    ");
        fprintf(annotated_ast_file, "\"type\": \"IfStatement\",\n");
        for (int i = 0; i < indent + 1; i++) fprintf(annotated_ast_file, "    ");
        fprintf(annotated_ast_file, "\"condition\": ");
        print_annotated_ast(node->children[0], indent + 1);
        fprintf(annotated_ast_file, ",\n");
        for (int i = 0; i < indent + 1; i++) fprintf(annotated_ast_file, "    ");
        fprintf(annotated_ast_file, "\"then\": ");
        print_annotated_ast(node->children[1], indent + 1);
        if (node->child_count > 2) {
            fprintf(annotated_ast_file, ",\n");
            for (int i = 0; i < indent + 1; i++) fprintf(annotated_ast_file, "    ");
            fprintf(annotated_ast_file, "\"else\": ");
            print_annotated_ast(node->children[2], indent + 1);
        }
        fprintf(annotated_ast_file, "\n");
        for (int i = 0; i < indent; i++) fprintf(annotated_ast_file, "    ");
        fprintf(annotated_ast_file, "}");
    }
    else if (strcmp(node->type, "WRITE") == 0) {
        fprintf(annotated_ast_file, "{\n");
        for (int i = 0; i < indent + 1; i++) fprintf(annotated_ast_file, "    ");
        fprintf(annotated_ast_file, "\"type\": \"FunctionCall\",\n");
        for (int i = 0; i < indent + 1; i++) fprintf(annotated_ast_file, "    ");
        fprintf(annotated_ast_file, "\"name\": \"%s\",\n", node->value);
        for (int i = 0; i < indent + 1; i++) fprintf(annotated_ast_file, "    ");
        fprintf(annotated_ast_file, "\"args\": [\n");
        print_annotated_ast(node->children[0], indent + 2);
        fprintf(annotated_ast_file, "\n");
        for (int i = 0; i < indent + 1; i++) fprintf(annotated_ast_file, "    ");
        fprintf(annotated_ast_file, "],\n");
        for (int i = 0; i < indent + 1; i++) fprintf(annotated_ast_file, "    ");
        fprintf(annotated_ast_file, "\"return_type\": \"void\"\n");
        for (int i = 0; i < indent; i++) fprintf(annotated_ast_file, "    ");
        fprintf(annotated_ast_file, "}");
    }
    else if (strcmp(node->type, "IDENTIFIER") == 0) {
        Symbol *sym = lookup_symbol(current_table, node->value);
        fprintf(annotated_ast_file, "{\"type\": \"Identifier\", \"name\": \"%s\", \"var_type\": \"%s\"}",
                node->value, sym ? sym->type : "undeclared");
    }
    else if (strcmp(node->type, "INTEGER_CONST") == 0) {
        fprintf(annotated_ast_file, "{\"type\": \"IntegerLiteral\", \"value\": %s}", node->value);
    }
    else if (strcmp(node->type, "REAL_CONST") == 0) {
        fprintf(annotated_ast_file, "{\"type\": \"RealLiteral\", \"value\": %s}", node->value);
    }
    else if (strcmp(node->type, "CHAR_CONST") == 0) {
        fprintf(annotated_ast_file, "{\"type\": \"CharLiteral\", \"value\": \"%s\"}", node->value);
    }
    else if (strcmp(node->type, "PLUS") == 0 || strcmp(node->type, "GREATER") == 0 || strcmp(node->type, "LESS") == 0) {
        fprintf(annotated_ast_file, "{\n");
        for (int i = 0; i < indent + 1; i++) fprintf(annotated_ast_file, "    ");
        fprintf(annotated_ast_file, "\"type\": \"BinaryExpression\",\n");
        for (int i = 0; i < indent + 1; i++) fprintf(annotated_ast_file, "    ");
        fprintf(annotated_ast_file, "\"operator\": \"%s\",\n", node->value);
        for (int i = 0; i < indent + 1; i++) fprintf(annotated_ast_file, "    ");
        fprintf(annotated_ast_file, "\"left\": ");
        print_annotated_ast(node->children[0], indent + 1);
        fprintf(annotated_ast_file, ",\n");
        for (int i = 0; i < indent + 1; i++) fprintf(annotated_ast_file, "    ");
        fprintf(annotated_ast_file, "\"right\": ");
        print_annotated_ast(node->children[1], indent + 1);
        fprintf(annotated_ast_file, "\n");
        for (int i = 0; i < indent; i++) fprintf(annotated_ast_file, "    ");
        fprintf(annotated_ast_file, "}");
    }
}