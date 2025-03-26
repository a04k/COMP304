#include <iostream>
#include <fstream>
#include <string>
#include "include/lexer.h"
#include "include/parser.h"

std::string readInputFile() {
    std::ifstream file("ex_input/input.txt");
    if (!file) {
      throw std::runtime_error("Could not open input file");
    }
    return std::string((std::istreambuf_iterator<char>(file)),
                      std::istreambuf_iterator<char>());
}

void writeTokens(const std::vector<Token>& tokens) {
  std::ofstream out("Outputs/tokens.txt");
  if (!out) {
    throw std::runtime_error("Could not open token output file");
  }
  
  for (const auto& token : tokens) {
    out << token.typeToString() << "\t'" << token.lexeme << "'\n";
  }
}

int main() {
  try {
    std::string input = readInputFile();

    Lexer lexer(input);
    std::vector<Token> tokens = lexer.tokenize();
    writeTokens(tokens);

    Parser parser(tokens);
    parser.parse();

    std::cout << "Parsing completed \n";
    std::cout << "Tokens saved to Outputs/tokens.txt\n";

    return 0;
  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }
}
