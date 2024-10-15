#include "grammar.hpp"
#include <iostream>
#include <queue>

template <typename T>
bool in_set(const std::set<T>& s, const T& t){
    return s.find(t) != s.end();
}

void Grammar::addRule(Symbol symbol, Phrase phrase){
    if(__rule.find(symbol) == __rule.end()){
        __rule[symbol] = std::vector<Phrase>({phrase});
    }
    else{
        __rule[symbol].push_back(phrase);
    }
}

void Grammar::eraseLeftRecursion() {
    std::map<Symbol, std::vector<Phrase>> new_rule;

    for(auto rule : __rule){
        const Symbol symbol = rule.first;
        std::vector<Phrase> phrases = rule.second;

        std::vector<Phrase> alpha, beta; // A -> Aα | β
        for(auto phrase : phrases){
            if(phrase.getPhrase().front() == symbol){
                std::vector<Symbol> phrase_symbols = phrase.getPhrase();
                phrase_symbols.erase(phrase_symbols.begin());
                if(phrase_symbols.empty()){ // A -> A
                    std::cout << "Error in eraseLeftRecursion: Infinite recursion" << std::endl;
                    exit(1);
                }
                alpha.push_back(Phrase(phrase_symbols));
            }
            else{
                beta.push_back(phrase);
            }
        }

        if(alpha.empty()){
            new_rule[symbol] = phrases;
            continue;
        }
        else{
            Symbol new_symbol(symbol.getName() + "'");
            for(auto& phrase : beta){
                phrase.insert(new_symbol);
            }
            new_rule[symbol] = beta; // A -> βA'

            for(auto& phrase : alpha){
                phrase.insert(new_symbol);
            }
            new_rule[new_symbol] = alpha; // A' -> αA'
            new_rule[new_symbol].push_back(Phrase({Symbol("epsilon")})); // A' -> ε
        }
    }

    this->__rule = new_rule;
}

void Grammar::eliminateLeftCommonFactor(){
    std::map<Symbol, std::vector<Phrase>> new_rule;

    for(auto rule : __rule){
        const Symbol symbol = rule.first;
        std::vector<Phrase> phrases = rule.second;

        std::map<Symbol, std::vector<Phrase>> factor;
        for(auto phrase : phrases){
            Symbol first_symbol = phrase.getPhrase().front();
            if(factor.find(first_symbol) == factor.end()){
                factor[first_symbol] = std::vector<Phrase>({phrase});
            }
            else{
                factor[first_symbol].push_back(phrase);
            }
        }

        for(auto factor_rule : factor){
            const Symbol factor_symbol = factor_rule.first;
            std::vector<Phrase> factor_phrases = factor_rule.second;

            if(factor_phrases.size() == 1){
                new_rule[symbol].push_back(factor_phrases.front());
            }
            else{
                Symbol new_symbol(symbol.getName() + "'");
                new_rule[symbol].push_back(Phrase({factor_symbol, new_symbol}));
                for(auto& factor_phrase : factor_phrases){
                    std::vector<Symbol> temp_phrase;
                    for(const auto& temp_symbol : factor_phrase.getPhrase()){
                        if(temp_symbol != factor_symbol)
                            temp_phrase.push_back(temp_symbol);
                    }
                    factor_phrase = Phrase(temp_phrase);
                }
                new_rule[new_symbol] = std::vector<Phrase>(factor_phrases);
            }
        }
    }

    this->__rule = new_rule;
}

