%{
#include "stdio.h"

#define BG 1
#define END 2
#define INTEGER 3
#define IF 4
#define THEN 5
#define ELSE 6
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

int line = 1;

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
:[^=] {
    fprintf(stderr, "***Line:%d  Symbol \"%c\" mismatch after \":\"\n", line, yytext[1]);
    yyless(1);
}
":=" return ASSIGN;
"(" return LP;
")" return RP;
";" return SC;
"\n" {
    line++;
    return EOLN;
}
<<EOF>> return EOF;
[a-zA-Z][0-9a-zA-z]{0,15} return ID;
[a-zA-Z][0-9a-zA-z]{16,} fprintf(stderr, "***Line:%d  Variable \"%s\" length exceeded\n", line, yytext);
[0-9]+ return CONST;
[ \t] ;
. fprintf(stderr, "***Line:%d  Invalid symbol \"%c\"\n", line, *yytext);

%%

#define MAX_LEN 16

int main(int argc, char* argv[]) {
    if (argc > 1) {
        char* filename = (char *) malloc(strlen(argv[1]) + 5 + 1 + 3 + 1);

        strcpy(filename, argv[1]);
        strcat(filename, ".pas");
        freopen(filename, "r", stdin);

        strcpy(filename, argv[1]);
        strcat(filename, "_flex.dyd");
        freopen(filename, "w", stdout);

        strcpy(filename, argv[1]);
        strcat(filename, "_flex.err");
        freopen(filename, "w", stderr);
    }
    else {
        freopen("source.pas", "r", stdin);
        freopen("source_flex.dyd", "w", stdout);
        freopen("source_flex.err", "w", stderr);
    }

    while (1) {
        int id;
        id = yylex();
        if (id == EOLN) {
            fprintf(stdout, "%*s %2d\n", MAX_LEN, "EOLN", EOLN);
        }
        else if (id == EOF) {
            fprintf(stdout, "%*s %2d\n", MAX_LEN, "EOF", 25);
            break;
        }
        else fprintf(stdout, "%*s %2d\n", MAX_LEN, yytext, id);
    }
    fclose(stdin);
    fclose(stdout);
    fclose(stderr);
    return 0;
}