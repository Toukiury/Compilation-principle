#include "ast.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Global variables
extern FILE *symbol_table_file;
extern FILE *annotated_ast_file;
extern SymbolTable *current_table;

// Temporary node structure for parsing
typedef struct TempNode {
    char *type;
    char *value;
    int child_count;
    char *parent_type;
    char *parent_value;
    Node *node;
    struct TempNode **children;
    int children_capacity;
    int id;
} TempNode;

// Debug print AST
void print_ast(Node *node, int indent) {
    if (!node) {
        printf("%*s[Null node]\n", indent * 2, "");
        return;
    }
    printf("%*s%s", indent * 2, "", node->type);
    if (node->value) printf(" (%s)", node->value);
    printf(", children: %d", node->child_count);
    if (node->child_count > 0) {
        printf(" [");
        for (int i = 0; i < node->child_count; i++) {
            if (i > 0) printf(", ");
            printf("%s", node->children[i]->type);
            if (node->children[i]->value) printf(" (%s)", node->children[i]->value);
        }
        printf("]");
    }
    printf("\n");
    for (int i = 0; i < node->child_count; i++) {
        print_ast(node->children[i], indent + 1);
    }
}

// Map node types to target AST
const char *map_node_type(const char *syntax_type, const char *value) {
    if (strcasecmp(syntax_type, "program") == 0) return "PROGRAM";
    if (strcasecmp(syntax_type, "id") == 0) return "IDENTIFIER";
    if (strcasecmp(syntax_type, "var_declaration") == 0) return "VAR";
    if (strcasecmp(syntax_type, "basic_type") == 0 && value && strcasecmp(value, "integer") == 0) return "INTEGER";
    if (strcasecmp(syntax_type, "statement") == 0 && value && strcasecmp(value, "assign") == 0) return "ASSIGN";
    if (strcasecmp(syntax_type, "statement") == 0 && value && strcasecmp(value, "write") == 0) return "WRITE";
    if (strcasecmp(syntax_type, "compound_statement") == 0) return "TOKEN_BEGIN";
    if (strcasecmp(syntax_type, "simple_expression") == 0 && value && strcasecmp(value, "plus") == 0) return "PLUS";
    if (strcasecmp(syntax_type, "factor") == 0 && value && value[0] && isdigit((unsigned char)value[0])) return "INTEGER_CONST";
    // Skip wrapper nodes
    if (strcasecmp(syntax_type, "program_head") == 0 ||
        strcasecmp(syntax_type, "program_body") == 0 ||
        strcasecmp(syntax_type, "const_declarations") == 0 ||
        strcasecmp(syntax_type, "var_declarations") == 0 ||
        strcasecmp(syntax_type, "subprogram_declarations") == 0 ||
        strcasecmp(syntax_type, "idlist") == 0 ||
        strcasecmp(syntax_type, "variable") == 0 ||
        strcasecmp(syntax_type, "id_varpart") == 0 ||
        strcasecmp(syntax_type, "expression") == 0 ||
        strcasecmp(syntax_type, "expression_list") == 0 ||
        strcasecmp(syntax_type, "statement_list") == 0 ||
        strcasecmp(syntax_type, "simple_expression") == 0 ||
        strcasecmp(syntax_type, "term") == 0 ||
        strcasecmp(syntax_type, "factor") == 0 ||
        (value && strcasecmp(value, "empty") == 0)) return NULL;
    return NULL;
}

