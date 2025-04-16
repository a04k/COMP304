#ifndef PARSE_TABLE_GENERATOR_H
#define PARSE_TABLE_GENERATOR_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <set>
#include <string>
#include "LL1_parser.h" // For ParseTable and token types

class ParseTableGenerator {
private:
    std::map<std::string, std::set<std::string>> first;  // First sets
    std::map<std::string, std::set<std::string>> follow; // Follow sets
    std::map<std::string, std::vector<std::vector<std::string>>> prods; // Productions
    ParseTable table; // Parse table
    std::set<std::string> terms, nonterms; // Terminals and non-terminals

public:
    // Load grammar from file (expects ":" separator)
    bool loadGrammar(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Cannot open grammar file.\n";
            return false;
        }

        std::string line;
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::string nt, colon, sym;

            // Read non-terminal and colon
            iss >> nt >> colon;
            if (colon != ":") continue;

            // Read production symbols
            std::vector<std::string> prod;
            while (iss >> sym) prod.push_back(sym);

            // Store production
            prods[nt].push_back(prod);
            nonterms.insert(nt);
        }
        file.close();

        // Find terminals
        for (const auto& nt : prods) {
            for (const auto& prod : nt.second) {
                for (const auto& sym : prod) {
                    if (sym != "epsilon" && !nonterms.count(sym)) terms.insert(sym);
                }
            }
        }
        terms.insert("$"); // Add end-of-file

        return true;
    }

    // Compute first sets
    void computeFirst() {
        for (const auto& nt : nonterms) first[nt] = {};

        bool changed = true;
        while (changed) {
            changed = false;
            for (const auto& nt : prods) {
                for (const auto& prod : nt.second) {
                    if (prod.size() == 1 && prod[0] == "epsilon") {
                        if (first[nt.first].insert("epsilon").second) changed = true;
                    } else {
                        auto sym = prod[0];
                        if (terms.count(sym)) {
                            if (first[nt.first].insert(sym).second) changed = true;
                        } else if (nonterms.count(sym)) {
                            for (const auto& t : first[sym]) {
                                if (t != "epsilon" && first[nt.first].insert(t).second) changed = true;
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
        follow[prods.begin()->first].insert("$"); // Start symbol gets $

        bool changed = true;
        while (changed) {
            changed = false;
            for (const auto& nt : prods) {
                for (const auto& prod : nt.second) {
                    for (size_t i = 0; i < prod.size(); ++i) {
                        if (!nonterms.count(prod[i])) continue;
                        if (i + 1 < prod.size()) {
                            auto next = prod[i + 1];
                            if (terms.count(next)) {
                                if (follow[prod[i]].insert(next).second) changed = true;
                            } else if (nonterms.count(next)) {
                                for (const auto& t : first[next]) {
                                    if (t != "epsilon" && follow[prod[i]].insert(t).second) changed = true;
                                }
                            }
                        }
                        if (i + 1 == prod.size() || first[prod[i + 1]].count("epsilon")) {
                            for (const auto& t : follow[nt.first]) {
                                if (follow[prod[i]].insert(t).second) changed = true;
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
        for (const auto& nt : prods) {
            for (const auto& prod : nt.second) {
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
                            else hasEpsilon = true;
                        }
                    }
                }

                // Add production for each terminal in first set
                for (const auto& t : firsts) table[nt.first][t] = prod;

                // If epsilon, add for follow set
                if (hasEpsilon) {
                    for (const auto& t : follow[nt.first]) table[nt.first][t] = prod;
                }
            }
        }

        // Print parse table
        std::cout << "\nParse Table:\n";
        for (const auto& nt : table) {
            for (const auto& t : nt.second) {
                std::cout << nt.first << " -> " << t.first << ": ";
                for (const auto& sym : t.second) std::cout << sym << " ";
                std::cout << "\n";
            }
        }
    }

    // Save parse table to file (with tabs)
    bool saveTable(const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Cannot save parse table.\n";
            return false;
        }

        for (const auto& nt : table) {
            for (const auto& t : nt.second) {
                file << nt.first << "\t" << t.first;
                for (const auto& sym : t.second) file << "\t" << sym;
                file << "\n";
            }
        }
        file.close();
        return true;
    }

    // Get parse table for main.cpp compatibility
    ParseTable getParseTable() const {
        return table;
    }
};

#endif // PARSE_TABLE_GENERATOR_H