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
