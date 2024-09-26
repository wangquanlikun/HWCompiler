#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include <vector>

#ifdef _WIN32
#include <Windows.h>
#endif

#include "token.hpp"
#include "lexer.hpp"

class Statistic{
    private:
        int numLines; // 语句行数
        int numWords; // 单词数
        int numChars; // 字符数

    public:
        Statistic(){
            numLines = 0;
            numWords = 0;
            numChars = 0;
        }

        void addLine(){
            numLines++;
        }
        int getLines(){
            return numLines;
        }
        void addWord(std::vector<Token> tokens){
            numWords += tokens.size();
        }
        void addChar(std::string line){
            numChars += line.size();
        }

        void print(){
            std::cout << "统计结果如下: " << std::endl;
            std::cout << "语句行数: " << numLines << "\t";
            std::cout << "单词数: " << numWords << "\t";
            std::cout << "字符数: " << numChars << std::endl;
        }
};

static std::string file_path = "example.txt";

void load_config(int argc, char* argv[]){
    #ifdef _WIN32
    system("chcp 65001"); // 设置控制台编码为UTF-8
    system("cls"); // 清屏
    #endif
    for(int parameter_index = 1; parameter_index < argc; parameter_index++){
        if(strcmp(argv[parameter_index], "-p") == 0){
            file_path = argv[++parameter_index];
        }
    }

    std::cout << "词法分析程序的的设计与实现" << std::endl;
    std::cout << "使用方法: lexer [-p <file_path>]" << std::endl;
    std::cout << "本程序具有的功能如下: " << std::endl;
    std::cout << "1. 统计源程序中的语句行数、各类单词的个数、以及字符总数，并输出统计结果" << std::endl;
    std::cout << "2. 识别出用源语言编写的源程序中的每个单词符号，并以记号的形式输出每个单词符号" << std::endl;
    std::cout << "3. 检查源程序中存在的词法错误，并报告错误所在的位置" << std::endl;
    std::cout << "------------------------  记号 | 属性  ------------------------" << std::endl;
}

int main(int argc, char* argv[]){
    load_config(argc, argv);
    std::ifstream file(file_path);
    if(!file.is_open()){ std::cerr << "File not open!" << std::endl; return 1; }

    Statistic statistic;
    Lexer lexer;
    Error errors;

    std::string line;
    while(std::getline(file, line)){
        statistic.addLine();
        statistic.addChar(line);
        std::vector<Token> tokens = lexer.tokenize(line, statistic.getLines(), errors);
        statistic.addWord(tokens);

        if(tokens.size() != 0)
            std::cout << statistic.getLines();
        for(auto token : tokens){
            std::cout << token.output() << std::endl;
        }
    }
    lexer.final_check(errors, statistic.getLines());

    statistic.print();
    errors.print();

    file.close();
    return 0;
}