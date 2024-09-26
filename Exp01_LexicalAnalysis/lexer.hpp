#ifndef LEXER_HPP
#define LEXER_HPP

#include <iostream>
#include <string>
#include <vector>
#include <tuple>

#include "token.hpp"

class Error{
    private:
        std::vector<std::tuple<int, int, std::string>> error_position;  // 记录错误位置 {行号, 列号, 错误信息}

    public:
        void addError(int line, int column, std::string error_message){
            error_position.push_back(std::make_tuple(line, column, error_message));
        }

        void print(){
            if(error_position.size() == 0){
                std::cout << "词法分析无错误!" << std::endl;
                return;
            }

            std::cout << "错误信息如下: " << std::endl;
            for(auto error : error_position){
                std::cout << "错误位置: " << std::get<0>(error) << "行" << std::get<1>(error) << "列" << " 发生在词法 " << std::get<2>(error) << std::endl;
            }
        }
};


class Lexer{
    private:
        bool in_comment = false;  // 是否在跨行/* */注释中
        int lbrace;  // 记录{的数量. 如果有}匹配则减1
    public:
        Lexer(){
            in_comment = false;
            lbrace = 0;
        }
        std::vector<Token> tokenize(std::string line, int line_number, Error &error);

        void final_check(Error &error, int errorLine){
            if(in_comment){
                error.addError(errorLine, -1, "注释未结束");
            }
            while(lbrace > 0){
                error.addError(errorLine, -1, "缺少}");
                lbrace--;
            }
        }
};

#endif