// Parse a single node line
TempNode *parse_node_line(const char *line, int id) {
    TempNode *tnode = (TempNode *)malloc(sizeof(TempNode));
    tnode->type = NULL;
    tnode->value = NULL;
    tnode->child_count = 0;
    tnode->parent_type = NULL;
    tnode->parent_value = NULL;
    tnode->node = NULL;
    tnode->children = NULL;
    tnode->children_capacity = 0;
    tnode->id = id;

    char type[256] = {0}, value[256] = {0}, parent_type[256] = {0}, parent_value[256] = {0};
    int child_count = 0;

    // Expected format: Node(type: X, value: Y, child_count: Z, parent: [type: P, value: Q])
    char *temp = strdup(line);
    char *ptr = temp;

    // Skip "Node("
    if (strncmp(ptr, "Node(", 5) == 0) ptr += 5;

    // Parse type
    ptr = strstr(ptr, "type: ");
    if (ptr) {
        ptr += 6;
        char *end = strchr(ptr, ',');
        if (end) {
            strncpy(type, ptr, end - ptr);
            type[end - ptr] = '\0';
            ptr = end + 1;
        }
    }

    // Parse value
    ptr = strstr(ptr, "value: ");
    if (ptr) {
        ptr += 7;
        char *end = strchr(ptr, ',');
        if (end) {
            if (strncmp(ptr, "NULL", 4) == 0) {
                value[0] = '\0';
            } else {
                strncpy(value, ptr, end - ptr);
                value[end - ptr] = '\0';
            }
            ptr = end + 1;
        }
    }

    // Parse child_count
    ptr = strstr(ptr, "child_count: ");
    if (ptr) {
        ptr += 13;
        child_count = atoi(ptr);
        ptr = strchr(ptr, ',');
        if (ptr) ptr++;
    }

    // Parse parent
    ptr = strstr(ptr, "parent: [type: ");
    if (ptr) {
        ptr += 15;
        char *end = strchr(ptr, ',');
        if (end) {
            strncpy(parent_type, ptr, end - ptr);
            parent_type[end - ptr] = '\0';
            ptr = end + 1;
        }
        ptr = strstr(ptr, "value: ");
        if (ptr) {
            ptr += 7;
            end = strchr(ptr, ']');
            if (end) {
                if (strncmp(ptr, "NULL", 4) == 0) {
                    parent_value[0] = '\0';
                } else {
                    strncpy(parent_value, ptr, end - ptr);
                    parent_value[end - ptr] = '\0';
                }
            }
        }
    }

    tnode->type = strdup(type);
    tnode->value = value[0] ? strdup(value) : NULL;
    tnode->child_count = child_count;
    tnode->parent_type = parent_type[0] ? strdup(parent_type) : NULL;
    tnode->parent_value = parent_value[0] ? strdup(parent_value) : NULL;
    tnode->children_capacity = child_count > 0 ? child_count : 1;
    tnode->children = (TempNode **)malloc(sizeof(TempNode *) * tnode->children_capacity);
    for (int i = 0; i < tnode->children_capacity; i++) tnode->children[i] = NULL;

    free(temp);
    return tnode;
}

// Free TempNode
void free_temp_node(TempNode *tnode) {
    if (!tnode) return;
    free(tnode->type);
    free(tnode->value);
    free(tnode->parent_type);
    free(tnode->parent_value);
    free(tnode->children);
    free(tnode);
}

// Convert TempNode to Node with mapping
Node *convert_to_node(TempNode *tnode, TempNode **nodes, int node_count) {
    if (!tnode) return NULL;

    const char *mapped_type = map_node_type(tnode->type, tnode->value);
    if (!mapped_type) {
        // Wrapper node: collect all children
        int total_children = 0;
        for (int i = 0; i < tnode->child_count; i++) {
            if (tnode->children[i]) {
                Node *child = convert_to_node(tnode->children[i], nodes, node_count);
                if (child) total_children++;
            }
        }
        Node **children = (Node **)malloc(sizeof(Node *) * total_children);
        int child_index = 0;
        for (int i = 0; i < tnode->child_count; i++) {
            if (tnode->children[i]) {
                Node *child = convert_to_node(tnode->children[i], nodes, node_count);
                if (child) children[child_index++] = child;
            }
        }
        if (child_index == 0) {
            free(children);
            return NULL;
        }
        if (child_index == 1) {
            Node *result = children[0];
            free(children);
            return result;
        }
        Node *node = create_node("TEMP", NULL, child_index);
        node->children = children;
        node->child_count = child_index;
        return node;
    }

    // Create mapped node
    Node *node = create_node(mapped_type, tnode->value ? strdup(tnode->value) : NULL, 0);
    int total_children = 0;
    for (int i = 0; i < tnode->child_count; i++) {
        if (tnode->children[i]) {
            Node *child = convert_to_node(tnode->children[i], nodes, node_count);
            if (child) total_children++;
        }
    }
    node->children = (Node **)malloc(sizeof(Node *) * total_children);
    node->child_count = total_children;
    int child_index = 0;
    for (int i = 0; i < tnode->child_count; i++) {
        if (tnode->children[i]) {
            Node *child = convert_to_node(tnode->children[i], nodes, node_count);
            if (child) node->children[child_index++] = child;
        }
    }
    return node;
}

