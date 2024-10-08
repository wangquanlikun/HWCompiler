#include "lexer.hpp"
#include "token.hpp"

enum class State{
    START,
    IN_WORD,  // 标识符 关键字
    IN_STRING,  // 字符串
    IN_NUM,  // 整形/浮点型数字常量
    IN_COMMENT  // 注释中
};

enum class NUM_State{
    NONE, START, DECIMAL, OCTAL, HEX_PREFIX, HEXADECIMAL, FLOAT, FLOAT_POINT, EXPONENT, END, ERROR
};

bool is_a2z(char ch){
    return (ch >= 'a' && ch <= 'z');
}
bool is_A2Z(char ch){
    return (ch >= 'A' && ch <= 'Z');
}
bool is_0to9(char ch){
    return (ch >= '0' && ch <= '9');
}
bool isHexDigit(char c) {
    return std::isdigit(c) || (std::tolower(c) >= 'a' && std::tolower(c) <= 'f');
}

bool is_keykord(std::string word){
    std::vector<std::string> keywords = {
        "auto", "break", "case", "char", "const", "continue", "default", "do", "double", "else",
        "enum", "extern", "float", "for", "goto", "if", "int", "long", "register", "return",
        "short", "signed", "sizeof", "static", "struct", "switch", "typedef", "union", "unsigned",
        "void", "volatile", "while"
    };
    for(auto keyword : keywords){
        if(word == keyword){
            return true;
        }
    }
    return false;
}