auto Grammar::firstSet(const Phrase& phrase) -> std::set<Symbol>{
    std::set<Symbol> first_set;
    bool allHaveEpsilon = true;
    for(const auto& symbol : phrase.getPhrase()){
        if(symbol.isTerminal()) {
            first_set.insert(symbol);
            allHaveEpsilon = false;
            break;
        }
        else {
            bool haveEpsilon = false;
            for(const auto& phrase : this->__rule[symbol]){
                std::set<Symbol> temp_set = firstSet(phrase);
                for(const auto& temp_symbol : temp_set){
                    if(temp_symbol.getName() != "ε")
                        first_set.insert(temp_symbol);
                    else
                        haveEpsilon = true;
                }
            }
            if(!haveEpsilon){
                allHaveEpsilon = false;
                break;
            }
        }
    }
    if (allHaveEpsilon) {
        first_set.insert(Symbol("epsilon"));
    }
    return first_set;
}

auto Grammar::followSet(const Symbol& B) -> std::set<Symbol>{
    std::set<Symbol> follow_set;

    if(B == this->__start){
        follow_set.insert(Symbol("$"));
    } // 文法开始符号的FOLLOW集合中加入$

    for(const auto& rule : this->__rule){
        const Symbol A = rule.first;
        std::vector<Phrase> phrases = rule.second;

        for(const auto& phrase : phrases){ // 每一条生成式格式：A -> phrase
            // 1 ---- A -> αBβ // FIRST(β) - {ε} ⊆ FOLLOW(B)
            // 2 ---- A -> αB || (A -> αBβ && ε ∈ FIRST(β)) // FOLLOW(A) ⊆ FOLLOW(B)

            std::vector<Symbol> phrase_symbols = phrase.getPhrase();
            for(auto it = phrase_symbols.begin(); it != phrase_symbols.end(); it++){
                if(*it == B){
                    if(it == phrase_symbols.end() - 1){ // A -> αB
                        if(A != B){
                            std::set<Symbol> temp_set = followSet(A);
                            follow_set.insert(temp_set.begin(), temp_set.end());
                        }
                    }
                    else{ // A -> αBβ
                        std::set<Symbol> temp_set = firstSet(Phrase(std::vector<Symbol>(it + 1, phrase_symbols.end()))); // FIRST(β)
                        for(const auto& temp_symbol : temp_set){
                            if(temp_symbol.getName() != "ε")
                                follow_set.insert(temp_symbol);
                        }
                        if(in_set(temp_set, Symbol("epsilon"))){ // ε ∈ FIRST(β)
                            if(A != B){
                                std::set<Symbol> temp_set = followSet(A);
                                follow_set.insert(temp_set.begin(), temp_set.end());
                            }
                        }
                    }
                }
            }
        }
    }

    return follow_set;
}


void Grammar::print(){
    std::cout << std::endl << "Grammar:" << std::endl;
    for(auto it = __rule.begin(); it != __rule.end(); it++){
        Symbol symbol = it->first;
        std::vector<Phrase> phrases = it->second;
        std::cout << symbol.getName() << " -> ";
        for(auto phrase : phrases){
            for(auto symbol : phrase.getPhrase()){
                std::cout << symbol.getName();
            }
            if(phrase != phrases.back())
                std::cout << " | ";
        }
        std::cout << std::endl;
    }
}

void grammarSet(Grammar& grammar){
    grammar.addRule(Symbol("E"), Phrase({Symbol("E"), Symbol("+"), Symbol("T")}));
    grammar.addRule(Symbol("E"), Phrase({Symbol("E"), Symbol("-"), Symbol("T")}));
    grammar.addRule(Symbol("E"), Phrase({Symbol("T")}));
    grammar.addRule(Symbol("T"), Phrase({Symbol("T"), Symbol("*"), Symbol("F")}));
    grammar.addRule(Symbol("T"), Phrase({Symbol("T"), Symbol("/"), Symbol("F")}));
    grammar.addRule(Symbol("T"), Phrase({Symbol("F")}));
    grammar.addRule(Symbol("F"), Phrase({Symbol("("), Symbol("E"), Symbol(")")}));
    grammar.addRule(Symbol("F"), Phrase({Symbol("num")}));

    grammar.setStart(Symbol("E"));

    grammar.eraseLeftRecursion();
    grammar.eliminateLeftCommonFactor();
}