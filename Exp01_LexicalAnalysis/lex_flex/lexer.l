%{
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>

    enum TokenType {
        Identifier = 1, Number, String, Char, Keyword, Operator, Delimiters
    };

    enum DEL_Type {
        LBRACE = 8,  // {
        RBRACE,      // }
        LBRACKET,    // [
        RBRACKET,    // ]
        LPAREN,      // (
        RPAREN,      // )
        SEMICOLON,   // ;
        COMMA        // ,
    };

    enum OP_Type {
        ADD = 16,     // +
        SUB,          // -
        MUL,          // *
        DIV,          // /
        MOD,          // %
        ASSIGN,       // =
        ADD_ASS,      // +=
        SUB_ASS,      // -=
        MUL_ASS,      // *=
        DIV_ASS,      // /=
        MOD_ASS,      // %=
        EQ,           // ==
        NE,           // !=
        LT,           // <
        GT,           // >
        LE,           // <=
        GE,           // >=
        AND,          // &&
        OR,           // ||
        BIT_AND,      // &
        BIT_OR,       // |
        BIT_XOR,      // ^
        BIT_NOT,      // ~
        BIT_AND_ASS,  // &=
        BIT_OR_ASS,   // |=
        BIT_XOR_ASS,  // ^=
        LSHIFT,       // <<
        RSHIFT,       // >>
        LSHIFT_ASS,   // <<=
        RSHIFT_ASS,   // >>=
        INC,          // ++
        DEC,          // --
        PTR_ACCESS,   // ->
        NOT,          // !
        DOT           // .
    };

    extern FILE *yyin;

    char file_path[40] = "./example.txt";
    const char* out_path = "./output.txt";
    FILE *output;

    int numLines = 1;         // 语句行数
    int thisLine_Chars = 1;
    int numChars = 0;         // 字符数
    int numKeywords[8] = {0}; // 各类单词的个数
%}

delim  [ \t\n]
ws     {delim}+
letter [a-zA-Z]
digit  [0-9]

ckeyw  "auto"|"break"|"case"|"char"|"const"|"continue"|"default"|"do"|"double"|"else"|"enum"|"extern"|"float"|"for"|"goto"|"if"|"inline"|"int"|"long"|"register"|"return"|"short"|"signed"|"sizeof"|"static"|"struct"|"switch"|"typedef"|"union"|"unsigned"|"void"|"volatile"|"while"
ident  [a-zA-Z_][a-zA-Z0-9_]* 

int10  [1-9][0-9]*
int16  0x[a-fA-F0-9]+
int8   0[0-7]*
float  [0-9]*(\.{int10})?([eE][+-]?{int10})?
num    {int10}|{int16}|{int8}|{float}

