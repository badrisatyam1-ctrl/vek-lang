#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>

#include <string>
#include <vector>
#include <unordered_map>

#include "./headers/lexer.hpp"
#include "./headers/parser.hpp"
#include "./headers/compiler.hpp"
#include "./headers/vm.hpp"

using std::cerr;
using std::cout;
using std::cin;
using std::endl;

using std::string;
using std::vector;

using std::ifstream;
using std::stringstream;

string read_file(const string& path) {
    ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Can't open file: " + path);
    }

    stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        cerr << "Usage: vek <file.vek>" << endl;
        return 1;
    }

    string path = argv[1];
    string code = read_file(path);

    Lexer lexer(code);
    auto tokens = lexer.lex();

    Parser parser(std::move(tokens));
    auto parsed = parser.parse_program();

    Compiler compiler;
    Bytecode bytecode = compiler.compile(parsed);

    VM vm;
    vm.run(bytecode);

    return 0;
}