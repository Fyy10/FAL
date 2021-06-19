#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "assert.h"

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

struct variable
{
    char vname[MAX_LEN + 1];
    char vproc[MAX_LEN + 1];
    int vkind;
    // types: (integer)
    char vtype[8];
    int vlev;
    int vadr;
} var_table[TABLE_LEN];
int len_var_table = 0;

struct process
{
    char pname[MAX_LEN + 1];
    // types: (integer)
    char ptype[8];
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

int level = 1, line = 1;
char proc[MAX_LEN + 1];
int eof = 0;

int get_next_token();
int get_buff_token();
int print_dys();
int print_buff_dys();

int flush();

int add_var(char *name, char *proc, int kind, char *type);
int add_proc(char *name, char *type);
int search_var(char *name, int lev);
int search_proc(char *name, int lev);
int print_var(FILE *fvar);
int print_proc(FILE *fproc);

int PROG();
int SUBPROG();
int DEF_LIST();
int DEF_LIST_P();
int EXE_LIST();
int EXE_LIST_P();
int DEF();
int VDEF();
int FDEF();
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

int main(int argc, char *argv[])
{
    FILE *fvar, *fproc;
    if (argc > 1)
    {
        char *filename = (char *)malloc(strlen(argv[1]) + 1 + 3 + 1);

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
    else
    {
        freopen("source.dyd", "r", stdin);
        freopen("source.dys", "w", stdout);
        freopen("source.err", "w", stderr);
        fvar = fopen("source.var", "w");
        fproc = fopen("source.pro", "w");
    }

    PROG();
    // the rest should be EOLN and EOF, simply output all
    get_next_token();
    print_var(fvar);
    print_proc(fproc);

    fclose(stdin);
    fclose(stdout);
    fclose(stderr);
    fclose(fvar);
    fclose(fproc);
    return 0;
}

int get_next_token()
{
    while (1)
    {
        if (used && buff_valid)
        {
            strcpy(token, buff_token);
            id = buff_id;
            buff_valid = 0;
            used = 0;
            break;
        }
        else if (used)
        {
            scanf("%s %d", token, &id);
            used = 0;
        }
        else
        {
            break;
        }

        if (id == EOLN)
        {
            print_dys();
            used = 1;
            line++;
            continue;
        }
        if (id == 25)
        {
            used = 1;
            eof = 1;
        }
        print_dys();
        break;
    }
    return 0;
}

int get_buff_token()
{
    while (1)
    {
        scanf("%s %d", buff_token, &buff_id);
        buff_valid = 1;

        if (buff_id == EOLN)
        {
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

int print_dys()
{
    fprintf(stdout, "%*s %2d\n", MAX_LEN, token, id);
    return 0;
}

int print_buff_dys()
{
    fprintf(stdout, "%*s %2d\n", MAX_LEN, buff_token, buff_id);
    return 0;
}

int flush()
{
    while (id != SC && id != EOLN)
    {
        used = 1;
        get_next_token();
    }
    return 0;
}

int PROG()
{
    strcpy(proc, "main");
    SUBPROG();
    return 0;
}

int SUBPROG()
{
    get_next_token();
    if (id == BG)
    {
        used = 1;
        DEF_LIST();
        get_next_token();
        if (id == SC)
        {
            used = 1;
            EXE_LIST();
            get_next_token();
            if (id == END)
            {
                used = 1;
                return 0;
            }
            else
            {
                fprintf(stderr, "***Line:%d no \"end\" after \"begin\"\n", line);
            }
        }
        else
        {
            fprintf(stderr, "***Line:%d expected \";\", got \"%s\"\n", line, token);
        }
    }
    else
    {
        fprintf(stderr, "***Line:%d expected \"begin\", got \"%s\"\n", line, token);
    }
    return 0;
}

int DEF_LIST()
{
    // get_next_token();
    // if (id == INTEGER) {
    //     used = 1;
    //     DEF();
    //     DEF_LIST_P();
    // }
    // else {
    //     fprintf(stderr, "***Line:%d expected \"integer\", got \"%s\"\n", line, token);
    // }
    // return 0;
    DEF();
    DEF_LIST_P();
    return 0;
}

int DEF_LIST_P()
{
    if (buff_valid)
        return 0;
    get_next_token();
    if (id == SC)
    {
        get_buff_token();
        if (buff_id == INTEGER)
        {
            used = 1;
            // buff_valid = 0;
            DEF();
            DEF_LIST_P();
        }
    }
    return 0;
}

int EXE_LIST()
{
    EXE();
    EXE_LIST_P();
    return 0;
}

int EXE_LIST_P()
{
    if (buff_valid)
        return 0;
    get_next_token();
    if (id == SC)
    {
        used = 1;
        EXE();
        EXE_LIST_P();
    }
    return 0;
}

int DEF()
{
    get_next_token();
    if (id == INTEGER)
    {
        // is_integer:
        get_buff_token();
        switch (buff_id)
        {
        case ID:
            VDEF();
            break;

        case FUNCTION:
            level++;
            FDEF();
            level--;
            break;

        default:
            fprintf(stderr, "***Line:%d expected a variable or process name, got \"%s\"\n", line, buff_token);
            used = 1;
            buff_valid = 0;
            break;
        }
    }
    else
    {
        fprintf(stderr, "***Line:%d expected \"integer\", got \"%s\"\n", line, token);
        flush();
        // recover
        // strcpy(token, "integer");
        // id = INTEGER;
        // goto is_integer;
    }

    // if (id == FUNCTION) {
    //     used = 1;
    //     ID_G();
    //     get_next_token();
    //     if (id == LP) {
    //         used = 1;
    //         PARAM();
    //         get_next_token();
    //         if (id == RP) {
    //             used = 1;
    //             get_next_token();
    //             if (id == SC) {
    //                 used = 1;
    //                 FUNC();
    //             }
    //             else {
    //                 fprintf(stderr, "***Line:%d expected \";\", got \"%s\"\n", line, token);
    //             }
    //         }
    //         else {
    //             fprintf(stderr, "***Line:%d no \")\" after \"(\"\n", line);
    //         }
    //     }
    //     else {
    //         fprintf(stderr, "***Line:%d expected \"(\", got \"%s\"\n", line, token);
    //     }
    // }
    // else {
    //     VAR();
    // }
    return 0;
}

int VDEF()
{
    get_next_token();
    if (id == INTEGER)
    {
        used = 1;
        get_next_token();
        if (search_var(token, level) != -1)
        {
            fprintf(stderr, "***Line:%d variable \"%s\" already defined\n", line, token);
            flush();
        }
        else
        {
            add_var(token, proc, 0, "int");
            VAR();
        }
    }
    else
    {
        fprintf(stderr, "***Line:%d expected \"integer\", got \"%s\"\n", line, token);
    }
    return 0;
}

int FDEF()
{
    get_next_token();
    if (id == INTEGER)
    {
        used = 1;
        get_next_token();
        if (id == FUNCTION)
        {
            used = 1;
            get_next_token();
            if (id == ID)
            {
                strcpy(proc, token);
                add_proc(token, "int");
            }
            ID_G();
            get_next_token();
            if (id == LP)
            {
                used = 1;
                PARAM();
                get_next_token();
                if (id == RP)
                {
                    used = 1;
                    get_next_token();
                    if (id == SC)
                    {
                        used = 1;
                        proc_table[len_proc_table - 1].fadr = len_var_table - 1;
                        FUNC();
                        proc_table[len_proc_table - 1].ladr = len_var_table - 1;
                        strcpy(proc, "main");
                    }
                    else
                    {
                        fprintf(stderr, "***Line:%d expected \";\", got \"%s\"\n", line, token);
                    }
                }
                else
                {
                    fprintf(stderr, "***Line:%d no \")\" after \"(\"\n", line);
                }
            }
            else
            {
                fprintf(stderr, "***Line:%d expected \"(\", got \"%s\"\n", line, token);
            }
        }
        else
        {
            fprintf(stderr, "***Line:%d expected \"function\", got \"%s\"\n", line, token);
        }
    }
    else
    {
        fprintf(stderr, "***Line:%d expected \"integer\", got \"%s\"\n", line, token);
    }
    return 0;
}

int VAR()
{
    get_next_token();
    if ((id == ID) && (search_var(token, level) == -1) && (search_proc(token, level) == -1))
    {
        fprintf(stderr, "***Line:%d variable \"%s\" undefined\n", line, token);
    }
    ID_G();
    return 0;
}

int ID_G()
{
    get_next_token();
    if (id == ID)
    {
        used = 1;
        return 0;
    }
    else
    {
        fprintf(stderr, "***Line:%d variable expected\n", line);
    }
    return 0;
}

int PARAM()
{
    get_next_token();
    add_var(token, proc, 1, "int");
    // VAR();
    ID_G();
    return 0;
}

int FUNC()
{
    get_next_token();
    if (id == BG)
    {
        used = 1;
        DEF_LIST();
        get_next_token();
        if (id == SC)
        {
            used = 1;
            EXE_LIST();
            get_next_token();
            if (id == END)
            {
                used = 1;
                return 0;
            }
            else
            {
                fprintf(stderr, "***Line:%d no \"end\" after \"begin\"\n", line);
            }
        }
        else
        {
            fprintf(stderr, "***Line:%d expected \";\", got \"%s\"\n", line, token);
        }
    }
    else
    {
        fprintf(stderr, "***Line:%d expected \"begin\", got \"%s\"\n", line, token);
    }
    return 0;
}

int EXE()
{
    get_next_token();
    switch (id)
    {
    case READ:
        READ_G();
        break;
    case WRITE:
        WRITE_G();
        break;
    case IF:
        COND();
        break;
    case ID:
        ASSIGN_G();
        break;
    default:
        fprintf(stderr, "***Line:%d invalid exe statement \"%s\"\n", line, token);
        flush();
        break;
    }
    return 0;
}

int READ_G()
{
    get_next_token();
    assert(id == READ);
    used = 1;
    get_next_token();
    if (id == LP)
    {
        used = 1;
        get_next_token();
        if ((id == ID) && (search_var(token, level) == -1))
        {
            fprintf(stderr, "***Line:%d variable \"%s\" undefined\n", line, token);
            flush();
            return 0;
        }
        VAR();
        get_next_token();
        if (id == RP)
        {
            used = 1;
            return 0;
        }
        else
        {
            fprintf(stderr, "***Line:%d no \")\" after \"(\"\n", line);
        }
    }
    else
    {
        fprintf(stderr, "***Line:%d expected \"(\", got \"%s\"\n", line, token);
    }
    return 0;
}

int WRITE_G()
{
    get_next_token();
    assert(id == WRITE);
    used = 1;
    get_next_token();
    if (id == LP)
    {
        used = 1;
        get_next_token();
        if ((id == ID) && (search_var(token, level) == -1))
        {
            fprintf(stderr, "***Line:%d variable \"%s\" undefined\n", line, token);
            // flush
            while (id != EOLN)
                scanf("%s %d", token, &id);
            used = 1;
            get_next_token();
            return 0;
        }
        VAR();
        get_next_token();
        if (id == RP)
        {
            used = 1;
            return 0;
        }
        else
        {
            fprintf(stderr, "***Line:%d no \")\" after \"(\"\n", line);
        }
    }
    else
    {
        fprintf(stderr, "***Line:%d expected \"(\", got \"%s\"\n", line, token);
    }
    return 0;
}

int ASSIGN_G()
{
    get_next_token();
    if ((id == ID) && (search_var(token, level) == -1) && (search_proc(token, level) == -1))
    {
        fprintf(stderr, "***Line:%d variable \"%s\" undefined\n", line, token);
        flush();
        return 0;
    }
    VAR();
    get_next_token();
    if (id == ASSIGN)
    {
        used = 1;
        EXPR();
    }
    else
    {
        fprintf(stderr, "***Line:%d expected \":=\", got \"%s\"\n", line, token);
    }
    return 0;
}

int COND()
{
    get_next_token();
    assert(id == IF);
    used = 1;
    COND_EXPR();
    get_next_token();
    if (id == THEN)
    {
        used = 1;
        EXE();
        get_next_token();
        if (id == ELSE)
        {
            used = 1;
            EXE();
        }
        else
        {
            fprintf(stderr, "***Line:%d expected \"else\", got \"%s\"\n", line, token);
        }
    }
    else
    {
        fprintf(stderr, "***Line:%d expected \"then\", got \"%s\"\n", line, token);
    }
    return 0;
}

int EXPR()
{
    TERM();
    EXPR_P();
    return 0;
}

int EXPR_P()
{
    get_next_token();
    if (id == SUB)
    {
        used = 1;
        TERM();
        EXPR_P();
    }
    return 0;
}

int TERM()
{
    FACTOR();
    TERM_P();
    return 0;
}

int TERM_P()
{
    get_next_token();
    if (id == MUL)
    {
        used = 1;
        FACTOR();
        TERM_P();
    }
    return 0;
}

int FACTOR()
{
    get_next_token();
    switch (id)
    {
    case CONST:
        CONST_G();
        break;

    case ID:
        get_buff_token();
        if (buff_id == LP)
        {
            CALL();
        }
        else
        {
            VAR();
        }
        break;

    default:
        fprintf(stderr, "***Line:%d invalid factor \"%s\"\n", line, token);
        break;
    }
    // if (id == ID)
    // {
    //     used = 1;
    //     get_next_token();
    //     if (id == LP)
    //     {
    //         CALL();
    //     }
    // }
    // else if (id == CONST)
    // {
    //     CONST_G();
    // }
    // else
    // {
    //     fprintf(stderr, "***Line:%d expected a constant, got \"%s\"\n", line, token);
    // }
    return 0;
}

int CONST_G()
{
    get_next_token();
    if (id == CONST)
    {
        used = 1;
        return 0;
    }
    else
    {
        fprintf(stderr, "***Line:%d expected a constant, got \"%s\"\n", line, token);
    }
    return 0;
}

int CALL()
{
    ID_G();
    get_next_token();
    if (id == LP)
    {
        used = 1;
        EXPR();
        get_next_token();
        if (id == RP)
        {
            used = 1;
            return 0;
        }
        else
        {
            fprintf(stderr, "***Line:%d no \")\" after \"(\"\n", line);
        }
    }
    else
    {
        fprintf(stderr, "***Line:%d expected \"(\", got \"%s\"\n", line, token);
    }
    return 0;
}

int COND_EXPR()
{
    EXPR();
    OP();
    EXPR();
    return 0;
}

int OP()
{
    get_next_token();
    switch (id)
    {
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
        fprintf(stderr, "***Line:%d expected an operator, got \"%s\"\n", line, token);
        break;
    }
    return 0;
}

int add_var(char *name, char *proc, int kind, char *type)
{
    strcpy(var_table[len_var_table].vname, name);
    strcpy(var_table[len_var_table].vproc, proc);
    var_table[len_var_table].vkind = kind;
    strcpy(var_table[len_var_table].vtype, type);
    var_table[len_var_table].vlev = level;
    var_table[len_var_table].vadr = len_var_table;
    len_var_table++;
    return 0;
}

int add_proc(char *name, char *type)
{
    strcpy(proc_table[len_proc_table].pname, name);
    strcpy(proc_table[len_proc_table].ptype, type);
    proc_table[len_proc_table].plev = level;
    len_proc_table++;
    return 0;
}

int search_var(char *name, int lev)
{
    for (int i = 0; i < len_var_table; i++)
    {
        if ((strcmp(var_table[i].vname, name) == 0) && (var_table[i].vlev == lev) && (var_table[i].vkind == 0))
            return i;
    }
    return -1;
}

int search_proc(char *name, int lev)
{
    for (int i = 0; i < len_proc_table; i++)
    {
        if (strcmp(proc_table[i].pname, name) == 0)
            return i;
    }
    return -1;
}

int print_var(FILE *fvar)
{
    for (int i = 0; i < len_var_table; i++)
    {
        fprintf(fvar, "%s %s %d %s %d %d\n",
                var_table[i].vname, var_table[i].vproc, var_table[i].vkind,
                var_table[i].vtype, var_table[i].vlev, var_table[i].vadr);
    }
    return 0;
}

int print_proc(FILE *fproc)
{
    for (int i = 0; i < len_proc_table; i++)
    {
        fprintf(fproc, "%s %s %d %d %d\n",
                proc_table[i].pname, proc_table[i].ptype,
                proc_table[i].plev, proc_table[i].fadr, proc_table[i].ladr);
    }
    return 0;
}
