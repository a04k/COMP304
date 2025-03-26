#include <cctype> // for std::tolower and std::isdigit
#include <fstream>
#include <iostream>
#include <set>
#include <string>

enum class State {
  START,
  NUMBER,
  COLOR_R,
  COLOR_RE,
  COLOR_RED,
  COLOR_B,
  COLOR_BL,
  COLOR_BLU,
  COLOR_BLUE,
  COLOR_G,
  COLOR_GR,
  COLOR_GRE,
  COLOR_GREE,
  COLOR_GREEN
};

int main() {
  std::ifstream in("./exInput.txt");
  std::ofstream out("./output.txt");

  std::set<std::string> colorsUsed;

  if (!in.is_open()) {
    std::cerr << "Could not open input file\n";
    return 1;
  }
  if (!out.is_open()) {
    std::cerr << "Could not open output file\n";
    return 1;
  }

  std::string input;
  char ch;
  while (in.get(ch)) {
    input += ch;
  }

  int i = 0;
  State state = State::START;

  while (i <= input.size()) {
    char c = (i < input.size()) ? std::tolower(input[i]) : '\0';

    switch (state) {
    case State::START:
      if (c == '(') {
        out << "LPAREN\n";
        i++;
      } else if (c == ')') {
        out << "RPAREN\n";
        i++;
      } else if (c == ',') {
        out << "COMMA\n";
        i++;
      } else if (std::isdigit(c)) {
        out << "NUMBER ";
        out << c;
        state = State::NUMBER;
        i++;
      } else if (c == 'r') {
        state = State::COLOR_R;
        i++;
      } else if (c == 'b') {
        state = State::COLOR_B;
        i++;
      } else if (c == 'g') {
        state = State::COLOR_G;
        i++;
      } else if (std::isspace(c) || c == '\0') {
        i++;
      } else {
        std::cerr << "Error: unexpected character '" << c << "'\n";
        return 1;
      }
      break;

    case State::NUMBER:
      if (std::isdigit(c)) {
        out << c;
        i++;
      } else {
        out << "\n";
        state = State::START;
      }
      break;

    case State::COLOR_R:
      if (c == 'e') {
        state = State::COLOR_RE;
        i++;
      } else {
        std::cerr << "Error: invalid color token at '" << c << "'\n";
        return 1;
      }
      break;

    case State::COLOR_RE:
      if (c == 'd') {
        out << "COLOR red\n";
        state = State::START;
        colorsUsed.insert("red");
        i++;
      } else {
        std::cerr << "Error: invalid color token at '" << c << "'\n";
        return 1;
      }
      break;

    case State::COLOR_B:
      if (c == 'l') {
        state = State::COLOR_BL;
        i++;
      } else {
        std::cerr << "Error: invalid color token at '" << c << "'\n";
        return 1;
      }
      break;

    case State::COLOR_BL:
      if (c == 'u') {
        state = State::COLOR_BLU;
        i++;
      } else {
        std::cerr << "Error: invalid color token at '" << c << "'\n";
        return 1;
      }
      break;

    case State::COLOR_BLU:
      if (c == 'e') {
        out << "COLOR blue\n";
        state = State::START;
        colorsUsed.insert("blue");
        i++;
      } else {
        std::cerr << "Error: invalid color token at '" << c << "'\n";
        return 1;
      }
      break;

    case State::COLOR_G:
      if (c == 'r') {
        state = State::COLOR_GR;
        i++;
      } else {
        std::cerr << "Error: invalid color token at '" << c << "'\n";
        return 1;
      }
      break;

    case State::COLOR_GR:
      if (c == 'e') {
        state = State::COLOR_GRE;
        i++;
      } else {
        std::cerr << "Error: invalid color token at '" << c << "'\n";
        return 1;
      }
      break;

    case State::COLOR_GRE:
      if (c == 'e') {
        state = State::COLOR_GREE;
        i++;
      } else {
        std::cerr << "Error: invalid color token at '" << c << "'\n";
        return 1;
      }
      break;

    case State::COLOR_GREE:
      if (c == 'n') {
        out << "COLOR green\n";
        state = State::START;
        colorsUsed.insert("green");
        i++;
      } else {
        std::cerr << "Error: invalid color token at '" << c << "'\n";
        return 1;
      }
      break;
    }
  }

  in.close();
  out.close();

  std::cout << "\nColors used:\n";
  for (const std::string &color : colorsUsed) {
    std::cout << color << "\n";
  }

  return 0;
}
