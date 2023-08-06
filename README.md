# FAL

- [FAL](#fal)
  - [Language Syntax](#language-syntax)
    - [Grammar Description](#grammar-description)
    - [Code Example](#code-example)
  - [Abstract Syntax Tree](#abstract-syntax-tree)
  - [Usage](#usage)
    - [Prerequisites](#prerequisites)
    - [Build](#build)
    - [Run](#run)

FA:male_sign: Language

## Language Syntax

### Grammar Description

```plain
<prog> : <stmts>

<stmts> : <stmt>
        | <stmts> <stmt>

<stmt> : <var_decl> | <func_decl> | <extern_decl>
       | <expr>
       | return <expr>

<var_decl> : <ident> <ident>
           | <ident> <ident> = <expr>

<func_decl> : <ident> <ident> (<func_decl_args>) <block>

<extern_decl> : extern <ident> <ident> (<func_decl_args>)

<expr> : <ident>
       | <numeric>
       | <ident> = <expr>
       | <ident> (<call_args>)
       | <expr> <binop> <expr>
       | (<expr>)

<func_decl_args> : /* blank */
                 | <var_decl>
                 | <func_decl_args>, <var_decl>

<block> : {}
        | {<stmts>}

<numeric> : <integer>
          | <double>

<call_args> : /* blank */
            | <expr>
            | <call_args>, <expr>

<binop> : + | - | * | / | < | <= | > | >= | != | ==
```

### Code Example

```c
extern void print_val(int val)

int my_func(int a) {
    int x = a * 5
    return x + 3
}

echo(my_func(11))
echo(my_func(12))
print_val(10)
```

## Abstract Syntax Tree

## Usage

Source codes are in the `src` directory.

### Prerequisites

- Flex 2.6.4
- Bison 2.3
- LLVM 16.0.6

### Build

You may need to set the path of `llvm-config` in `Makefile`.

```bash
mkdir build
make
```

### Run

```bash
./build/main example.fa
```
