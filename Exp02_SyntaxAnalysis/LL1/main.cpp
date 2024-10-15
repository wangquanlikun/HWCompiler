#include "grammar.hpp"
#include "parser.hpp"
#include <iostream>

void config(){
    #ifdef _WIN32
    system("chcp 65001"); // 设置控制台编码为UTF-8 防止乱码
    system("cls"); // 清屏
    #endif

    std::cout << "LL(1) Parser" << std::endl;
}

int main(){
    config();
    
    Grammar grammar;
    grammarSet(grammar);
    grammar.print();

    LL1Table table(grammar);
    table.print();

    LL1Parser parser(table);
    parser.parse();

    return 0;
}