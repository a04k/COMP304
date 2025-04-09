## Simple Math Expression Lexer + Parser
---

Based on [Assignment 6 ](https://github.com/m-fakhry/comp304-24-25/blob/main/Assignments/assign6.md) of creating a top-down recursive descent parser (& LL1 - Part two)

The lexer creates all the tokens needed in the language and the parser parses & provides syntax error checks

Language CFG Used:

```plaintext
Exp -> Term Expr
Expr ->  + Term Expr | - Term Expr | e
Term -> Factor Termp
Termp ->  * Factor Termp |  / Factor Termp | e  
Factor -> ( Exp ) | id
```

To compile the code (gcc) use the command:

```sh
g++ -I Include main.cpp -o parser
```
---

### Part 2 Update:

added LL1 parser for the language, built the parse table using the following first and follow sets:

```plaintext
* FIRST(EXPR) = FIRST(TERM) = FIRST(FACTOR) = { ID, NUMBER, ( }
* FOLLOW(EXPR) = { ), $ }
* FOLLOW(EXPR_PRIME) = FOLLOW(EXPR) = { ), $ }
* FOLLOW(TERM) = FIRST(EXPR_PRIME) + FOLLOW(EXPR) = { +, -, ), $ }
* FOLLOW(TERM_PRIME) = FOLLOW(TERM) = { +, -, ), $ }
* FOLLOW(FACTOR) = FIRST(TERM_PRIME) + FOLLOW(TERM) = { *, /, +, -, ), $ }
```

built using the following function 

```cpp
void buildParsingTable() {
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
```