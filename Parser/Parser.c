#include "stdio.h"
#include "string.h"
#include "stdlib.h"

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

#define MAX_LEN 16
#define TABLE_LEN 10

struct variable {
    char vname[MAX_LEN + 1];
    char vproc[MAX_LEN + 1];
    int vkind;
    // types: (int)
    char vtype[4];
    int vlev;
    int vadr;
} var_table[TABLE_LEN];
int len_var_table = 0;

struct process {
    char pname[MAX_LEN + 1];
    // types: (int)
    char ptype[4];
    int plev;
    int fadr;
    int ladr;
} proc_table[TABLE_LEN];
int len_proc_table = 0;

int used = 1;
char token[MAX_LEN + 1];
int id;

// to retract
int buff_valid = 0;
char buff_token[MAX_LEN + 1];
int buff_id;

int level = 0, line = 1;
int eof = 0;

int get_next_token();
int get_buff_token();
int print_dys();
int print_buff_dys();

int add_var();
int add_proc();
int search_var();
int search_proc();

int PROG();
int SUBPROG();
int DEF_LIST();
int DEF_LIST_P();
int EXE_LIST();
int EXE_LIST_P();
int DEF();
int VAR();
int ID_G();
int PARAM();
int FUNC();
int EXE();
int READ_G();
int WRITE_G();
int ASSIGN_G();
int COND();
int EXPR();
int EXPR_P();
int TERM();
int TERM_P();
int FACTOR();
int CONST_G();
int CALL();
int COND_EXPR();
int OP();

int main(int argc, char* argv[]) {
    FILE *fvar, *fproc;
    if (argc > 1) {
        char* filename = (char *) malloc(strlen(argv[1]) + 1 + 3 + 1);

        strcpy(filename, argv[1]);
        strcat(filename, ".dyd");
        freopen(filename, "r", stdin);

        strcpy(filename, argv[1]);
        strcat(filename, ".dys");
        freopen(filename, "w", stdout);

        strcpy(filename, argv[1]);
        strcat(filename, ".err");
        freopen(filename, "w", stderr);

        strcpy(filename, argv[1]);
        strcat(filename, ".var");
        fvar = fopen(filename, "w");

        strcpy(filename, argv[1]);
        strcat(filename, ".pro");
        fproc = fopen(filename, "w");
    }
    else {
        freopen("source.dyd", "r", stdin);
        freopen("source.dys", "w", stdout);
        freopen("source.err", "w", stderr);
        fvar = fopen("source.var", "w");
        fproc = fopen("source.pro", "w");
    }

    PROG();
    // the rest should be EOLN and EOF, simply output all
    get_next_token();

    fclose(stdin);
    fclose(stdout);
    fclose(stderr);
    fclose(fvar);
    fclose(fproc);
    return 0;
}

int get_next_token() {
    while (1) {
        if (used && buff_valid) {
            strcpy(token, buff_token);
            id = buff_id;
            buff_valid = 0;
            break;
        }
        else if (used) {
            scanf("%s %d", token, &id);
            used = 0;
        }
        else {
            break;
        }

        if (id == EOLN) {
            print_dys();
            used = 1;
            line++;
            continue;
        }
        if (id == 25) {
            used = 1;
            eof = 1;
        }
        print_dys();
        break;
    }
    return 0;
}

int get_buff_token() {
    while (1) {
        scanf("%s %d", buff_token, &buff_id);
        buff_valid = 1;

        if (buff_id == EOLN) {
            print_buff_dys();
            line++;
            continue;
        }
        // if (id == 25) {
        //     used = 1;
        //     eof = 1;
        // }
        print_buff_dys();
        break;
    }
    return 0;
}

int print_dys() {
    fprintf(stdout, "%*s %2d\n", MAX_LEN, token, id);
    return 0;
}

int print_buff_dys() {
    fprintf(stdout, "%*s %2d\n", MAX_LEN, buff_token, buff_id);
    return 0;
}

int PROG() {
    SUBPROG();
    return 0;
}

int SUBPROG() {
    get_next_token();
    if (id == BG) {
        used = 1;
        DEF_LIST();
        get_next_token();
        if (id == SC) {
            used = 1;
            EXE_LIST();
            get_next_token();
            if (id == END) {
                used = 1;
                return 0;
            }
            else {
                fprintf(stderr, "***Line:%d \"end\" missing\n", line);
            }
        }
        else {
            fprintf(stderr, "***Line:%d \";\" missing\n", line);
        }
    }
    else {
        fprintf(stderr, "***Line:%d no \"begin\"\n", line);
    }
    return 0;
}

int DEF_LIST() {
    get_next_token();
    if (id == INTEGER) {
        used = 1;
        DEF();
        DEF_LIST_P();
    }
    else {
        fprintf(stderr, "***Line:%d expected integer here\n", line);
    }
    return 0;
}

int DEF_LIST_P() {
    if (buff_valid) return 0;
    get_next_token();
    if (id == SC) {
        get_buff_token();
        if (buff_id == INTEGER) {
            used = 1;
            buff_valid = 0;
            DEF();
            DEF_LIST_P();
        }
    }
    return 0;
}

int EXE_LIST() {
    EXE();
    EXE_LIST_P();
    return 0;
}

int EXE_LIST_P() {
    if (buff_valid) return 0;
    get_next_token();
    if (id == SC) {
        used = 1;
        EXE();
        EXE_LIST_P();
    }
    return 0;
}

