#ifndef PARSER_HH
#define PARSER_HH

#include "grammar.hh"
#include <set>
#include <map>
#include <vector>
#include <string>

class LR0Item {
private:
    Symbol __left;
    Phrase __right;
    int __dot; // dot位置 表示在下标 i 之前或在下标 i - 1 之后
public:
    LR0Item() {}
    LR0Item(LR0Item& item, bool next) : __left(item.__left), __right(item.__right), __dot(item.__dot) { if(next) __dot++; }
    LR0Item(Symbol left, Phrase right, int dot = 0) : __left(left), __right(right), __dot(dot) {}

    Symbol getDotNext() const { return __right.getPhrase().at(__dot); }

    bool isReduce() const { return __dot == __right.getPhrase().size(); } // 归约项
    std::string toString() const;

    bool operator<(const LR0Item& other) const {
        if(__left != other.__left)
            return __left < other.__left;
        if(__right != other.__right)
            return __right < other.__right;
        return __dot < other.__dot;
    }
    bool operator==(const LR0Item& other) const {
        return __left == other.__left && __right == other.__right && __dot == other.__dot;
    }
};

class LR0VaildItemSet {
private:
    int __id;
    std::set<LR0Item> __items;
    std::map<Symbol, int> __goto;
public:
    LR0VaildItemSet() {}
    void setId(int id) { __id = id; }
    int getId() const { return __id; }
    void insertItem(LR0Item item) { __items.insert(item); }
    auto getItems() const { return __items; }
    void setGoto(Symbol symbol, int id) { __goto[symbol] = id; }
    auto getGoto() const { return __goto; }

    void closure(const Grammar& grammar);

    bool operator==(const LR0VaildItemSet& other) const {
        return __items == other.__items;
    }
};

class viablePrefix_DFA {
private:
    std::vector<LR0VaildItemSet> __viablePrefix_DFA;
public:
    viablePrefix_DFA(Grammar grammar);
    void print() const ;
};

class LRTable {

};

class LRParser {

};

#endif