std::vector<Token> Lexer::tokenize(std::string line, int line_number, Error &error){
    std::vector<Token> tokens;
    if(in_comment){
        size_t comment_end = line.find("*/");
        if(comment_end == std::string::npos){
            return tokens;
        }
        in_comment = false;
        line = line.substr(comment_end + 2);  // 从注释结束后开始继续分析
    }

    int line_length = line.size();

    State state = State::START;
    NUM_State num_state = NUM_State::NONE;
    int tempState = -1;
    Stake stake;

    std::string temp_token;

    int i = 0;
    while(i < line_length){
        char ch = line[i];

        switch (state){
            case State::START:
                if(is_a2z(ch) || is_A2Z(ch) || ch == '_'){
                    state = State::IN_WORD;
                    temp_token += ch;
                    i++;
                }
                else if(ch == '('){
                    stake.push(ch);
                    Token new_token("(", Type::Delimiters);
                    tokens.push_back(new_token);
                    i++;
                }
                else if(ch == '['){
                    stake.push(ch);
                    Token new_token("[", Type::Delimiters);
                    tokens.push_back(new_token);
                    i++;
                }
                else if(ch == '{'){
                    lbrace++;
                    Token new_token("{", Type::Delimiters);
                    tokens.push_back(new_token);
                    i++;
                }
                else if(ch == ')'){
                    if(stake.check(')')){
                        Token new_token(")", Type::Delimiters);
                        tokens.push_back(new_token);
                        i++;
                    }
                    else{
                        error.addError(line_number, i, "括号 ) 匹配错误");
                        i++;
                    }
                }
                else if(ch == ']'){
                    if(stake.check(']')){
                        Token new_token("]", Type::Delimiters);
                        tokens.push_back(new_token);
                        i++;
                    }
                    else{
                        error.addError(line_number, i, "括号 ] 匹配错误");
                        i++;
                    }
                }
                else if(ch == '}'){
                    if(lbrace > 0){
                        lbrace--;
                        Token new_token("}", Type::Delimiters);
                        tokens.push_back(new_token);
                        i++;
                    }
                    else{
                        error.addError(line_number, i, "括号 } 匹配错误");
                        i++;
                    }
                }
                else if(ch == ';'){
                    Token new_token(";", Type::Delimiters);
                    tokens.push_back(new_token);
                    i++;
                }
                else if(ch == ','){
                    Token new_token(",", Type::Delimiters);
                    tokens.push_back(new_token);
                    i++;
                }
                else if(ch == ' ' || ch == '\t'){
                    i++;
                }
                else if(ch == '#'){
                    if(i == 0){ //预处理指令
                        return tokens;
                    }
                    else{
                        error.addError(line_number, i, "不合法的输入字符 #");
                        i++;
                    }
                }
                else if(ch == '!'){
                    if(i + 1 < line_length && line[i + 1] == '='){
                        Token new_token("!=", Type::Operator);
                        tokens.push_back(new_token);
                        i += 2;
                    }
                    else{
                        Token new_token("!", Type::Operator);
                        tokens.push_back(new_token);
                        i++;
                    }
                }
                else if(ch == '='){
                    if(i + 1 < line_length && line[i + 1] == '='){
                        Token new_token("==", Type::Operator);
                        tokens.push_back(new_token);
                        i += 2;
                    }
                    else{
                        Token new_token("=", Type::Operator);
                        tokens.push_back(new_token);
                        i++;
                    }
                }
                else if(ch == '<'){
                    if(i + 1 < line_length && line[i + 1] == '='){
                        Token new_token("<=", Type::Operator);
                        tokens.push_back(new_token);
                        i += 2;
                    }
                    else if(i + 1 < line_length && line[i + 1] == '<'){
                        Token new_token("<<", Type::Operator);
                        tokens.push_back(new_token);
                        i += 2;
                    }
                    else{
                        Token new_token("<", Type::Operator);
                        tokens.push_back(new_token);
                        i++;
                    }
                }
                else if(ch == '>'){
                    if(i + 1 < line_length && line[i + 1] == '='){
                        Token new_token(">=", Type::Operator);
                        tokens.push_back(new_token);
                        i += 2;
                    }
                    else if(i + 1 < line_length && line[i + 1] == '>'){
                        Token new_token(">>", Type::Operator);
                        tokens.push_back(new_token);
                        i += 2;
                    }
                    else{
                        Token new_token(">", Type::Operator);
                        tokens.push_back(new_token);
                        i++;
                    }
                }
                else if(ch == '+'){
                    if(i + 1 < line_length && line[i + 1] == '='){
                        Token new_token("+=", Type::Operator);
                        tokens.push_back(new_token);
                        i += 2;
                    }
                    else if(i + 1 < line_length && line[i + 1] == '+'){
                        Token new_token("++", Type::Operator);
                        tokens.push_back(new_token);
                        i += 2;
                    }
                    else{
                        Token new_token("+", Type::Operator);
                        tokens.push_back(new_token);
                        i++;
                    }
                }
                else if(ch == '-'){
                    if(i + 1 < line_length && line[i + 1] == '='){
                        Token new_token("-=", Type::Operator);
                        tokens.push_back(new_token);
                        i += 2;
                    }
                    else if(i + 1 < line_length && line[i + 1] == '-'){
                        Token new_token("--", Type::Operator);
                        tokens.push_back(new_token);
                        i += 2;
                    }
                    else if(i + 1 < line_length && line[i + 1] == '>'){
                        Token new_token("->", Type::Operator);
                        tokens.push_back(new_token);
                        i += 2;
                    }
                    else{
                        Token new_token("-", Type::Operator);
                        tokens.push_back(new_token);
                        i++;
                    }
                }
                else if(ch == '%'){
                    if(i + 1 < line_length && line[i + 1] == '='){
                        Token new_token("%=", Type::Operator);
                        tokens.push_back(new_token);
                        i += 2;
                    }
                    else{
                        Token new_token("%", Type::Operator);
                        tokens.push_back(new_token);
                        i++;
                    }
                }
                else if(ch == '&'){
                    if(i + 1 < line_length && line[i + 1] == '&'){
                        Token new_token("&&", Type::Operator);
                        tokens.push_back(new_token);
                        i += 2;
                    }
                    else if(i + 1 < line_length && line[i + 1] == '='){
                        Token new_token("&=", Type::Operator);
                        tokens.push_back(new_token);
                        i += 2;
                    }
                    else{
                        Token new_token("&", Type::Operator);
                        tokens.push_back(new_token);
                        i++;
                    }
                }
                else if(ch == '|'){
                    if(i + 1 < line_length && line[i + 1] == '|'){
                        Token new_token("||", Type::Operator);
                        tokens.push_back(new_token);
                        i += 2;
                    }
                    else if(i + 1 < line_length && line[i + 1] == '='){
                        Token new_token("|=", Type::Operator);
                        tokens.push_back(new_token);
                        i += 2;
                    }
                    else{
                        Token new_token("|", Type::Operator);
                        tokens.push_back(new_token);
                        i++;
                    }
                }
                else if(ch == '^'){
                    if(i + 1 < line_length && line[i + 1] == '='){
                        Token new_token("^=", Type::Operator);
                        tokens.push_back(new_token);
                        i += 2;
                    }
                    else{
                        Token new_token("^", Type::Operator);
                        tokens.push_back(new_token);
                        i++;
                    }
                }
                else if(ch == '~'){
                    Token new_token("~", Type::Operator);
                    tokens.push_back(new_token);
                    i++;
                }
                else if(ch == '*'){
                    if(i + 1 < line_length && line[i + 1] == '='){
                        Token new_token("*=", Type::Operator);
                        tokens.push_back(new_token);
                        i += 2;
                    }
                    else{
                        Token new_token("*", Type::Operator);
                        tokens.push_back(new_token);
                        i++;
                    }
                }
                else if(ch == '/'){
                    if(i + 1 < line_length && line[i + 1] == '/'){
                        goto END; // 识别到"//"，之后都是注释内容
                    }
                    else if(i + 1 < line_length && line[i + 1] == '*'){
                        this->in_comment = true;
                        state = State::IN_COMMENT;
                        tempState = 0;
                        i += 2;
                    }
                    else if(i + 1 < line_length && line[i + 1] == '='){
                        Token new_token("/=", Type::Operator);
                        tokens.push_back(new_token);
                        i += 2;
                    }
                    else{
                        Token new_token("/", Type::Operator);
                        tokens.push_back(new_token);
                        i++;
                    }
                }
                else if(ch == '.'){
                    Token new_token(".", Type::Operator);
                    tokens.push_back(new_token);
                    i++;
                }
                else if(ch == '?'){
                    Token new_token("?", Type::Operator);
                    tokens.push_back(new_token);
                    i++;
                }
                else if(ch == ':'){
                    Token new_token(":", Type::Operator);
                    tokens.push_back(new_token);
                    i++;
                }
                else if(ch == '"'){
                    temp_token += ch;
                    state = State::IN_STRING;
                    i++;
                }
                else if(ch == '\''){
                    if(i + 3 < line_length){
                        // 同时考虑'a' '\t' 两种情况
                        if(line[i + 1] == '\\'){
                            if(line[i + 3] == '\''){
                                temp_token += ch;
                                temp_token += line[i + 1];
                                temp_token += line[i + 2];
                                temp_token += line[i + 3];
                                if(line[i + 2] == 'n' || line[i + 2] == 't' || line[i + 2] == 'r' || line[i + 2] == '0' || line[i + 2] == '\\' || line[i + 2] == '\'' || line[i + 2] == '\"'){
                                    Token new_token(temp_token, Type::Char);
                                    tokens.push_back(new_token);
                                    temp_token.clear();
                                    i += 4;
                                }
                                else{
                                    temp_token.clear();
                                    error.addError(line_number, i, "字符常量错误: 转义字符错误");
                                    i++;
                                }
                            }
                            else{
                                error.addError(line_number, i, "字符常量错误: 未闭合");
                                i++;
                            }
                        }
                        else{
                            if(line[i + 2] == '\''){
                                temp_token += ch;
                                temp_token += line[i + 1];
                                temp_token += line[i + 2];
                                Token new_token(temp_token, Type::Char);
                                tokens.push_back(new_token);
                                temp_token.clear();
                                i += 3;
                            }
                            else{
                                error.addError(line_number, i, "字符常量错误: 未闭合");
                                i++;
                            }
                        }
                    }
                    else if(i + 2 < line_length){
                        if(line[i + 2] == '\''){
                            temp_token += ch;
                            temp_token += line[i + 1];
                            temp_token += line[i + 2];
                            if(line[i + 1] == '\\'){
                                temp_token.clear();
                                error.addError(line_number, i, "字符常量错误: 未闭合");
                                i++;
                            }
                            else{
                                Token new_token(temp_token, Type::Char);
                                tokens.push_back(new_token);
                                temp_token.clear();
                                i += 3;
                            }
                        }
                        else{
                            error.addError(line_number, i, "字符常量错误: 未闭合");
                            i++;
                        }
                    }
                    else{
                        error.addError(line_number, i, "字符常量错误: 未闭合");
                        i++;
                    }
                }
                else if(is_0to9(ch)){
                    state = State::IN_NUM;
                    num_state = NUM_State::START;
                }
                else if(ch == '\\'){
                    if(i + 1 < line_length){
                        error.addError(line_number, i, "不合法的输入字符 \\");
                        i++;
                    }
                    else{
                        i++; // C语言中可用\换行
                    }
                }
                else{
                    error.addError(line_number, i, "不合法的输入字符 " + std::string(1, ch));
                    i++;
                }
                break;

            case State::IN_WORD:
                if(is_a2z(ch) || is_A2Z(ch) || is_0to9(ch) || ch == '_'){
                    temp_token += ch;
                    i++;
                }
                else{
                    if(is_keykord(temp_token)){
                        Token new_token(temp_token, Type::Keyword);
                        tokens.push_back(new_token);
                        temp_token.clear();
                        state = State::START;
                    }
                    else{
                        Token new_token(temp_token, Type::Identifier);
                        tokens.push_back(new_token);
                        temp_token.clear();
                        state = State::START;
                    }
                }
                break;

            case State::IN_STRING:
                if(ch == '"'){
                    temp_token += ch;
                    Token new_token(temp_token, Type::String);
                    tokens.push_back(new_token);
                    temp_token.clear();
                    state = State::START;
                    i++;
                }
                else if(ch == '\\'){
                    if(i + 1 < line_length){
                        char ch2 = line[i + 1];
                        if(ch2 == 'n' || ch2 == 't' || ch2 == 'r' || ch2 == '0' || ch2 == '\\' || ch2 == '\'' || ch2 == '\"'){
                            temp_token += ch;
                            temp_token += ch2;
                            i += 2;
                        }
                        else{
                            temp_token += ch;
                            error.addError(line_number, i, "字符串常量转义字符错误");
                            i++;
                        }
                    }
                    else{
                        temp_token += ch;
                        error.addError(line_number, i, "字符串常量转义字符错误");
                        i++;
                    }
                }
                else{
                    temp_token += ch;
                    i++;
                }
                break;

            case State::IN_COMMENT:
                if(tempState == 0){
                    if(ch == '*')
                        tempState = 1;
                    i++;
                }
                else if(tempState == 1){
                    if(ch == '*')
                        tempState = 1;
                    else if(ch == '/'){
                        tempState = -1;
                        state = State::START;
                        this->in_comment = false;
                    }
                    else
                        tempState = 0;
                    i++;
                }
                break;

            case State::IN_NUM:
                switch (num_state){
                    case NUM_State::START:
                        temp_token += ch;
                        if(ch == '0'){
                            num_state = NUM_State::OCTAL;   // 可能是八进制、十六进制或者浮点
                        }
                        else{
                            num_state = NUM_State::DECIMAL; // 十进制
                        }
                        i++;
                        break;

                    case NUM_State::DECIMAL:
                        if (std::isdigit(ch)){
                            temp_token += ch;
                            i++;
                        }
                        else if (ch == '.') {
                            temp_token += ch;
                            num_state = NUM_State::FLOAT_POINT;
                            i++;
                        }
                        else if (ch == 'e' || ch == 'E') {
                            temp_token += ch;
                            num_state = NUM_State::EXPONENT;
                            i++;
                        }
                        else{
                            num_state = NUM_State::END;
                        }
                        break;

                    case NUM_State::OCTAL:
                        if (ch >= '0' && ch <= '7'){
                            temp_token += ch;
                            i++;
                        }
                        else if (ch == 'x' || ch == 'X'){
                            temp_token += ch;
                            num_state = NUM_State::HEX_PREFIX;
                            i++;
                        }
                        else if (ch == '.'){
                            temp_token += ch;
                            num_state = NUM_State::FLOAT_POINT;
                            i++;
                        }
                        else if (std::isdigit(ch)){
                            num_state = NUM_State::ERROR;
                        }
                        else{
                            num_state = NUM_State::END;
                        }
                        break;

                    case NUM_State::HEX_PREFIX:
                        if (isHexDigit(ch)){
                            temp_token += ch;
                            num_state = NUM_State::HEXADECIMAL;
                            i++;
                        }
                        else{
                            num_state = NUM_State::ERROR;
                        }
                        break;

                    case NUM_State::HEXADECIMAL:
                        if (isHexDigit(ch)){
                            temp_token += ch;
                            i++;
                        }
                        else{
                            num_state = NUM_State::END;
                        }
                        break;

                    case NUM_State::FLOAT_POINT:
                        if (std::isdigit(ch)){
                            temp_token += ch;
                            num_state = NUM_State::FLOAT;
                            i++;
                        }
                        else{
                            num_state = NUM_State::ERROR;
                        }
                        break;

                    case NUM_State::FLOAT:
                        if (std::isdigit(ch)){
                            temp_token += ch;
                            i++;
                        }
                        else if (ch == 'e' || ch == 'E'){
                            temp_token += ch;
                            num_state = NUM_State::EXPONENT;
                            i++;
                        }
                        else{
                            num_state = NUM_State::END;
                        }
                        break;

                    case NUM_State::EXPONENT:
                        if (std::isdigit(ch) || ch == '+' || ch == '-'){
                            temp_token += ch;
                            i++;
                        }
                        else{
                            num_state = NUM_State::END;
                        }
                        break;

                    case NUM_State::END:
                        if(ch == ' ' || ch == '\t' || ch == '\n' || ch == ';' || ch == ',' || ch == ')' || ch == ']' || ch == '}' || ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '%' || ch == '&' || ch == '|' || ch == '^' || ch == '~' || ch == '<' || ch == '>' || ch == '=' || ch == '!' || ch == '.' || ch == '#' || ch == '\'' || ch == '"'){
                            Token new_token(temp_token, Type::Number);
                            tokens.push_back(new_token);
                            temp_token.clear();
                            num_state = NUM_State::NONE;
                            state = State::START;
                            break;
                        }
                        else if(is_a2z(ch) || is_A2Z(ch) || ch == '_'){
                            error.addError(line_number, i, "错误的标识符格式");
                            temp_token.clear();
                            num_state = NUM_State::NONE;
                            state = State::START;
                            break;
                        }
                        else{
                            error.addError(line_number, i, "数字常量错误");
                            temp_token.clear();
                            num_state = NUM_State::NONE;
                            state = State::START;
                            break;
                        }
                    case NUM_State::ERROR:
                        error.addError(line_number, i, "数字常量错误");
                        temp_token.clear();
                        num_state = NUM_State::NONE;
                        state = State::START;
                        break;
                }
                break;
        }
    }

    END:
    if(state == State::IN_STRING){
        error.addError(line_number, i, "字符串常量缺少右引号 \" 错误");
        temp_token.clear();
    }

    if(temp_token.size() != 0){
        if(state == State::IN_WORD){
            if(is_keykord(temp_token)){
                Token new_token(temp_token, Type::Keyword);
                tokens.push_back(new_token);
            }
            else{
                Token new_token(temp_token, Type::Identifier);
                tokens.push_back(new_token);
            }
        }
        else if(state == State::IN_NUM){
            if(num_state == NUM_State::DECIMAL || num_state == NUM_State::FLOAT || num_state == NUM_State::EXPONENT){
                Token new_token(temp_token, Type::Number);
                tokens.push_back(new_token);
            }
            else{
                error.addError(line_number, i, "数字常量错误");
            }
        }
    }

    while(!stake.empty()){
        if(stake.top() == '('){
            error.addError(line_number, i, "括号 ( 匹配错误");
        }
        else if(stake.top() == '['){
            error.addError(line_number, i, "括号 [ 匹配错误");
        }
        else if(stake.top() == '{'){
            error.addError(line_number, i, "括号 { 匹配错误");
        }
        stake.pop();
    }

    return tokens;
}