//
// Created by Lenovo on 2021/5/15.
//
#include "stdio.h"
#include "string.h"

#define MAX_LEN 16

char token[MAX_LEN + 1];
int len_token = 0;
int line = 0;
int used = 1;

char get_char(char c) {
    if (used) c = getchar();
    used = 0;
    return c;
}

int concat(char ch) {
    if (len_token == MAX_LEN) return 1;
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
    return 0;
}

int main() {
    freopen("source.pas", "r", stdin);
    freopen("source.dyd", "w", stdout);
    freopen("source.err", "w", stderr);
    char c;
    // process a word during a loop
    // the entry is state 0 (always)
    while (1) {
        c = get_char(c);
        if (c == EOF) break;

        s0:
        if (c == ' ' || c == '\t') continue;
        else if (letter(c)) {
            // todo: check error
            concat(c);
            goto s1;
        }
        else if (digit(c)) {
            concat(c);
            goto s3;
        }
        switch (c) {
            case '=': concat(c); goto s5; break;
            case '-': concat(c); goto s6; break;
            case '*': concat(c); goto s7; break;
            case '(': concat(c); goto s8; break;
            case ')': concat(c); goto s9; break;
            case '<': concat(c); goto s10; break;
            case '>': concat(c); goto s14; break;
            case ':': concat(c); goto s17; break;
            case ';': concat(c); goto s20; break;
            default: goto s21;
        }

        s1:
        c = get_char(c);
        if (c == EOF) break;
        if (letter(c) || digit(c)) {
            concat(c);
            goto s1;
        }
        else {
            concat(c);
            goto s2;
        }

        s2:
        accept();
        continue;

        s3:
        c = get_char(c);
        if (c == EOF) break;
        if (digit(c)) {
            concat(c);
            goto s3;
        }
        else {
            concat(c);
            goto s4;
        }

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
        c = get_char(c);
        if (c == EOF) break;
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
        c = get_char(c);
        if (c == EOF) break;
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
        c = get_char(c);
        if (c == EOF) break;
        if (c == '=') {
            concat(c);
            goto s18;
        }
        goto s19;

        s18:
        accept();
        continue;

        s19:
        accept();
        continue;

        s20:
        accept();
        continue;

        s21:
        if (c == '\n') {
            line++;
            fprintf(stdout, "%16s %2d\n", "EOLN", 24);
        }
        else fprintf(stderr, "***Line:%d  invalid symbol %c\n", line, c);
    }
    fprintf(stdout, "%*s %2d\n", MAX_LEN, "EOF", 25);
    fclose(stdin);
    fclose(stdout);
    fclose(stderr);
    return 0;
}