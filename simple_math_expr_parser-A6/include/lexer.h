#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <vector>
#include <cctype>

enum class TokenType {
    ID, NUMBER,
    PLUS, MINUS, TIMES, DIVIDE,
    LPAREN, RPAREN,
    END_OF_FILE,
    INVALID
};

struct Token {
    TokenType type;
    std::string lexeme;

    Token(TokenType t, const std::string& l) : type(t), lexeme(l) {}

    std::string typeToString() const {
        switch(type) {
            case TokenType::ID: return "ID";
            case TokenType::NUMBER: return "NUMBER";
            case TokenType::PLUS: return "PLUS";
            case TokenType::MINUS: return "MINUS";
            case TokenType::TIMES: return "TIMES";
            case TokenType::DIVIDE: return "DIVIDE";
            case TokenType::LPAREN: return "LPAREN";
            case TokenType::RPAREN: return "RPAREN";
            case TokenType::END_OF_FILE: return "EOF";
            default: return "INVALID";
        }
    }
};

class Lexer {
public:
    Lexer(const std::string& input) : input(input), pos(0) {}

    std::vector<Token> tokenize() {
        std::vector<Token> tokens;
        while (pos < input.size()) {
            char c = input[pos];

            if (isspace(c)) {
                pos++;
                continue;
            }

            if (isalpha(c)) {
                tokens.push_back(scanIdentifier());
            }
            else if (isdigit(c)) {
                tokens.push_back(scanNumber());
            }
            else {
                switch (c) { 
                  // emplace back is effectively just a push back that doesnt copy the object / var into the vector, emplace back directly places within vector
                  // this was a suggestion to use for performance
                    case '+': tokens.emplace_back(TokenType::PLUS, "+"); break;
                    case '-': tokens.emplace_back(TokenType::MINUS, "-"); break;
                    case '*': tokens.emplace_back(TokenType::TIMES, "*"); break;
                    case '/': tokens.emplace_back(TokenType::DIVIDE, "/"); break;
                    case '(': tokens.emplace_back(TokenType::LPAREN, "("); break;
                    case ')': tokens.emplace_back(TokenType::RPAREN, ")"); break;
                    default: throw std::runtime_error("Unexpected character: " + std::string(1, c));
                }
                pos++;
            }
        }
        tokens.emplace_back(TokenType::END_OF_FILE, "");
        return tokens;
    }

private:
    Token scanIdentifier() {
        size_t start = pos;
        while (pos < input.size() && isalnum(input[pos])) pos++;   // alnum = alphanumeric 
        // this loops from current pos until a non alphanumeric is found so a space or something, very simple, probably problematic
        return Token(TokenType::ID, input.substr(start, pos - start));
    }

    Token scanNumber() { // same as the above code really but just digits.
        size_t start = pos;
        while (pos < input.size() && isdigit(input[pos])) pos++;
        return Token(TokenType::NUMBER, input.substr(start, pos - start));
    }

    std::string input;
    size_t pos;
};

#endif // LEXER_H
