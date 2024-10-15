%{
    #include <stdio.h>
    #include <ctype.h>

    int yylex();
    void yyerror(char *s);
%}

%start E
%token NUM

%%
E : E '+' T   { printf("E → E+T\n"); }
  | E '-' T   { printf("E → E-T\n"); }
  | T         { printf("E → T\n"); }
  ;

T : T '*' F   { printf("T → T*F\n"); }
  | T '/' F   { printf("T → T/F\n"); }
  | F         { printf("T → F\n"); }
  ;

F : '(' E ')' { printf("F → (E)\n"); }
  | NUM       { printf("F → num\n"); }
  ;
%%

int main() {
    if (yyparse()){
        printf("Reject!\n");
    } 
    else{
        printf("Accept!\n");
    }
    return 0;
}

void yyerror(char *s) {
    fprintf(stderr, "%s\n", s);
}