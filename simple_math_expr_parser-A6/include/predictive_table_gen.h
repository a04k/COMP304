// ll1_table_generator.h
#ifndef LL1_TABLE_GENERATOR_H
#define LL1_TABLE_GENERATOR_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <algorithm>

using ParseTable = std::map<std::string, std::map<std::string, std::vector<std::string>>>;

ParseTable generateLL1TableMapDynamic(const std::string& firstFile, const std::string& followFile);

std::string generateLL1TableString(const std::string& firstFile, const std::string& followFile) {
    std::map<std::string, std::set<std::string>> firstSets;
    std::map<std::string, std::set<std::string>> followSets;
    std::string parseTableStr;
    std::set<std::string> nonTerminals;
    std::set<std::string> terminals;

    // Read FIRST sets
    std::ifstream firstStream(firstFile);
    if (firstStream.is_open()) {
        std::string line;
        while (getline(firstStream, line)) {
            std::istringstream iss(line);
            std::string nonTerminal;
            if (iss >> nonTerminal) {
                nonTerminals.insert(nonTerminal);
                std::string symbol;
                while (iss >> symbol) {
                    firstSets[nonTerminal].insert(symbol);
                }
            }
        }
        firstStream.close();
    } else {
        std::cerr << "Error: Could not open first set file: " << firstFile << std::endl;
        return "";
    }

    // Read FOLLOW sets
    std::ifstream followStream(followFile);
    if (followStream.is_open()) {
        std::string line;
        while (getline(followStream, line)) {
            std::istringstream iss(line);
            std::string nonTerminal;
            if (iss >> nonTerminal) {
                nonTerminals.insert(nonTerminal);
                std::string symbol;
                while (iss >> symbol) {
                    followSets[nonTerminal].insert(symbol);
                }
            }
        }
        followStream.close();
    } else {
        std::cerr << "Error: Could not open follow set file: " << followFile << std::endl;
        return "";
    }

    // Identify terminals
    for (const auto& entry : firstSets) {
        for (const auto& symbol : entry.second) {
            if (nonTerminals.find(symbol) == nonTerminals.end() && symbol != "epsilon") {
                terminals.insert(symbol);
            }
        }
    }
    for (const auto& entry : followSets) {
        for (const auto& symbol : entry.second) {
            if (nonTerminals.find(symbol) == nonTerminals.end() && symbol != "epsilon") {
                terminals.insert(symbol);
            }
        }
    }
    terminals.insert("$");

    std::vector<std::string> sortedNonTerminals(nonTerminals.begin(), nonTerminals.end());
    std::sort(sortedNonTerminals.begin(), sortedNonTerminals.end());
    std::vector<std::string> sortedTerminals(terminals.begin(), terminals.end());
    std::sort(sortedTerminals.begin(), sortedTerminals.end());

    for (const auto& nonTerminal : sortedNonTerminals) {
        parseTableStr += nonTerminal;
        std::set<std::string> addedTerminals;
        for (const auto& terminal : sortedTerminals) {
            if (addedTerminals.count(terminal)) continue;

            std::vector<std::string> production;
            if (firstSets.count(nonTerminal) && firstSets.at(nonTerminal).count(terminal)) {
                production = {terminal}; // Assuming production starts with the terminal
                parseTableStr += " " + terminal;
                for (const auto& symbol : production) {
                    parseTableStr += " " + symbol;
                }
                addedTerminals.insert(terminal);
            } else if (firstSets.count(nonTerminal) && firstSets.at(nonTerminal).count("epsilon") && followSets.count(nonTerminal) && followSets.at(nonTerminal).count(terminal)) {
                production = {"epsilon"};
                parseTableStr += " " + terminal;
                for (const auto& symbol : production) {
                    parseTableStr += " " + symbol;
                }
                addedTerminals.insert(terminal);
            }
        }
        parseTableStr += "\n";
    }

    return parseTableStr;
}

#endif 