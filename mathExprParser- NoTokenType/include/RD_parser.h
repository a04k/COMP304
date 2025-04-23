#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include <vector>
#include <stdexcept>
#include <fstream>

class Parser {
public:
    Parser(const std::vector<Token>& tokens) : tokens(tokens), current(0) {}

    void parse() {
        parseExp();
        if (currentToken().type != TokenType::END_OF_FILE) {
            throw std::runtime_error("Unexpected token at end of input");
        }
    }

private:
    const Token& currentToken() const {
        return tokens[current];
    }

    void consume(TokenType expected) {
        if (currentToken().type == expected) {
            current++;
        } else {
            throw std::runtime_error("Expected " + tokenTypeToString(expected) +
                                   " but got " + currentToken().typeToString());
        }
    }

    std::string tokenTypeToString(TokenType type) const {
        switch (type) {
            case TokenType::ID: return "ID";
            case TokenType::NUMBER: return "NUMBER";
            case TokenType::PLUS: return "+";
            case TokenType::MINUS: return "-";
            case TokenType::TIMES: return "*";
            case TokenType::DIVIDE: return "/";
            case TokenType::LPAREN: return "(";
            case TokenType::RPAREN: return ")";
            case TokenType::END_OF_FILE: return "EOF";
            default: return "INVALID";
        }
    }

    void parseExp() {
        parseTerm();
        parseExpr();
    }

    void parseExpr() {
        if (currentToken().type == TokenType::PLUS ||
            currentToken().type == TokenType::MINUS) {
            consume(currentToken().type);
            parseTerm();
            parseExpr();
        }
    }

    void parseTerm() {
        parseFactor();
        parseTermP();
    }

    void parseTermP() {
        if (currentToken().type == TokenType::TIMES ||
            currentToken().type == TokenType::DIVIDE) {
            consume(currentToken().type);
            parseFactor();
            parseTermP();
        }
    }

    void parseFactor() {
        if (currentToken().type == TokenType::LPAREN) {
            consume(TokenType::LPAREN);
            parseExp();
            consume(TokenType::RPAREN);
        }
        else if (currentToken().type == TokenType::ID ||
                 currentToken().type == TokenType::NUMBER) {
            consume(currentToken().type);
        }
        else {
            throw std::runtime_error("Expected ID, NUMBER, or '('");
        }
    }

    std::vector<Token> tokens;
    size_t current;
};

#endif // PARSER_H
