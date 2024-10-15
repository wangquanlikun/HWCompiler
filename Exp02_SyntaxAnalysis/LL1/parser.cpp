#include "parser.hpp"
#include <iostream>

bool LL1Table::createMap(Symbol symbol, Symbol first_symbol, Phrase phrase){
    if(__table.find(std::make_pair(symbol, first_symbol)) != __table.end()){
        return false;
    }
    __table[std::make_pair(symbol, first_symbol)] = phrase;
    return true;
}

LL1Table::LL1Table(Grammar grammar){
    this->__grammar = grammar;
    std::map<Symbol, std::vector<Phrase>> gRules = grammar.getRule();
    
    for(const auto& rule : gRules){
        Symbol symbol = rule.first;
        std::vector<Phrase> phrases = rule.second;

        for(auto phrase : phrases){
            std::set<Symbol> first_set = grammar.firstSet(phrase);
            for(auto first_symbol : first_set){
                if(first_symbol.getName() == "ε"){
                    std::set<Symbol> follow_set = grammar.followSet(symbol);
                    for(auto follow_symbol : follow_set){
                        if(!createMap(symbol, follow_symbol, phrase)){
                            std::cout << "Error: Not a LL(1) grammar!" << std::endl;
                            exit(1);
                        }
                    }
                }
                else{
                    if(!createMap(symbol, first_symbol, phrase)){
                        std::cout << "Error: Not a LL(1) grammar!" << std::endl;
                        exit(1);
                    }
                }
            }
        }
    }
}

void LL1Table::print(){
    std::cout << std::endl << "LL(1) Table:" << std::endl;
    std::set<Symbol> symbols;
    std::set<Symbol> first_symbols;
    for(auto it = __table.begin(); it != __table.end(); it++){
        symbols.insert(it->first.first);
        first_symbols.insert(it->first.second);
    }

    for(const auto& first_symbol : first_symbols){
        std::cout << "\t" << first_symbol.getName();
    }
    std::cout << std::endl;
    for(const auto& symbol : symbols){
        std::cout << symbol.getName();
        for(const auto& first_symbol : first_symbols){
            auto it = __table.find(std::make_pair(symbol, first_symbol));
            if(it != __table.end()){
                std::cout << "\t" << it->second.toString();
            }
            else{
                std::cout << "\t";
            }
        }
        std::cout << std::endl;
    }
}

void LL1Parser::parse(){
    std::string input;
    std::cout << std::endl << "Input the string to parse: ";
    std::cin >> input;
    input.append("$");
    __stack.push_back(this->__table.getGrammar().getStart());
    for(int i = 0; i < input.size(); ){
        auto c = input[i];
        
        printStack();
        std::cout << input.substr(i);
        for(int j = 0; j < 30 - input.substr(i).size(); j++){
            std::cout << " ";
        }
        
        Symbol symbol = __stack.back();
        Symbol input_symbol;
        if(c >= '0' && c <= '9'){
            input_symbol = Symbol("num");
        }
        else if(c == ' '){
            i++;
            continue;
        }
        else{
            input_symbol = Symbol(std::string(1, c));
        }

        if(symbol.isTerminal()){
            if(symbol == input_symbol){
                if(symbol.getName() == "$"){
                    std::cout << std::endl << "Accept!" << std::endl;
                    return;
                }
                __stack.pop_back();
                i++;
                std::cout << std::endl;
            }
            else{
                std::cout << "Error: Not a LL(1) grammar!" << std::endl;
                exit(1);
            }
        }
        else{
            auto temp_it = __tableMap.find(std::make_pair(symbol, input_symbol));
            if(temp_it == __tableMap.end()){
                std::cout << "Error: Not a LL(1) grammar!" << std::endl;
                exit(1);
            }
            Phrase phrase = __tableMap[std::make_pair(symbol, input_symbol)];
            __stack.pop_back();
            if(!(phrase.getPhrase().size() == 1 && phrase.getPhrase()[0].getName() == "ε")){
                std::vector<Symbol> temp;
                for(auto symbol : phrase.getPhrase()){
                    temp.push_back(symbol);
                }
                for(int i = 0; i < phrase.getPhrase().size(); i++){
                    auto tempSymbol = temp.back();
                    __stack.push_back(tempSymbol);
                    temp.pop_back();
                }
            }
            std::cout << symbol.getName() << " -> " << phrase.toString() << std::endl;
        }
    }
    std::cout << "Error: Not a LL(1) grammar!" << std::endl;
}