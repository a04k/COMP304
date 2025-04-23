#ifndef PARSE_TABLE_GEN_H
#define PARSE_TABLE_GEN_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <set>
#include <string>
#include "LL1_parser_ET.h" // for ParseTable and token types

class ParseTableGenerator {
private:
  std::map<std::string, std::set<std::string>> first;   // first sets
  std::map<std::string, std::set<std::string>> follow;  // follow sets
  std::map<std::string, std::vector<std::vector<std::string>>> prods; // productions
  ParseTable table; // parse table
  std::set<std::string> terms, nonterms; // terminals and non-terminals
  std::string startSymbol; // start symbol

public:
  // Load grammar from file (: is the separator)
  bool loadGrammar(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
      std::cerr << "Error: Cannot open grammar file.\n";
      return false;
    }

    std::string line;
    bool firstRule = true; // Flag to identify the first rule
    while (std::getline(file, line)) {
      std::istringstream iss(line);
      std::string nt, colon, sym;

      // Read non-terminal and colon
      iss >> nt >> colon;
      if (iss.fail() || colon != ":") continue; // Added iss.fail() check

      if (firstRule) {
        startSymbol = nt; // Store the first non-terminal as the start symbol
        firstRule = false;
      }

      // Read production symbols
      std::vector<std::string> prod;
      while (iss >> sym) prod.push_back(sym);

      // Store production
      prods[nt].push_back(prod);
      nonterms.insert(nt);
    }
    file.close();

    // finds terminals
    for (const auto& rule : prods) { // Changed loop variable name for clarity
      for (const auto& prod_vec : rule.second) { // Changed loop variable name for clarity
        for (const auto& sym : prod_vec) { // Changed loop variable name for clarity
          if (sym != "epsilon" && !nonterms.count(sym)) terms.insert(sym);
        }
      }
    }
    terms.insert("$"); // Add eof

    return true;
  }

  // Compute first sets
  void computeFirst() {
    for (const auto& nt : nonterms) first[nt] = {}; // empties/intializes the first set for nonterminal nt

    bool changed = true;
    while (changed) {
      changed = false;
      for (const auto& rule : prods) { // Changed loop variable name for clarity
        const std::string& nt = rule.first;
        for (const auto& prod : rule.second) {
          if (prod.size() == 1 && prod[0] == "epsilon") {
            if (first[nt].insert("epsilon").second) changed = true;
          } else {
            auto sym = prod[0];
            if (terms.count(sym)) {
              if (first[nt].insert(sym).second) changed = true;
            } else if (nonterms.count(sym)) {
              for (const auto& t : first[sym]) {
                if (t != "epsilon" && first[nt].insert(t).second) changed = true;
              }
            }
          }
        }
      }
    }

    // Print first sets
    std::cout << "\nFirst Sets:\n";
    for (const auto& nt : first) {
      std::cout << nt.first << ": ";
      for (const auto& t : nt.second) std::cout << t << " ";
      std::cout << "\n";
    }
  }

  // Compute follow sets
  void computeFollow() {
    for (const auto& nt : nonterms) follow[nt] = {};
    // Ensure start symbol exists before accessing follow
    if (!startSymbol.empty() && nonterms.count(startSymbol)) {
        follow[startSymbol].insert("$"); // Start symbol gets $
    }


    bool changed = true;
    while (changed) {
      changed = false;
      for (const auto& rule : prods) { // Changed loop variable name for clarity
        const std::string& nt_lhs = rule.first; // Non-terminal on the left-hand side
        for (const auto& prod : rule.second) {
          for (size_t i = 0; i < prod.size(); ++i) {
            auto current_sym = prod[i];
            if (!nonterms.count(current_sym)) continue;

            if (i + 1 < prod.size()) {
              auto next = prod[i + 1];
              if (terms.count(next)) {
                if (follow[current_sym].insert(next).second) changed = true;
              } else if (nonterms.count(next)) {
                for (const auto& t : first[next]) {
                  if (t != "epsilon" && follow[current_sym].insert(t).second) changed = true;
                }
              }
            }
            // If B is the last symbol in A -> alpha B, or if B is followed by beta where beta derives epsilon,
            // then Follow(B) includes Follow(A).
            // This simplistic check only handles the case where B is the last symbol OR B is followed by a single non-terminal that derives epsilon.
            if (i + 1 == prod.size() || (i + 1 < prod.size() && nonterms.count(prod[i+1]) && first[prod[i+1]].count("epsilon"))) {
              for (const auto& t : follow[nt_lhs]) {
                if (follow[current_sym].insert(t).second) changed = true;
              }
            }
          }
        }
      }
    }

    // Print follow sets
    std::cout << "\nFollow Sets:\n";
    for (const auto& nt : follow) {
      std::cout << nt.first << ": ";
      for (const auto& t : nt.second) std::cout << t << " ";
      std::cout << "\n";
    }
  }

  // Generate parse table
  void generateTable() {
    computeFirst();
    computeFollow();
    table.clear(); // Ensure table is empty

    for (const auto& rule : prods) { // Changed loop variable name for clarity
      const std::string& nt = rule.first;
      for (const auto& prod : rule.second) {
        std::set<std::string> firsts;
        bool hasEpsilon = false;

        if (prod.size() == 1 && prod[0] == "epsilon") {
          hasEpsilon = true;
        } else {
          auto sym = prod[0];
          if (terms.count(sym)) firsts.insert(sym);
          else if (nonterms.count(sym)) {
            for (const auto& t : first[sym]) {
              if (t != "epsilon") firsts.insert(t);
              else hasEpsilon = true; // If first symbol derives epsilon
                                      // Note: Does not correctly handle productions with sequences like A -> BC where B derives epsilon
            }
          }
        }

        // Add production for each terminal in first set
        for (const auto& t : firsts) {
            if (table.count(nt) && table.at(nt).count(t)) {
                 std::cerr << "Warning: LL(1) conflict for '" << nt << "' on '" << t << "'. Overwriting.\n";
            }
            table[nt][t] = prod;
        }

        // If production can derive epsilon, add for follow set
        if (hasEpsilon) {
          if (follow.count(nt)) { // Check if follow set exists
             for (const auto& t : follow[nt]) {
                if (table.count(nt) && table.at(nt).count(t)) {
                   std::cerr << "Warning: LL(1) conflict for '" << nt << "' on '" << t << "'. Overwriting with epsilon rule.\n";
                }
                table[nt][t] = prod;
             }
          }
        }
      }
    }

    // Print parse table
    std::cout << "\nParse Table:\n";
    for (const auto& nt_pair : table) { // Changed loop variable name for clarity
      const std::string& nt = nt_pair.first;
      for (const auto& term_pair : nt_pair.second) { // Changed loop variable name for clarity
        const std::string& term = term_pair.first;
        const auto& prod = term_pair.second;
        std::cout << nt << "-" << term << ":\t";
        for (const auto& sym : prod) std::cout << sym << " ";
        std::cout << "\n";
      }
    }
  }

  // get parse table to use directly instead of reading in the parser
  ParseTable getParseTable() const {
    return table;
  }
  std::set<std::string> getTerminals() const {
    return terms;
  }
  std::set<std::string> getNonTerminals() const {
      return nonterms;
  }

  // get start symbol
  std::string getStartSymbol() const {
      return startSymbol;
  }
};

#endif // PARSE_TABLE_GEN_H