// Read output.txt
Node *read_output_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Error: Cannot open file %s\n", filename);
        return NULL;
    }

    char line[1024];
    TempNode **nodes = NULL;
    int node_count = 0, capacity = 10;
    nodes = (TempNode **)malloc(sizeof(TempNode *) * capacity);

    // Read all nodes
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = 0;
        if (strlen(line) == 0) continue;

        if (node_count >= capacity) {
            capacity *= 2;
            nodes = (TempNode **)realloc(nodes, sizeof(TempNode *) * capacity);
        }
        nodes[node_count] = parse_node_line(line, node_count);
        node_count++;
    }

    // Build parent-child relationships
    for (int i = 0; i < node_count; i++) {
        TempNode *node = nodes[i];
        if (!node->parent_type) continue; // Root has no parent
        for (int j = 0; j < node_count; j++) {
            if (i == j) continue;
            TempNode *potential_parent = nodes[j];
            int type_match = strcasecmp(potential_parent->type, node->parent_type) == 0;
            int value_match = (!potential_parent->value && !node->parent_value) ||
                              (potential_parent->value && node->parent_value &&
                               strcmp(potential_parent->value, node->parent_value) == 0);
            if (type_match && value_match) {
                if (potential_parent->child_count > potential_parent->children_capacity) {
                    potential_parent->children_capacity = potential_parent->child_count;
                    potential_parent->children = (TempNode **)realloc(
                            potential_parent->children,
                            sizeof(TempNode *) * potential_parent->children_capacity
                    );
                }
                for (int k = 0; k < potential_parent->child_count; k++) {
                    if (!potential_parent->children[k]) {
                        potential_parent->children[k] = node;
                        break;
                    }
                }
                break;
            }
        }
    }

    // Find root (no parent)
    TempNode *root_temp = NULL;
    for (int i = 0; i < node_count; i++) {
        if (!nodes[i]->parent_type) {
            root_temp = nodes[i];
            break;
        }
    }

    if (!root_temp) {
        fprintf(stderr, "Error: No root node found\n");
        for (int i = 0; i < node_count; i++) free_temp_node(nodes[i]);
        free(nodes);
        fclose(file);
        return NULL;
    }

    // Convert to final AST
    Node *root = convert_to_node(root_temp, nodes, node_count);

    // Post-process PROGRAM
    if (root && strcmp(root->type, "PROGRAM") == 0) {
        int new_count = 0;
        Node **new_children = (Node **)malloc(sizeof(Node *) * (root->child_count + 1));
        for (int i = 0; i < root->child_count; i++) {
            if (!root->children[i]) continue;
            if (strcmp(root->children[i]->type, "IDENTIFIER") == 0 &&
                root->children[i]->value && strcmp(root->children[i]->value, "main") == 0) {
                free(root->value);
                root->value = strdup("main");
            }
            if (strcmp(root->children[i]->type, "TEMP") == 0) {
                for (int j = 0; j < root->children[i]->child_count; j++) {
                    new_children[new_count++] = root->children[i]->children[j];
                }
                free(root->children[i]->children);
                free(root->children[i]);
            } else {
                new_children[new_count++] = root->children[i];
            }
        }
        free(root->children);
        root->children = new_children;
        root->child_count = new_count;
    }

    // Cleanup
    for (int i = 0; i < node_count; i++) free_temp_node(nodes[i]);
    free(nodes);
    fclose(file);

    if (!root) {
        fprintf(stderr, "Error: No valid AST parsed\n");
    }
    return root;
}

int main() {
    Node *root = read_output_file("output.txt");
    if (!root) {
        fprintf(stderr, "Error: Failed to parse AST\n");
        return 1;
    }

    printf("Parsed AST:\n");
    print_ast(root, 0);
    printf("\n");

    symbol_table_file = fopen("symbol_table.txt", "w");
    if (!symbol_table_file) {
        fprintf(stderr, "Error: Cannot open symbol_table.txt\n");
        free_node(root);
        return 1;
    }
    annotated_ast_file = fopen("annotated_ast.txt", "w");
    if (!annotated_ast_file) {
        fprintf(stderr, "Error: Cannot open annotated_ast.txt\n");
        fclose(symbol_table_file);
        free_node(root);
        return 1;
    }

    current_table = create_symbol_table(NULL);
    fprintf(symbol_table_file, "Phase 1: Symbol Table and Type Checking\n");
    check_symbol_table(root);
    fprintf(annotated_ast_file, "Phase 2: Annotated AST\n");
    print_annotated_ast(root, 0);

    fclose(symbol_table_file);
    fclose(annotated_ast_file);
    free_node(root);
    free_symbol_table(current_table);

    printf("Analysis complete, check symbol_table.txt and annotated_ast.txt\n");
    return 0;
}