comment1 \/\*[^*]*\*+([^/*][^*]*\*+)*\/
comment2 \/\/[^\n]*
comment  {comment1}|{comment2}

string \"([^\\\"]|\\.)*\"
char   \'([^\\\']|\\.)*\'

%%
"\n"      { numLines++; thisLine_Chars = 1; return -9; }
{ws}      { numChars+=strlen(yytext); thisLine_Chars+=strlen(yytext);  return -8; }
{comment} { }
{ckeyw}   { fprintf(output, "Keyword: %s\n", yytext); numChars+=strlen(yytext); thisLine_Chars+=strlen(yytext);  numKeywords[Keyword]++; return Keyword; }
{ident}   { fprintf(output, "Identifier: %s\n", yytext); numChars+=strlen(yytext); thisLine_Chars+=strlen(yytext);  numKeywords[Identifier]++; return Identifier; }
{num}     { fprintf(output, "Number: %s\n", yytext); numChars+=strlen(yytext); thisLine_Chars+=strlen(yytext);  numKeywords[Number]++; return Number; }
{string}  { fprintf(output, "String: %s\n", yytext); numChars+=strlen(yytext); thisLine_Chars+=strlen(yytext);  numKeywords[String]++; return String; }
{char}    { fprintf(output, "Char: %s\n", yytext); numChars+=strlen(yytext); thisLine_Chars+=strlen(yytext);  numKeywords[Char]++; return Char; }

"!="      { fprintf(output, "Operator: !=\n"); numChars+=strlen(yytext); thisLine_Chars+=strlen(yytext);  numKeywords[Operator]++; return NE; }
"!"       { fprintf(output, "Operator: !\n"); numChars+=strlen(yytext); thisLine_Chars+=strlen(yytext);  numKeywords[Operator]++; return NOT; }
"=="      { fprintf(output, "Operator: ==\n"); numChars+=strlen(yytext); thisLine_Chars+=strlen(yytext);  numKeywords[Operator]++; return EQ; }
"="       { fprintf(output, "Operator: =\n"); numChars+=strlen(yytext); thisLine_Chars+=strlen(yytext);  numKeywords[Operator]++; return ASSIGN; }
"<="      { fprintf(output, "Operator: <=\n"); numChars+=strlen(yytext); thisLine_Chars+=strlen(yytext);  numKeywords[Operator]++; return LE; }
">="      { fprintf(output, "Operator: >=\n"); numChars+=strlen(yytext); thisLine_Chars+=strlen(yytext);  numKeywords[Operator]++; return GE; }
"<"       { fprintf(output, "Operator: <\n"); numChars+=strlen(yytext); thisLine_Chars+=strlen(yytext);  numKeywords[Operator]++; return LT; }
">"       { fprintf(output, "Operator: >\n"); numChars+=strlen(yytext); thisLine_Chars+=strlen(yytext);  numKeywords[Operator]++; return GT; }
"<<="     { fprintf(output, "Operator: <<= \n"); numChars+=strlen(yytext); thisLine_Chars+=strlen(yytext);  numKeywords[Operator]++; return LSHIFT_ASS; }
">>="     { fprintf(output, "Operator: >>= \n"); numChars+=strlen(yytext); thisLine_Chars+=strlen(yytext);  numKeywords[Operator]++; return RSHIFT_ASS; }
"<<"      { fprintf(output, "Operator: <<\n"); numChars+=strlen(yytext); thisLine_Chars+=strlen(yytext);  numKeywords[Operator]++; return LSHIFT; }
">>"      { fprintf(output, "Operator: >>\n"); numChars+=strlen(yytext); thisLine_Chars+=strlen(yytext);  numKeywords[Operator]++; return RSHIFT; }
"++"      { fprintf(output, "Operator: ++\n"); numChars+=strlen(yytext); thisLine_Chars+=strlen(yytext);  numKeywords[Operator]++; return INC; }
"+="      { fprintf(output, "Operator: +=\n"); numChars+=strlen(yytext); thisLine_Chars+=strlen(yytext);  numKeywords[Operator]++; return ADD_ASS; }
"+"       { fprintf(output, "Operator: +\n"); numChars+=strlen(yytext); thisLine_Chars+=strlen(yytext);  numKeywords[Operator]++; return ADD; }
"--"      { fprintf(output, "Operator: --\n"); numChars+=strlen(yytext); thisLine_Chars+=strlen(yytext);  numKeywords[Operator]++; return DEC; }
"-="      { fprintf(output, "Operator: -=\n"); numChars+=strlen(yytext); thisLine_Chars+=strlen(yytext);  numKeywords[Operator]++; return SUB_ASS; }
"->"      { fprintf(output, "Operator: ->\n"); numChars+=strlen(yytext); thisLine_Chars+=strlen(yytext);  numKeywords[Operator]++; return PTR_ACCESS; }
"-"       { fprintf(output, "Operator: -\n"); numChars+=strlen(yytext); thisLine_Chars+=strlen(yytext);  numKeywords[Operator]++; return SUB; }
"%="      { fprintf(output, "Operator: %%=\n"); numChars+=strlen(yytext); thisLine_Chars+=strlen(yytext);  numKeywords[Operator]++; return MOD_ASS; }
"%"       { fprintf(output, "Operator: %%\n"); numChars+=strlen(yytext); thisLine_Chars+=strlen(yytext);  numKeywords[Operator]++; return MOD; }
"&="      { fprintf(output, "Operator: &=\n"); numChars+=strlen(yytext); thisLine_Chars+=strlen(yytext);  numKeywords[Operator]++; return BIT_AND_ASS; }
"&&"      { fprintf(output, "Operator: &&\n"); numChars+=strlen(yytext); thisLine_Chars+=strlen(yytext);  numKeywords[Operator]++; return AND; }
"&"       { fprintf(output, "Operator: &\n"); numChars+=strlen(yytext); thisLine_Chars+=strlen(yytext);  numKeywords[Operator]++; return BIT_AND; }
"|="      { fprintf(output, "Operator: |=\n"); numChars+=strlen(yytext); thisLine_Chars+=strlen(yytext);  numKeywords[Operator]++; return BIT_OR_ASS; }
"||"      { fprintf(output, "Operator: ||\n"); numChars+=strlen(yytext); thisLine_Chars+=strlen(yytext);  numKeywords[Operator]++; return OR; }
"|"       { fprintf(output, "Operator: |\n"); numChars+=strlen(yytext); thisLine_Chars+=strlen(yytext);  numKeywords[Operator]++; return BIT_OR; }
"^="      { fprintf(output, "Operator: ^=\n"); numChars+=strlen(yytext); thisLine_Chars+=strlen(yytext);  numKeywords[Operator]++; return BIT_XOR_ASS; }
"^"       { fprintf(output, "Operator: ^\n"); numChars+=strlen(yytext); thisLine_Chars+=strlen(yytext);  numKeywords[Operator]++; return BIT_XOR; }
"~"       { fprintf(output, "Operator: ~\n"); numChars+=strlen(yytext); thisLine_Chars+=strlen(yytext);  numKeywords[Operator]++; return BIT_NOT; }
"*="      { fprintf(output, "Operator: *=\n"); numChars+=strlen(yytext); thisLine_Chars+=strlen(yytext);  numKeywords[Operator]++; return MUL_ASS; }
"*"       { fprintf(output, "Operator: *\n"); numChars+=strlen(yytext); thisLine_Chars+=strlen(yytext);  numKeywords[Operator]++; return MUL; }
"/="      { fprintf(output, "Operator: /=\n"); numChars+=strlen(yytext); thisLine_Chars+=strlen(yytext);  numKeywords[Operator]++; return DIV_ASS; }
"/"       { fprintf(output, "Operator: /\n"); numChars+=strlen(yytext); thisLine_Chars+=strlen(yytext);  numKeywords[Operator]++; return DIV; }
"."       { fprintf(output, "Operator: .\n"); numChars+=strlen(yytext); thisLine_Chars+=strlen(yytext);  numKeywords[Operator]++; return DOT; }

"{"       { fprintf(output, "Delimiters: {\n"); numChars+=strlen(yytext); thisLine_Chars+=strlen(yytext);  numKeywords[Delimiters]++; return LBRACE; }
"}"       { fprintf(output, "Delimiters: }\n"); numChars+=strlen(yytext); thisLine_Chars+=strlen(yytext);  numKeywords[Delimiters]++; return RBRACE; }
"["       { fprintf(output, "Delimiters: [\n"); numChars+=strlen(yytext); thisLine_Chars+=strlen(yytext);  numKeywords[Delimiters]++; return LBRACKET; }
"]"       { fprintf(output, "Delimiters: ]\n"); numChars+=strlen(yytext); thisLine_Chars+=strlen(yytext);  numKeywords[Delimiters]++; return RBRACKET; }
"("       { fprintf(output, "Delimiters: (\n"); numChars+=strlen(yytext); thisLine_Chars+=strlen(yytext);  numKeywords[Delimiters]++; return LPAREN; }
")"       { fprintf(output, "Delimiters: )\n"); numChars+=strlen(yytext); thisLine_Chars+=strlen(yytext);  numKeywords[Delimiters]++; return RPAREN; }
";"       { fprintf(output, "Delimiters: ;\n"); numChars+=strlen(yytext); thisLine_Chars+=strlen(yytext);  numKeywords[Delimiters]++; return SEMICOLON; }
","       { fprintf(output, "Delimiters: ,\n"); numChars+=strlen(yytext); thisLine_Chars+=strlen(yytext);  numKeywords[Delimiters]++; return COMMA; }

.         { fprintf(output, "%s :", yytext); return -1; } // 未匹配的字符返回-1

%%

void config_load(int argc, char **argv){
    if(argc > 1){
        strcpy(file_path, argv[1]);
    }
    return;
}

int main(int argc, char **argv){
    config_load(argc, argv);
    FILE *input = fopen(file_path, "r");
    if(!input){ fprintf(output, "ERROR in file path:%s\n", file_path); return 1; }
    output = fopen(out_path, "w");
    if(!output){ fprintf(output, "ERROR in out path:%s\n", out_path); return 1; }

    yyin = input;
    while(1){
        int return_num = yylex();
        if (return_num == 0)  // lex EOF
            break;
        else if (return_num == -1){
            fprintf(output, "ERROR in Line %d Char %d : Undefined matching pattern\n", numLines, thisLine_Chars);
        }
        else{
            ;
        }
    }
    fprintf(output, "\n");

    fprintf(output, "Line Numbers: %d\n", numLines);
    fprintf(output, "Char Numbers: %d\n", numChars);
    fprintf(output, "Token Numbers\n");
    fprintf(output, "\tIdentifier: %d\n", numKeywords[Identifier]);
    fprintf(output, "\tNumber: %d\n", numKeywords[Number]);
    fprintf(output, "\tString: %d\n", numKeywords[String]);
    fprintf(output, "\tChar: %d\n", numKeywords[Char]);
    fprintf(output, "\tKeyword: %d\n", numKeywords[Keyword]);
    fprintf(output, "\tOperator: %d\n", numKeywords[Operator]);
    fprintf(output, "\tDelimiters: %d\n", numKeywords[Delimiters]);

    fclose(input);
    fclose(output);
    return 0;
}
