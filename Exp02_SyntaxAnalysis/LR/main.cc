#include "grammar.hh"
#include "parser.hh"

#include <iostream>

void config(){
    #ifdef _WIN32
    system("chcp 65001"); // 设置控制台编码为UTF-8 防止乱码
    system("cls"); // 清屏
    #endif

    std::cout << "LR Parser" << std::endl;
}

int main(){
    config();

    Grammar grammar;
    grammarInit(grammar);
    grammar.print();

    viablePrefix_DFA dfa(grammar);
    dfa.print();

    return 0;
}