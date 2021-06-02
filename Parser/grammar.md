# 语法分析

## 非终结符

程序：PROG
分程序：SUBPROG
说明语句表：DEF_LIST
执行语句表：EXE_LIST
说明语句：DEF
变量说明：VDEF
函数说明：FDEF
变量：VAR
标识符：ID
参数：PARAM
函数体：FUNC
执行语句：EXE
读语句：READ
写语句：WRITE
赋值语句：ASSIGN
条件语句：COND
算术表达式：EXPR
项：TERM
因子：FACTOR
常数：CONST
函数调用：CALL
条件表达式：COND_EXPR
关系运算符：OP

## 文法

PROG -> SUBPROG
SUBPROG -> begin DEF_LIST ; EXE_LIST end
DEF_LIST -> DEF | DEF_LIST ; DEF
DEF -> VDEF | FDEF
VDEF -> integer VAR
VAR -> ID
FDEF -> integer function ID (PARAM) ; FUNC
PARAM -> VAR
FUNC -> begin DEF_LIST ; EXE_LIST end
EXE_LIST -> EXE | EXE_LIST ; EXE
EXE -> READ | WRITE | ASSIGN | COND
READ -> read(VAR)
WRITE -> write(VAR)
ASSIGN -> VAR := EXPR
EXPR -> EXPR - TERM | TERM
TERM -> TERM * FACTOR | FACTOR
FACTOR -> VAR | CONST | CALL
COND -> if COND_EXPR then EXE else EXE
COND_EXPR -> EXPR OP EXPR

## 消除左递归

PROG -> SUBPROG
SUBPROG -> begin DEF_LIST ; EXE_LIST end
DEF_LIST -> integer DEF DEF_LIST_P
DEF_LIST_P -> ; integer DEF DEF_LIST_P | $\varepsilon$
DEF -> VAR | function id (PARAM) ; FUNC
VAR -> id
PARAM -> VAR
FUNC -> begin DEF_LIST ; EXE_LIST end
EXE_LIST -> EXE EXE_LIST_P
EXE_LIST_P -> ; EXE EXE_LIST_P | $\varepsilon$
EXE -> read READ | write WRITE | ASSIGN | if COND
READ -> (VAR)
WRITE -> (VAR)
ASSIGN -> VAR := EXPR
EXPR -> TERM EXPR_P
EXPR_P -> - TERM EXPR_P | $\varepsilon$
TERM -> FACTOR TERM_P
TERM_P -> * FACTOR TERM_P | $\varepsilon$
FACTOR -> id | CONST | id ( CALL
CALL -> EXPR )
COND -> COND_EXPR then EXE else EXE
COND_EXPR -> EXPR OP EXPR