int DEF() {
    get_next_token();
    if (id == FUNCTION) {
        used = 1;
        ID_G();
        get_next_token();
        if (id == LP) {
            used = 1;
            PARAM();
            get_next_token();
            if (id == RP) {
                used = 1;
                get_next_token();
                if (id == SC) {
                    used = 1;
                    FUNC();
                }
                else {
                    fprintf(stderr, "***Line:%d \";\" missing\n", line);
                }
            }
            else {
                fprintf(stderr, "***Line:%d \")\" missing\n", line);
            }
        }
        else {
            fprintf(stderr, "***Line:%d \"(\" missing\n", line);
        }
    }
    else {
        VAR();
    }
    return 0;
}

int VAR() {
    ID_G();
    return 0;
}

int ID_G() {
    get_next_token();
    if (id == ID) {
        used = 1;
        return 0;
    }
    else {
        fprintf(stderr, "***Line:%d variable expected\n", line);
    }
    return 0;
}

int PARAM() {
    VAR();
    return 0;
}

int FUNC() {
    get_next_token();
    if (id == BG) {
        used = 1;
        DEF_LIST();
        get_next_token();
        if (id == SC) {
            used = 1;
            EXE_LIST();
            get_next_token();
            if (id == END) {
                used = 1;
                return 0;
            }
            else {
                fprintf(stderr, "***Line:%d \"end\" missing\n", line);
            }
        }
        else {
            fprintf(stderr, "***Line:%d \";\" missing\n", line);
        }
    }
    else {
        fprintf(stderr, "***Line:%d no \"begin\"\n", line);
    }
    return 0;
}

int EXE() {
    get_next_token();
    switch (id) {
    case READ:
        used = 1;
        READ_G();
        break;
    case WRITE:
        used = 1;
        WRITE_G();
        break;
    case IF:
        used = 1;
        COND();
        break;
    default:
        ASSIGN_G();
        break;
    }
    return 0;
}

int READ_G() {
    get_next_token();
    if (id == LP) {
        used = 1;
        VAR();
        get_next_token();
        if (id == RP) {
            used = 1;
            return 0;
        }
        else {
            fprintf(stderr, "***Line:%d \")\" missing\n", line);
        }
    }
    else {
        fprintf(stderr, "***Line:%d \"(\" missing\n", line);
    }
    return 0;
}

int WRITE_G() {
    get_next_token();
    if (id == LP) {
        used = 1;
        VAR();
        get_next_token();
        if (id == RP) {
            used = 1;
            return 0;
        }
        else {
            fprintf(stderr, "***Line:%d \")\" missing\n", line);
        }
    }
    else {
        fprintf(stderr, "***Line:%d \"(\" missing\n", line);
    }
    return 0;
}

int ASSIGN_G() {
    VAR();
    get_next_token();
    if (id == ASSIGN) {
        used = 1;
        EXPR();
    }
    else {
        fprintf(stderr, "***Line:%d \":=\" missing\n", line);
    }
    return 0;
}

int COND() {
    COND_EXPR();
    get_next_token();
    if (id == THEN) {
        used = 1;
        EXE();
        get_next_token();
        if (id == ELSE) {
            used = 1;
            EXE();
        }
        else {
            fprintf(stderr, "***Line:%d \"else\" missing\n", line);
        }
    }
    else {
        fprintf(stderr, "***Line:%d \"then\" missing\n", line);
    }
    return 0;
}

int EXPR() {
    TERM();
    EXPR_P();
    return 0;
}

int EXPR_P() {
    get_next_token();
    if (id == SUB) {
        used = 1;
        TERM();
        EXPR_P();
    }
    return 0;
}

int TERM() {
    FACTOR();
    TERM_P();
    return 0;
}

int TERM_P() {
    get_next_token();
    if (id == MUL) {
        used = 1;
        FACTOR();
        TERM_P();
    }
    return 0;
}

int FACTOR() {
    get_next_token();
    if (id == ID) {
        used = 1;
        get_next_token();
        if (id == LP) {
            CALL();
        }
    }
    else if (id == CONST) {
        CONST_G();
    }
    else {
        fprintf(stderr, "***Line:%d constant expected\n", line);
    }
    return 0;
}

int CONST_G() {
    get_next_token();
    if (id == CONST) {
        used = 1;
        return 0;
    }
    else {
        fprintf(stderr, "***Line:%d constant expected\n", line);
    }
    return 0;
}

int CALL() {
    // ID_G();
    get_next_token();
    if (id == LP) {
        used = 1;
        EXPR();
        get_next_token();
        if (id == RP) {
            used = 1;
            return 0;
        }
        else {
            fprintf(stderr, "***Line:%d \")\" missing\n", line);
        }
    }
    else {
        fprintf(stderr, "***Line:%d \"(\" missing\n", line);
    }
    return 0;
}

int COND_EXPR() {
    EXPR();
    OP();
    EXPR();
    return 0;
}

int OP() {
    get_next_token();
    switch (id) {
    case EQU:
        used = 1;
        break;
    case NEQ:
        used = 1;
        break;
    case LTEQ:
        used = 1;
        break;
    case LT:
        used = 1;
        break;
    case GTEQ:
        used = 1;
        break;
    case GT:
        used = 1;
        break;
    default:
        fprintf(stderr, "***Line:%d invalid operator\n", line);
        break;
    }
    return 0;
}

int add_var() {
    return 0;
}

int add_proc() {
    return 0;
}

int search_var() {
    return 0;
}

int search_proc() {
    return 0;
}
