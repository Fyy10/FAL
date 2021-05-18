%{
#include "stdio.h"

#define BG 1
#define END 2
#define INTEGER 3
#define IF 4
#define ELSE 5
#define THEN 6
#define FUNCTION 7
#define READ 8
#define WRITE 9
#define ID 10
#define CONST 11
#define EQU 12
#define NEQ 13
#define LTEQ 14
#define LT 15
#define GTEQ 16
#define GT 17
#define SUB 18
#define MUL 19
#define ASSIGN 20
#define LP 21
#define RP 22
#define SC 23
#define EOLN 24

// enum {
//     BG = 1, END, INTEGER,
//     IF, ELSE, THEN,
//     FUNCTION, READ, WRITE,
//     ID, CONST, EQU,
//     NEQ, LTEQ, LT,
//     GTEQ, GT, SUB,
//     MUL, ASSIGN, LP,
//     // semicolon
//     RP, SC
// };

int yywrap(void) {
    return 1;
}

%}

%%

"begin" return BG;
"end" return END;
"integer" return INTEGER;
"if" return IF;
"else" return ELSE;
"then" return THEN;
"function" return FUNCTION;
"read" return READ;
"write" return WRITE;
"=" return EQU;
"<>" return NEQ;
"<=" return LTEQ;
"<" return LT;
">=" return GTEQ;
">" return GT;
"-" return SUB;
"*" return MUL;
":=" return ASSIGN;
"(" return LP;
")" return RP;
";" return SC;
"\n" return EOLN;
<<EOF>> return EOF;
[a-zA-Z]+[0-9a-zA-z]* return ID;
[0-9]+ return CONST;
[ \t] ;
. printf("Invalid symbol \"%c\"\n", *yytext);

%%

#define MAX_LEN 16

int main() {
    while (1) {
        int id;
        id = yylex();
        if (id == EOLN) {
            printf("%*s %2d\n", MAX_LEN, "EOLN", EOLN);
        }
        else if (id == EOF) {
            printf("%*s %2d\n", MAX_LEN, "EOF", 25);
            break;
        }
        else printf("%*s %2d\n", MAX_LEN, yytext, id);
    }
    return 0;
}