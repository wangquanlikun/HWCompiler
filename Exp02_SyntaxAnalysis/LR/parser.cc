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
    }
}

void LRTable::create_id(Grammar grammar){
    int id = 0;
    for(auto rule : grammar.getRule()){
        for(auto phrase : rule.second){
            phrase_id[{rule.first, phrase}] = id++;
            id_phrase[phrase_id[{rule.first, phrase}]] = {rule.first, phrase};
        }
    }
}

LRTable::LRTable(viablePrefix_DFA prefix_dfa, Grammar grammar){
    auto dfa = prefix_dfa.getDFA();
    create_id(grammar);
    for(int state_i = 0; state_i < dfa.size(); state_i++){
        auto I_n = dfa[state_i];

        auto I_n_goto = I_n.getGoto();
        for(auto goto_ : I_n_goto){
            if(goto_.first.isTerminal()){
                __action[{state_i, goto_.first}] = {'S', goto_.second};
            }
            else{
                __goto[{state_i, goto_.first}] = goto_.second;
            }
        }

        for(auto item : I_n.getItems()){
            if(!item.isReduce())
                continue;
            
            // S' -> S.
            if(item.getLeft() == grammar.getStart() && item.getRight().getPhrase().size() == item.getDot()){
                __action[{state_i, Symbol("$")}] = {'A', 0};
                continue;
            }

            // A -> α.
            auto A = item.getLeft();
            auto alpha = item.getRight();
            auto id = phrase_id[{A, alpha}];
            for(auto a : grammar.followSet(A)){
                __action[{state_i, a}] = {'R', id};
            }
        }
    }
}

void LRTable::print() const {
    std::cout << std::endl << "LR Table:" << std::endl;
    std::set<Symbol> action_symbol;
    std::set<Symbol> goto_symbol;
    for(auto action : __action){
        action_symbol.insert(action.first.second);
    }
    for(auto goto_ : __goto){
        goto_symbol.insert(goto_.first.second);
    }
    std::cout << "\t";
    int i = 0;
    std::map<Symbol, int> symbol2int;
    for(auto symbol : action_symbol){
        std::cout << symbol.getName() << "\t";
        symbol2int[symbol] = i++;
    }
    for(auto symbol : goto_symbol){
        std::cout << symbol.getName() << "\t";
        symbol2int[symbol] = i++;
    }
    int max_state = 0;
    std::pair<char, int> martix[20][15];
    for(auto action : __action){
        martix[action.first.first][symbol2int[action.first.second]] = action.second;
        max_state = std::max(max_state, action.first.first);
    }
    for(auto goto_ : __goto){
        martix[goto_.first.first][symbol2int[goto_.first.second]] = {'G', goto_.second};
        max_state = std::max(max_state, goto_.first.first);
    }

    for(int i = 0; i < max_state + 1; i++){
        std::cout << std::endl << i << "\t";
        for(int j = 0; j < 15; j++){
            if(martix[i][j].first == 'S')
                std::cout << "S" << martix[i][j].second << "\t";
            else if(martix[i][j].first == 'G')
                std::cout << martix[i][j].second << "\t";
            else if(martix[i][j].first == 'R')
                std::cout << "R" << martix[i][j].second << "\t";
            else if(martix[i][j].first == 'A')
                std::cout << "ACC\t";
            else
                std::cout << "\t";
        }
    }
}