#include "ast.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

SymbolTable* current_scope = NULL;

ASTNode* create_node(NodeType type, char* id, int sem_type) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = type;
    node->id = id ? strdup(id) : NULL;
    node->sem_type = sem_type;
    node->child = NULL;
    node->next = NULL;
    node->array_size = 0;
    node->op = NULL;
    node->left = NULL;
    node->right = NULL;
    node->index = -1;
    return node;
}

Symbol* lookup(SymbolTable* table, char* name) {
    Symbol* s = table->symbols;
    while (s != NULL) {
        if (strcmp(s->name, name) == 0) return s;
        s = s->next;
    }
    return NULL;
}

void insert(SymbolTable* table, char* name, int type, ASTNode* type_info) {
    Symbol* s = (Symbol*)malloc(sizeof(Symbol));
    s->name = strdup(name);
    s->type = type;
    s->type_info = type_info;
    s->next = table->symbols;
    table->symbols = s;
}

SymbolTable* create_scope() {
    SymbolTable* table = (SymbolTable*)malloc(sizeof(SymbolTable));
    table->symbols = NULL;
    table->parent = NULL;
    return table;
}

void free_scope(SymbolTable* table) {
    Symbol* s = table->symbols;
    while (s != NULL) {
        Symbol* next = s->next;
        free(s->name);
        if (s->type_info) free_ast(s->type_info);
        free(s);
        s = next;
    }
    free(table);
}

void free_ast(ASTNode* node) {
    if (!node) return;
    free_ast(node->child);
    free_ast(node->next);
    free_ast(node->left);
    free_ast(node->right);
    if (node->id) free(node->id);
    if (node->op) free(node->op);
    free(node);
}

void print_type(int type) {
    switch (type) {
        case INTEGER_TYPE: printf("integer"); break;
        case REAL_TYPE: printf("real"); break;
        case CHAR_TYPE: printf("char"); break;
        case BOOLEAN_TYPE: printf("boolean"); break;
        case ARRAY_TYPE: printf("array"); break;
        case RECORD_TYPE: printf("record"); break;
        default: printf("unknown"); break;
    }
}

void semantic_analysis(ASTNode* root) {
    if (!root) return;

    switch (root->type) {
        case NODE_PROGRAM:
            printf("Program: %s\n", root->id);
            break;
        case NODE_DECLARATION:
            if (lookup(current_scope, root->id) != NULL) {
                printf("Error: Redeclaration of '%s'\n", root->id);
            } else {
                insert(current_scope, root->id, root->sem_type, NULL);
                printf("Declared '%s' with type ", root->id);
                print_type(root->sem_type);
                printf("\n");
            }
            break;
        case NODE_ASSIGN:
            if (!lookup(current_scope, root->id)) {
                printf("Error: Undeclared variable '%s'\n", root->id);
            }
            break;
        case NODE_CALL:
            if (strcmp(root->id, "write") != 0 && !lookup(current_scope, root->id)) {
                printf("Error: Undeclared procedure '%s'\n", root->id);
            }
            break;
    }

    semantic_analysis(root->child);
    semantic_analysis(root->next);
}

void generate_c_code(ASTNode* root, FILE* out) {
    if (!root) return;

    switch (root->type) {
        case NODE_PROGRAM:
            fprintf(out, "// Generated C code from Pascal-S\n#include <stdio.h>\n");
            ASTNode* decl = root->child;
            while (decl && decl->type == NODE_DECLARATION) {
                generate_c_code(decl, out);
                decl = decl->next;
            }
            fprintf(out, "int main() {\n");
            ASTNode* stmt = root->child->next;
            while (stmt) {
                generate_c_code(stmt, out);
                stmt = stmt->next;
            }
            fprintf(out, "    return 0;\n}\n");
            break;
        case NODE_DECLARATION:
            switch (root->sem_type) {
                case INTEGER_TYPE: fprintf(out, "int %s;\n", root->id); break;
                case REAL_TYPE: fprintf(out, "double %s;\n", root->id); break;
                case CHAR_TYPE: fprintf(out, "char %s;\n", root->id); break;
                case BOOLEAN_TYPE: fprintf(out, "int %s;\n", root->id); break;
                case ARRAY_TYPE:
                    fprintf(out, "int %s[%d];\n", root->id, root->array_size);
                    break;
                case RECORD_TYPE:
                    fprintf(out, "struct %s {\n", root->id);
                    ASTNode* field = root->child;
                    while (field) {
                        switch (field->sem_type) {
                            case INTEGER_TYPE: fprintf(out, "    int %s;\n", field->id); break;
                            case REAL_TYPE: fprintf(out, "    double %s;\n", field->id); break;
                        }
                        field = field->next;
                    }
                    fprintf(out, "} %s;\n", root->id);
                    break;
            }
            break;
        case NODE_ASSIGN:
            fprintf(out, "    %s = ", root->id);
            generate_c_code(root->right, out);
            fprintf(out, ";\n");
            break;
        case NODE_IF:
            fprintf(out, "    if (");
            generate_c_code(root->child, out);
            fprintf(out, ") {\n");
            ASTNode* then_stmt = root->child->next;
            while (then_stmt) {
                generate_c_code(then_stmt, out);
                then_stmt = then_stmt->next;
            }
            fprintf(out, "    }\n");
            break;
        case NODE_EXPR:
            if (root->id) {
                fprintf(out, "%s", root->id);
            } else if (root->op) {
                generate_c_code(root->left, out);
                fprintf(out, " %s ", root->op);
                generate_c_code(root->right, out);
            }
            break;
        case NODE_CALL:
            if (strcmp(root->id, "write") == 0) {
                fprintf(out, "    printf(\"%%c\", ");
                generate_c_code(root->child, out);
                fprintf(out, ");\n");
            }
            break;
    }
}