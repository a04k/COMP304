#ifndef LL1_PARSER_H
#define LL1_PARSER_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <stack>
#include <string>
#include <map>
#include <set>
#include <stdexcept>
#include "lexer.h"

using ParseTable = std::map<std::string, std::map<std::string, std::vector<std::string>>>;

std::string tokenTypeToStringParser(TokenType type) {
  switch(type) {
    case TokenType::ID: return "id";
    case TokenType::NUMBER: return "id";
    case TokenType::PLUS: return "+";
    case TokenType::MINUS: return "-";
    case TokenType::TIMES: return "*";
    case TokenType::DIVIDE: return "/";
    case TokenType::LPAREN: return "(";
    case TokenType::RPAREN: return ")";
    case TokenType::END_OF_FILE: return "$";
    default: return "invalid";
  }
}

class LL1Parser {
public:
  ParseTable table;
  std::set<std::string> terminals;
  std::set<std::string> nonTerminals;
  std::string startSymbol;
  std::vector<Token> tokens;

  LL1Parser(const std::vector<Token>& t) : tokens(t), startSymbol("") {}

/*   bool loadParseTable(const std::string& filename) {
      std::ifstream file(filename);
      if (!file.is_open()) {
        std::cerr << "Error: Could not open parse table file.\n";
        return false;
      }
  
      std::string line;
      while (getline(file, line)) {
        if (line.empty()) continue;
  
        size_t firstOpen = line.find('(');
        size_t firstClose = line.find(')', firstOpen);
        size_t secondOpen = line.find('(', firstClose + 1); // Look for the second '(' after the first ')'
        size_t secondClose = line.find(')', secondOpen);
  
        if (firstOpen == std::string::npos || firstClose == std::string::npos ||
            secondOpen == std::string::npos || secondClose == std::string::npos) {
          std::cerr << "Error: Invalid parse table line format: " << line << '\n';
          continue;
        }
  
        std::string keyPairStr = line.substr(firstOpen + 1, firstClose - firstOpen - 1);
        std::string nonTerminal;
        std::string terminal;
  
        size_t commaPos = keyPairStr.find(',');
        if (commaPos != std::string::npos) {
          nonTerminal = keyPairStr.substr(0, commaPos);
          size_t startTerminal = commaPos + 1;
          size_t endTerminal = keyPairStr.length();
          terminal = keyPairStr.substr(startTerminal, endTerminal - startTerminal);
  
          // Remove surrounding quotes from terminal if present
          if (terminal.length() > 2 && terminal.front() == '"' && terminal.back() == '"') {
            terminal = terminal.substr(1, terminal.length() - 2);
          }
          nonTerminal = trim(nonTerminal);
          terminal = trim(terminal);
  
          nonTerminals.insert(nonTerminal);
          terminals.insert(terminal);
        } else {
          std::cerr << "Error: Invalid key pair format in line: " << line << '\n';
          continue;
        }
  
        std::string productionStr = line.substr(secondOpen + 1, secondClose - secondOpen - 1);
        std::vector<std::string> production;
        std::stringstream ss(productionStr);
        std::string symbol;
        while (getline(ss, symbol, ',')) {
          production.push_back(trim(symbol));
        }
  
        table[nonTerminal][terminal] = production;
  
        if (startSymbol.empty()) {
          startSymbol = nonTerminal;
        }
      }
  
      logGrammarSymbols();
      file.close();
      return !startSymbol.empty();
    }    
*/

bool loadParseTable(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open parse table file.\n";
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string nonTerminal, terminal, symbol;

        // Read non-terminal and terminal
        if (!(iss >> nonTerminal >> terminal)) {
            std::cerr << "Error: Invalid line format: " << line << '\n';
            continue;
        }

        // Store symbols in grammar sets
        nonTerminals.insert(nonTerminal);
        terminals.insert(terminal);

        // Read production symbols
        std::vector<std::string> production;
        while (iss >> symbol) {
            production.push_back(symbol);
        }

        // Save to parse table
        table[nonTerminal][terminal] = production;

        // Set start symbol on first rule
        if (startSymbol.empty()) {
            startSymbol = nonTerminal;
        }
    }

    file.close();
    logGrammarSymbols();
    return !startSymbol.empty();
}


private:
  void logGrammarSymbols() const {
    std::cout << "\n--- Grammar Symbols ---\n";
    std::cout << "Non-Terminals: ";
    for (const auto& nt : nonTerminals) {
      std::cout << "'" << nt << "' ";
    }
    std::cout << "\n";
    std::cout << "Terminals: ";
    for (const auto& t : terminals) {
      std::cout << "'" << t << "' ";
    }
    std::cout << "\n-------------------------\n";
  }

public:
  bool parse() {
    int tokenIndex = 0;
    std::stack<std::string> parseStack;
    parseStack.push("$");
    parseStack.push(startSymbol);

    while (!parseStack.empty()) {
      std::string top = parseStack.top();

      std::string currentTokenStr = (tokenIndex < tokens.size()) ?
          tokenTypeToStringParser(tokens[tokenIndex].type) : "$";
      std::string currentLexeme = (tokenIndex < tokens.size()) ?
          tokens[tokenIndex].lexeme : "EOF";

      // log current state
      std::cout << "Stack top: '" << top << "', Current token: '" << currentTokenStr
                << "' (lexeme: '" << currentLexeme << "')\n";

      if (terminals.count(top)) { // if top element is a terminal (if the set of terminals has at least one of the top token)
        if (top == currentTokenStr) {
          std::cout << "Matched terminal: '" << currentLexeme << "'\n";
          parseStack.pop();
          if (top == "$") {
            std::cout << "Parse successful!\n";
            return true;
          }
          tokenIndex++;
        } else {
          std::cerr << "Syntax Error: Unexpected token '" << currentLexeme
                    << "'. Expected: '" << top << "'.\n";
          return false;
        }
      } else if (nonTerminals.count(top)) { // same thing as above just for non terminals
        if (table.count(top) && table[top].count(currentTokenStr)) {
          const std::vector<std::string>& production = table[top][currentTokenStr];
          std::cout << "Applying rule: " << top << " -> ";
          for (const auto& sym : production) {
            std::cout << "'" << sym << "' ";
          }
          std::cout << "\n";
          parseStack.pop();
          if (!(production.size() == 1 && production[0] == "epsilon")) {
            for (int i = production.size() - 1; i >= 0; --i) {
              parseStack.push(production[i]);
            }
          }
        } else {
          std::cerr << "Syntax Error: No production for '" << top
                    << "' with token '" << currentTokenStr << "'.\n";
          return false;
        }
      } else {
        std::cerr << "Internal Error: Unknown symbol on stack: '" << top << "'.\n";
        return false;
      }
    }

    // successful if end of tokens is reached
    if (tokenIndex == tokens.size()) {
      std::cout << "Parse successful!\n";
      return true;
    } else {
      std::cerr << "Syntax Error: Parsing did not complete correctly.\n";
      return false;
    }
  }
};

#endif // LL1_PARSER_H