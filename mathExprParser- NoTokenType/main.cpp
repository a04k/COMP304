#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <stdexcept>

#include "include/lexer.h"
#include "include/LL1_parser_ET.h"
#include "include/parse_table_gen.h"

std::string readInputFile(const std::string& filename = "ex_input/input.txt") {
  std::ifstream file(filename);
  if (!file) {
    throw std::runtime_error("Could not open input file: " + filename);
  }
  return std::string((std::istreambuf_iterator<char>(file)),
                     std::istreambuf_iterator<char>());
}

void writeTokens(const std::vector<Token>& tokens, const std::string& filename = "Outputs/tokens.txt") {
  std::ofstream out(filename);
  if (!out) {
    throw std::runtime_error("Could not open token output file: " + filename);
  }
  for (const auto& token : tokens) {
    out << token.type << "\t\t'" << token.lexeme << "'\n";
  }
}

int main() {
  try {
    std::string input = readInputFile();
    std::cout << "Read input from file.\n";

    Lexer lexer(input);
    std::vector<Token> tokens = lexer.tokenize();
    std::cout << "Lexing complete.\n";
    writeTokens(tokens);
    std::cout << "Tokens saved to Outputs/tokens.txt\n";

    ParseTableGenerator gen;
    if (!gen.loadGrammar("ex_input/grammar.txt")) {
      std::cerr << "Failed to load grammar.\n";
      return 1;
    }
    std::cout << "Grammar loaded \n";

    std::cout << "Generating First/Follow sets and Parse Table...\n";
    gen.generateTable(); 
    // i should add a check here / make the gentable function return bool for an easy check w / cond
    std::cout << "Parse Table generation process completed.\n";

    LL1Parser ll1Parser(
      tokens,
      gen.getParseTable(),
      gen.getTerminals(),
      gen.getNonTerminals(),
      gen.getStartSymbol() // Get start symbol from gen
    );

    std::cout << "Starting LL1 Parsing process...\n";
    if (!ll1Parser.parse()) {
      std::cerr << "\nParsing failed.\n";
      return 1;
    }

    // Success
    std::cout << "\nParsing completed successfully.\n";

    return 0;

  } catch (const std::exception& e) {
    std::cerr << "\nUnhandled Exception: " << e.what() << std::endl;
    return 1;
  } catch (...) {
    std::cerr << "\nAn unknown error occurred." << std::endl;
    return 1;
  }
}