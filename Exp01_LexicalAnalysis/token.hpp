#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <string>
#include <vector>

enum class Type{
    Identifier, // 标识符
    Number,  // 数字常量
    String,  // 字符串常量
    Char,    // 字符常量
    Keyword, // 关键字
    Operator, // 运算符
    Delimiters // 分隔符
};

class Token{
    private:
        Type type;
        std::string property;

    public:
        Token(std::string original, Type type){
            this->type = type;
            this->property = original;
        }

        Type get_type(){
            return this->type;
        }

        std::string output(){
            std::string type_str;
            switch(type){
                case Type::Identifier:
                    type_str = "Identifier";
                    break;
                case Type::Number:
                    type_str = "Number\t";
                    break;
                case Type::String:
                    type_str = "String\t";
                    break;
                case Type::Char:
                    type_str = "Char\t";
                    break;
                case Type::Keyword:
                    type_str = "Keyword";
                    break;
                case Type::Operator:
                    type_str = "Operator";
                    break;
                case Type::Delimiters:
                    type_str = "Delimiters";
                    break;
            }
            return "\t " + type_str + "\t|\t" + property;
        }
};

#endif