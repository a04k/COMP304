#ifndef LL1_PARSER_ET_H
#define LL1_PARSER_ET_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <stack>
#include <string>
#include <map>
#include <set>
#include <stdexcept>
#include "lexer.h" // lexer included for token struct

using ParseTable = std::map<std::string, std::map<std::string, std::vector<std::string>>>;

std::string tokenToParserSymbol(const Token& token) {
  if (token.type == "ID") return "id";
  // if (token.type == "NUMBER") return "id"; 

  if (token.type == "DATATYPE") {
    return token.lexeme; // e.g., "int", "float", "char"
  }

  if (token.type == "PLUS") return "+";
  if (token.type == "MINUS") return "-";
  if (token.type == "TIMES") return "*";
  if (token.type == "DIVIDE") return "/";
  if (token.type == "LPAREN") return "(";
  if (token.type == "RPAREN") return ")";
  if (token.type == "COMMA") return ",";
  if (token.type == "SEMICOLON") return ";";
  if (token.type == "ASSIGN") return "=";

  // eof / $
  if (token.type == "EOF") return "$";

  // Fallback: Assume the lexeme itself might be the terminal symbol.
  std::cerr << "Warning: Unhandled token type in tokenToParserSymbol: '" << token.type
            << "'. Falling back to using its lexeme '" << token.lexeme << "' as the symbol.\n";
  return token.lexeme;
}


class LL1Parser {
public:
  ParseTable table;
  std::set<std::string> terminals;
  std::set<std::string> nonTerminals;
  std::string startSymbol;
  std::vector<Token> tokens;

  LL1Parser(const std::vector<Token>& t,
          const ParseTable& parseTable,
          const std::set<std::string>& terms,
          const std::set<std::string>& nonTerms,
          const std::string& startSym)
    : tokens(t),
      table(parseTable),
      terminals(terms),
      nonTerminals(nonTerms),
      startSymbol(startSym)
  {
    if (startSymbol.empty() && !nonTerminals.empty()) {
      std::cerr << "Warning: Start symbol not provided or empty.\n";
    }
    terminals.insert("$"); // $ is treated as a terminal
  }

  bool parse() {
    if (startSymbol.empty()) {
      std::cerr << "Error: Cannot parse without a start symbol.\n";
      return false;
    }
    if (tokens.empty()) {
      std::cerr << "Error: No tokens to parse.\n";
      return false;
    }
    if (!nonTerminals.count(startSymbol)) {
      std::cerr << "Error: Provided start symbol '" << startSymbol << "' is not in the set of non-terminals.\n";
      return false;
    }

    int tokenIndex = 0;
    std::stack<std::string> parseStack;
    parseStack.push("$");
    parseStack.push(startSymbol);

    std::cout << "\n--- Starting Parse ---" << std::endl;

    while (!parseStack.empty()) {
      std::string stackTop = parseStack.top();
      std::string currentTokenSymbol = "$";
      std::string currentLexeme = "EOF";

      if (tokenIndex < tokens.size()) {
        currentTokenSymbol = tokenToParserSymbol(tokens[tokenIndex]);
        currentLexeme = tokens[tokenIndex].lexeme;
        if (tokens[tokenIndex].type == "EOF") {
          currentLexeme = "$";
          currentTokenSymbol = "$"; // Ensure EOF maps to $ symbol via function
        }
      } else if (stackTop != "$"){
        std::cerr << "Syntax Error: Unexpected end of input. Expected token for stack symbol '" << stackTop << "'.\n";
        return false;
      }

      std::cout << "Stack top: '" << stackTop
                << "', Current token symbol: '" << currentTokenSymbol
                << "' (Lexeme: '" << currentLexeme << "')" << std::endl;

      if (terminals.count(stackTop)) {
        if (stackTop == currentTokenSymbol) {
          std::cout << "  Action: Matched terminal '" << stackTop << "'. Popping stack, advancing input.\n";
          parseStack.pop();
          tokenIndex++;

          if (stackTop == "$") {
            if (tokenIndex >= tokens.size() || tokens[tokenIndex-1].type == "EOF") {
              std::cout << "Parse successful!\n";
              return true;
            } else {
              std::cerr << "Syntax Error: Input remaining after end-of-input marker '$' was processed. Next token: '"
                        << tokens[tokenIndex].lexeme << "' (Symbol: '" << tokenToParserSymbol(tokens[tokenIndex]) << "').\n";
              return false;
            }
          }
        } else {
          std::cerr << "Syntax Error: Mismatch. Expected terminal '" << stackTop
                    << "' but found token '" << currentTokenSymbol
                    << "' (Lexeme: '" << currentLexeme << "').\n";
          return false;
        }
      }
      else if (nonTerminals.count(stackTop)) {
        if (table.count(stackTop) && table.at(stackTop).count(currentTokenSymbol)) {
          const std::vector<std::string>& production = table.at(stackTop).at(currentTokenSymbol);

          std::cout << "  Action: Apply rule " << stackTop << " ->";
          for (const auto& sym : production) std::cout << " '" << sym << "'";
          std::cout << ". Popping stack, pushing production.\n";

          parseStack.pop();

          if (!(production.size() == 1 && production[0] == "epsilon")) {
            for (int i = production.size() - 1; i >= 0; --i) {
              if (!terminals.count(production[i]) && !nonTerminals.count(production[i])) {
                std::cerr << "Internal Error: Grammar production for '" << stackTop << "' contains unknown symbol '" << production[i] << "' that is neither terminal nor non-terminal.\n";
                return false;
              }
              parseStack.push(production[i]);
            }
          } else {
            std::cout << "  (Epsilon production, only popped stack)\n";
          }
        } else {
          std::cerr << "Syntax Error: No production rule found for Non-Terminal '" << stackTop
                    << "' with lookahead token symbol '" << currentTokenSymbol
                    << "' (Lexeme: '" << currentLexeme << "').\n";
          if (table.count(stackTop)) {
            std::cerr << "  Possible expected token symbols for '" << stackTop << "':";
            for(const auto& pair : table.at(stackTop)) std::cerr << " '" << pair.first << "'";
            std::cerr << std::endl;
          } else {
            std::cerr << "  Non-terminal '" << stackTop << "' has no rules defined in the parse table.\n";
          }
          return false;
        }
      }
      else {
        std::cerr << "Internal Error: Symbol '" << stackTop
                  << "' on stack is neither a known terminal nor a non-terminal. Check grammar/sets generation.\n";
        return false;
      }
    }

    std::cerr << "Syntax Error: Parsing ended unexpectedly. Stack empty, but end condition not met?\n";
    return false;
  }
};

#endif 