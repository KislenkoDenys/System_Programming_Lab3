#include <iostream>
#include <fstream>
#include <memory>
#include "ast.h"
#include <map>

extern int yyparse();
extern FILE* yyin;
extern std::shared_ptr<BlockNode> rootAST;

int main(int argc, char** argv) {
    if (argc > 1) {
        FILE* file = fopen(argv[1], "r");
        if (!file) {
            std::cerr << "Error: failed to open file " << argv[1] << std::endl;
            return 1;
        }
        yyin = file;
    } else {
        std::cout << "Enter C code (press Ctrl+D to finish):\n";
    }

    std::cout << "[1] Starting syntax analysis...\n";
    if (yyparse() == 0 && rootAST) {
        std::cout << "[+] Parsing completed successfully!\n\n";

        std::ofstream jsonFile("ast.json");
        jsonFile << rootAST->toJSON();
        jsonFile.close();
        std::cout << "[+] AST successfully exported to 'ast.json'\n\n";

        std::cout << "  GENERATED CODE\n\n";
        std::cout << rootAST->generateCode(0);
        std::cout << "\n  SEMANTIC EVALUATION (MEMORY STATE)\n\n";
        std::map<std::string, int> memory;
        rootAST->evaluate(memory);

        for (const auto& [variable, value] : memory) {
            std::cout << "  " << variable << " = " << value << "\n";
        }
        std::cout << "\n";
    } else {
        std::cerr << "[-] Syntax analysis failed.\n";
        return 1;
    }

    return 0;
}
