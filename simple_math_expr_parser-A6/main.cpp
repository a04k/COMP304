#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
// #include "include/RD_parser.h" // Unused, kept for reference
#include "include/lexer.h"
#include "include/LL1_parser_ET.h"
#include "include/parse_table_gen.h"

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

    ParseTableGenerator gen;
    if (!gen.loadGrammar("ex_input/grammar.txt")) {
      std::cerr << "Failed to load grammar.\n";
      return 1;
    }
    gen.generateTable();

    LL1Parser ll1Parser(tokens, gen.getParseTable(), gen.getTerminals(), gen.getNonTerminals());

    if (!ll1Parser.parse()) {
      std::cerr << "Parsing failed due to syntax errors.\n";
      return 1;
    }

    std::cout << "Parsing completed\n";
    std::cout << "Tokens saved to Outputs/tokens.txt\n";

    return 0;

  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }
}