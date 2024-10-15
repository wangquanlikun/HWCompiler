#ifndef PARSER_H
#define PARSER_H

#include "grammar.hpp"
#include <map>
#include <vector>
#include <iostream>

class LL1Table {
private:
    Grammar __grammar;
    std::map<std::pair<Symbol, Symbol>, Phrase> __table;
public:
    bool createMap(Symbol symbol, Symbol first_symbol, Phrase phrase);
    LL1Table() {}
    LL1Table(Grammar grammar);

    auto getGrammar() const { return __grammar; }
    void print();

    auto getPhrase(Symbol symbol, Symbol first_symbol) const {
        return __table.find(std::make_pair(symbol, first_symbol))->second;
    }
    auto getTable() const { return __table; }
};

class LL1Parser {
private:
    LL1Table __table;
    std::map<std::pair<Symbol, Symbol>, Phrase> __tableMap;
    std::vector<Symbol> __stack;
public:
    LL1Parser(LL1Table table) : __table(table) {
        this->__tableMap = table.getTable();
        __stack.push_back(Symbol("$"));
    }

    void printStack() const {
        int stack_len;
        for(auto it = __stack.begin(); it != __stack.end(); it++) {
            std::cout << it->getName() << " ";
            stack_len += it->getName().size();
            stack_len++;
        }
        for(int i = 0; i < 45 - stack_len; i++) {
            std::cout << " ";
        }
    }
    void parse();
};

#endif