#include "ast/ast_stmt.hpp"
#include "ast/stmt_print.hpp"
#include <fstream>
#include <iostream>

void printSyntaxTree(const ProgramStmt* program) {
    // Check if the input program is null
    if (program == nullptr) {
        std::cerr << "Error: ProgramStmt is nullptr." << std::endl;
        return;
    }

    // Generate the string representation of the syntax tree
    std::string treeStr = program_stmt_str(program, 0);

    // Open the output file
    std::ofstream outfile("tree.txt");
    if (!outfile.is_open()) {
        std::cerr << "Error: Unable to open tree.txt for writing." << std::endl;
        return;
    }

    // Write the syntax tree string to the file
    outfile << treeStr;

    // Close the file
    outfile.close();

    // Notify the user of success
    std::cout << "Syntax tree has been written to tree.txt." << std::endl;
}
