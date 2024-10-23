#include "parser.hh"
#include <iostream>

template <typename T>
static bool in_set(const std::set<T>& s, const T& t){
    return s.find(t) != s.end();
}

std::string LR0Item::toString() const {
    std::string str;
    str += __left.getName() + " -> ";
    for(int i = 0; i < __right.getPhrase().size(); i++){
        if(i == __dot)
            str += "∘ ";
        str += __right.getPhrase().at(i).getName();
    }
    if(__dot == __right.getPhrase().size())
        str += "∘ ";
    return str;
}

void LR0VaildItemSet::closure(const Grammar& grammar){
    std::set<LR0Item> J = __items;
    std::set<LR0Item> J_new;

    do{
        J_new = J;
        for(auto item : J_new){ // item: 对于每一个 A -> α.Bβ
            if(item.isReduce())
                continue;
            Symbol B = item.getDotNext();
            if(B.isTerminal())
                continue;
            for(auto rule : grammar.getRule()){ // 对于每一个产生式 B -> γ
                if(rule.first != B)
                    continue;
                auto phrases = rule.second;
                for(auto phrase : phrases){
                    J.insert(LR0Item(B, phrase));
                }
            }
        }
    } while(J != J_new);

    __items = J;
}

viablePrefix_DFA::viablePrefix_DFA(Grammar grammar){
    LR0VaildItemSet I0;
    I0.setId(0);
    I0.insertItem(LR0Item(grammar.getStart(), grammar.getRule().at(grammar.getStart()).front()));
    I0.closure(grammar);
    __viablePrefix_DFA.push_back(I0);

    for(int i = 0; i < __viablePrefix_DFA.size(); i++){
        auto I_n = __viablePrefix_DFA[i];
        for(auto item : I_n.getItems()){
            if(item.isReduce())
                continue;
            Symbol X = item.getDotNext();

            LR0VaildItemSet temp;
            for(auto item : I_n.getItems()){
                if(item.isReduce())
                    continue;
                if(item.getDotNext() == X){
                    temp.insertItem(LR0Item(item, true));
                }
            }
            temp.closure(grammar);

            bool have_same = false;
            for(auto itemSet : __viablePrefix_DFA){
                if(itemSet == temp){
                    have_same = true;
                    __viablePrefix_DFA[i].setGoto(X, itemSet.getId());
                    break;
                }
            }
            if(!have_same){
                temp.setId(__viablePrefix_DFA.size());
                __viablePrefix_DFA[i].setGoto(X, temp.getId());
                __viablePrefix_DFA.push_back(temp);
            }
        }
    }
}

void viablePrefix_DFA::print() const {
    std::cout << std::endl << "Viable Prefix DFA:" ;
    for(auto itemSet : __viablePrefix_DFA){
        std::cout << std::endl << "I" << itemSet.getId() << ":" << std::endl;
        for(auto item : itemSet.getItems()){
            std::cout << item.toString() << std::endl;
        }
        for(auto gotoItem : itemSet.getGoto()){
            std::cout << "( " << gotoItem.first.getName() << " ) => " << gotoItem.second << std::endl;
        }
        std::cout << std::endl;
    }
}