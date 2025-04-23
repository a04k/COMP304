#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <vector>
#include <map>
#include <set>
#include <cctype>
#include <stdexcept>

struct Token {
    std::string type;
    std::string lexeme;

    Token(const std::string& t, const std::string& l) : type(t), lexeme(l) {}
};

class Lexer {
private:
    std::string input;
    size_t pos;
    
    // Maps for token definitions
    std::map<std::string, std::string> operators;
    std::map<std::string, std::string> delimiters;
    std::map<std::string, std::string> keywords;

public:
    Lexer(const std::string& input) : input(input), pos(0) {
        // Default token definitions - can be modified
        
        // Basic operators
        operators["+"] = "PLUS";
        operators["-"] = "MINUS";
        operators["*"] = "TIMES";
        operators["/"] = "DIVIDE";
        operators["="] = "ASSIGN";
        
        // Basic delimiters
        delimiters["("] = "LPAREN";
        delimiters[")"] = "RPAREN";
        delimiters[","] = "COMMA";
        delimiters[";"] = "SEMICOLON";
        
        // Basic keywords
        keywords["int"] = "DATATYPE";
        keywords["float"] = "DATATYPE";
        keywords["char"] = "DATATYPE";
    }

    // Configuration methods
    void addOperator(const std::string& symbol, const std::string& type) {
        operators[symbol] = type;
    }
    
    void addDelimiter(const std::string& symbol, const std::string& type) {
        delimiters[symbol] = type;
    }
    
    void addKeyword(const std::string& word, const std::string& type) {
        keywords[word] = type;
    }
    
    void clearTokenDefinitions() {
        operators.clear();
        delimiters.clear();
        keywords.clear();
    }

    // Scan an identifier or keyword
    Token scanIdentifier() {
        size_t start = pos;
        while (pos < input.size() && (isalnum(input[pos]) || input[pos] == '_')) pos++;
        std::string text = input.substr(start, pos - start);
        
        // Check if it's a keyword
        auto it = keywords.find(text);
        return (it != keywords.end()) ? Token(it->second, text) : Token("ID", text);
    }

    // Scan a numeric literal
    Token scanNumber() {
        size_t start = pos;
        bool hasDecimal = false;
        
        while (pos < input.size()) {
            if (isdigit(input[pos])) {
                pos++;
            } else if (input[pos] == '.' && !hasDecimal) {
                pos++;
                hasDecimal = true;
            } else {
                break;
            }
        }
        
        return Token("NUMBER", input.substr(start, pos - start));
    }

    // Try to match operator or delimiter
    bool tryMatch(Token& token) {
        // Check for operators
        for (const auto& op : operators) {
            if (input.substr(pos, op.first.length()) == op.first) {
                token = Token(op.second, op.first);
                pos += op.first.length();
                return true;
            }
        }
        
        // Check for delimiters
        for (const auto& del : delimiters) {
            if (input.substr(pos, del.first.length()) == del.first) {
                token = Token(del.second, del.first);
                pos += del.first.length();
                return true;
            }
        }
        
        return false;
    }

    // Convert input to tokens
    std::vector<Token> tokenize() {
        std::vector<Token> tokens;
        
        while (pos < input.size()) {
            char c = input[pos];
            
            // Skip whitespace
            if (isspace(c)) {
                pos++;
                continue;
            }
            
            // Handle identifiers
            if (isalpha(c) || c == '_') {
                tokens.push_back(scanIdentifier());
                continue;
            }
            
            // Handle numbers
            if (isdigit(c)) {
                tokens.push_back(scanNumber());
                continue;
            }
            
            // Try operators/delimiters
            Token token("", "");
            if (tryMatch(token)) {
                tokens.push_back(token);
                continue;
            }
            
            // Unknown character
            throw std::runtime_error("Unexpected character: " + std::string(1, c));
        }
        
        // Add EOF token
        tokens.emplace_back("EOF", "$");
        return tokens;
    }
};

#endif // LEXER_H