#ifndef LL1_PARSER_H
#define LL1_PARSER_H

#include "lexer.h"
#include <iostream>
#include <map>
#include <stack>
#include <vector>

enum class NonTerminal {
  EXPR, EXPR_PRIME, TERM, TERM_PRIME, FACTOR
};

struct ProductionElement {
  bool isNonTerminal;
  NonTerminal nt;
  TokenType terminal;

  // Constructor for non-terminals
  ProductionElement(NonTerminal non_term) 
    : isNonTerminal(true), nt(non_term), terminal(TokenType::END_OF_FILE) {} // Arbitrary valid TokenType

  // Constructor for terminals
  ProductionElement(TokenType tok)
    : isNonTerminal(false), nt(NonTerminal::EXPR), terminal(tok) {}
};

using Production = std::vector<ProductionElement>;
using ParsingTable = std::map<NonTerminal, std::map<TokenType, Production>>;

class LL1Parser {
public:
  LL1Parser(const std::vector<Token>& tokens) : tokens(tokens), currentIndex(0) {
    buildParsingTable();
  }

  void parse() {
    std::stack<ProductionElement> parseStack;
    parseStack.push(ProductionElement(TokenType::END_OF_FILE));
    parseStack.push(ProductionElement(NonTerminal::EXPR));

    while (!parseStack.empty()) {
      ProductionElement top = parseStack.top();
      parseStack.pop();

      TokenType currentToken = tokens[currentIndex].type;

      if (!top.isNonTerminal) {
        if (top.terminal == currentToken) {
          if (top.terminal == TokenType::END_OF_FILE) {
            std::cout << "Parse successful!\n";
            return;
          }
          currentIndex++;
        } else {
          throw std::runtime_error("Syntax error: Unexpected token");
        }
      } else {
        NonTerminal currentNT = top.nt;
        if (parseTable.count(currentNT) && parseTable[currentNT].count(currentToken)) {
          const Production& rule = parseTable[currentNT][currentToken];
          for (int i = rule.size() - 1; i >= 0; --i) {
            parseStack.push(rule[i]);
          }
        } else {
          throw std::runtime_error("Syntax error: Invalid token for production");
        }
      }
    }
  }

private:
  const std::vector<Token>& tokens;
  size_t currentIndex;
  ParsingTable parseTable;

  void buildParsingTable() {
    /* 
     * FIRST(EXPR) = FIRST(TERM) = FIRST(FACTOR) = { ID, NUMBER, ( }
     * FOLLOW(EXPR) = { ), $ }
     * FOLLOW(EXPR_PRIME) = FOLLOW(EXPR) = { ), $ }
     * FOLLOW(TERM) = FIRST(EXPR_PRIME) + FOLLOW(EXPR) = { +, -, ), $ }
     * FOLLOW(TERM_PRIME) = FOLLOW(TERM) = { +, -, ), $ }
     * FOLLOW(FACTOR) = FIRST(TERM_PRIME) + FOLLOW(TERM) = { *, /, +, -, ), $ }
     */

    // EXPR -> TERM EXPR_PRIME
    Production exprProd = {NonTerminal::TERM, NonTerminal::EXPR_PRIME};
    parseTable[NonTerminal::EXPR][TokenType::ID] = exprProd;
    parseTable[NonTerminal::EXPR][TokenType::NUMBER] = exprProd;
    parseTable[NonTerminal::EXPR][TokenType::LPAREN] = exprProd;

    // EXPR_PRIME -> + TERM EXPR_PRIME
    Production exprPrimeProd1 = {TokenType::PLUS, NonTerminal::TERM, NonTerminal::EXPR_PRIME};
    parseTable[NonTerminal::EXPR_PRIME][TokenType::PLUS] = exprPrimeProd1;

    // EXPR_PRIME -> - TERM EXPR_PRIME
    Production exprPrimeProd2 = {TokenType::MINUS, NonTerminal::TERM, NonTerminal::EXPR_PRIME};
    parseTable[NonTerminal::EXPR_PRIME][TokenType::MINUS] = exprPrimeProd2;

    // EXPR_PRIME -> ε
    Production emptyProd = {};
    parseTable[NonTerminal::EXPR_PRIME][TokenType::RPAREN] = emptyProd;
    parseTable[NonTerminal::EXPR_PRIME][TokenType::END_OF_FILE] = emptyProd;

    // TERM -> FACTOR TERM_PRIME
    Production termProd = {NonTerminal::FACTOR, NonTerminal::TERM_PRIME};
    parseTable[NonTerminal::TERM][TokenType::ID] = termProd;
    parseTable[NonTerminal::TERM][TokenType::NUMBER] = termProd;
    parseTable[NonTerminal::TERM][TokenType::LPAREN] = termProd;

    // TERM_PRIME -> * FACTOR TERM_PRIME
    Production termPrimeProd1 = {TokenType::TIMES, NonTerminal::FACTOR, NonTerminal::TERM_PRIME};
    parseTable[NonTerminal::TERM_PRIME][TokenType::TIMES] = termPrimeProd1;

    // TERM_PRIME -> / FACTOR TERM_PRIME
    Production termPrimeProd2 = {TokenType::DIVIDE, NonTerminal::FACTOR, NonTerminal::TERM_PRIME};
    parseTable[NonTerminal::TERM_PRIME][TokenType::DIVIDE] = termPrimeProd2;

    // TERM_PRIME -> ε
    parseTable[NonTerminal::TERM_PRIME][TokenType::PLUS] = emptyProd;
    parseTable[NonTerminal::TERM_PRIME][TokenType::MINUS] = emptyProd;
    parseTable[NonTerminal::TERM_PRIME][TokenType::RPAREN] = emptyProd;
    parseTable[NonTerminal::TERM_PRIME][TokenType::END_OF_FILE] = emptyProd;

    // FACTOR -> ( EXPR )
    Production factorProd1 = {TokenType::LPAREN, NonTerminal::EXPR, TokenType::RPAREN};
    parseTable[NonTerminal::FACTOR][TokenType::LPAREN] = factorProd1;

    // FACTOR -> ID
    Production factorProd2 = {TokenType::ID};
    parseTable[NonTerminal::FACTOR][TokenType::ID] = factorProd2;

    // FACTOR -> NUMBER
    Production factorProd3 = {TokenType::NUMBER};
    parseTable[NonTerminal::FACTOR][TokenType::NUMBER] = factorProd3;
  }
};

#endif