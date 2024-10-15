#ifndef GRAMMAR_H
#define GRAMMAR_H

#include <vector>
#include <map>
#include <set>
#include <string>

class Symbol {
private:
    std::string __name;
    bool __isTerminal;
public:
    Symbol() {}
    Symbol(std::string name) : __name(name) {
        if(name.size() >= 1 && (name[0] >= 'A' && name[0] <= 'Z')) {
            __isTerminal = false;
        } else {
            __isTerminal = true;
        }
    }
    Symbol(std::string name, bool isTerminal) : __name(name), __isTerminal(isTerminal) {}
    void setName(std::string name) { __name = name; }
    std::string getName() const { 
        if(__name == "epsilon") return "ε";
        return __name;
    }
    bool isTerminal() const { return __isTerminal; }

    bool operator<(const Symbol& other) const {
        return __name < other.__name;
    }
    bool operator==(const Symbol& other) const {
        return __name == other.__name;
    }
    bool operator!=(const Symbol& other) const {
        return __name != other.__name;
    }
};

class Phrase {
private:
    std::vector<Symbol> __phrase;
public:
    Phrase() {}
    Phrase(std::vector<Symbol> phrase) : __phrase(phrase) {}
    void insert(Symbol symbol) { __phrase.push_back(symbol); }
    void setPhrase(std::vector<Symbol> phrase) { __phrase = phrase; }
    std::vector<Symbol> getPhrase() const { return __phrase; }
    std::string toString() const {
        std::string str;
        for(const auto& symbol : __phrase) {
            str += symbol.getName();
        }
        return str;
    }

    bool empty() const { return __phrase.empty(); }

    bool operator==(const Phrase& other) const {
        return __phrase == other.__phrase;
    }
    bool operator!=(const Phrase& other) const {
        return __phrase != other.__phrase;
    }
};

class Grammar {
private:
    std::map<Symbol, std::vector<Phrase>> __rule;
    Symbol __start;
public:
    void addRule (Symbol symbol, Phrase phrase);
    void setStart(Symbol symbol) { __start = symbol; }
    Symbol getStart() const { return __start; }

    void eraseLeftRecursion();
    void eliminateLeftCommonFactor();

    auto firstSet(const Phrase& phrase) -> std::set<Symbol>;
    auto followSet(const Symbol& symbol) -> std::set<Symbol>;

    void print();
    auto getRule() const { return __rule; }
};

void grammarSet(Grammar& grammar);

#endif