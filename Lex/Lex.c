//
// Created by Lenovo on 2021/5/15.
//
#include "stdio.h"
#include "string.h"
#include "malloc.h"

#define MAX_LEN 16

// plus an ending character '\0'
char token[MAX_LEN + 1];
int len_token = 0;
int line = 1;
char c;
int used = 1;

char get_char() {
    if (used) {
        c = getchar();
        used = 0;
    }
    return c;
}

int exceeded = 0;
int concat(char ch) {
    if (len_token == MAX_LEN) {
        // report exceed error only once
        if (!exceeded) {
            exceeded = 1;
            fprintf(stderr, "***Line:%d  Variable \"%s~\" length exceeded\n", line, token);
        }
        used = 1;
        return 1;
    }
    token[len_token++] = ch;
    token[len_token] = '\0';
    used = 1;
    return 0;
}

int letter(char c) {
    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) return 1;
    else return 0;
}

int digit(char c) {
    if (c >= '0' && c <= '9') return 1;
    else return 0;
}

int number() {
    for (int i = 0; i < len_token; i++) {
        if (!digit(token[i])) return 0;
    }
    return 1;
}

// check token id in the reserved words
// if token is not a reserved word, return 0
int reserve() {
    char* reserved[] = {
            "\0",
            "begin", "end", "integer",
            "if", "then", "else",
            "function", "read", "write",
            "\0", "\0", "=",
            "<>", "<=", "<",
            ">=", ">", "-",
            "*", ":=", "(",
            ")", ";"
    };
    int i;
    for (i = 0; i < 24; i++) {
        if (!strcmp(token, reserved[i])) break;
    }
    if (i == 24) i = 0;
    return i;
}

int accept() {
    int type = reserve();
    if (type == 0) {
        if (number()) type = 11;
        else type = 10;
    }
    fprintf(stdout, "%*s %2d\n", MAX_LEN, token, type);
    len_token = 0;
    token[0] = '\0';
    exceeded = 0;
    return 0;
}

int main(int argc, char* argv[]) {
    if (argc > 1) {
        char* filename = (char *) malloc(strlen(argv[1]) + 1 + 3 + 1);

        strcpy(filename, argv[1]);
        strcat(filename, ".pas");
        freopen(filename, "r", stdin);

        strcpy(filename, argv[1]);
        strcat(filename, ".dyd");
        freopen(filename, "w", stdout);

        strcpy(filename, argv[1]);
        strcat(filename, ".err");
        freopen(filename, "w", stderr);
    }
    else {
        freopen("source.pas", "r", stdin);
        freopen("source.dyd", "w", stdout);
        freopen("source.err", "w", stderr);
    }

    // process a word during a loop
    // the entry is state 0 (always)
    while (1) {
        c = get_char();
        if (c == EOF) break;

        s0:
        if (c == ' ' || c == '\t') {
            used = 1;
            continue;
        }
        else if (letter(c)) {
            concat(c);
            goto s1;
        }
        else if (digit(c)) {
            concat(c);
            goto s3;
        }
        switch (c) {
            case '=': concat(c); goto s5;
            case '-': concat(c); goto s6;
            case '*': concat(c); goto s7;
            case '(': concat(c); goto s8;
            case ')': concat(c); goto s9;
            case '<': concat(c); goto s10;
            case '>': concat(c); goto s14;
            case ':': concat(c); goto s17;
            case ';': concat(c); goto s20;
            default: goto s21;
        }

        s1:
        c = get_char();
        if (letter(c) || digit(c)) {
            concat(c);
            goto s1;
        }
        else goto s2;

        s2:
        accept();
        continue;

        s3:
        c = get_char();
        if (digit(c)) {
            concat(c);
            goto s3;
        }
        else goto s4;

        s4:
        accept();
        continue;

        s5:
        accept();
        continue;

        s6:
        accept();
        continue;

        s7:
        accept();
        continue;

        s8:
        accept();
        continue;

        s9:
        accept();
        continue;

        s10:
        c = get_char();
        if (c == '=') {
            concat(c);
            goto s11;
        }
        if (c == '>') {
            concat(c);
            goto s12;
        }
        goto s13;

        s11:
        accept();
        continue;

        s12:
        accept();
        continue;

        s13:
        accept();
        continue;

        s14:
        c = get_char();
        if (c == '=') {
            concat(c);
            goto s15;
        }
        goto s16;

        s15:
        accept();
        continue;

        s16:
        accept();
        continue;

        s17:
        c = get_char();
        if (c == '=') {
            concat(c);
            goto s18;
        }
        goto s19;

        s18:
        accept();
        continue;

        s19:
        fprintf(stderr, "***Line:%d  Char \"%c\" mismatch after \":\"\n", line, c);
        len_token = 0;
        token[0] = '\0';
        continue;

        s20:
        accept();
        continue;

        s21:
        if (c == '\n') {
            fprintf(stdout, "%*s %2d\n", MAX_LEN, "EOLN", 24);
            line++;
            used = 1;
        }
        else {
            fprintf(stderr, "***Line:%d  Invalid symbol \"%c\"\n", line, c);
            used = 1;
        }
    }
    fprintf(stdout, "%*s %2d\n", MAX_LEN, "EOF", 25);
    fclose(stdin);
    fclose(stdout);
    fclose(stderr);
    return 0;
}