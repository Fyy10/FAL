#include <iostream>
#include <vector>
#include <llvm/IR/Value.h>

class CodeGenContext;
class NStatement;
class NExpression;
class NVariableDeclaration;

// <stmts> : <stmt>
//         | <stmts> <stmt>
typedef std::vector<NStatement*> StatementList;

// <call_args> : /* blank */
//             | <expr>
//             | <call_args>, <expr>
typedef std::vector<NExpression*> ExpressionList;

// <func_decl_args> : /* blank */
//                  | <var_decl>
//                  | <func_decl_args>, <var_decl>
typedef std::vector<NVariableDeclaration*> VariableList;

class Node {
public:
    virtual ~Node() {}
    virtual llvm::Value* codeGen(CodeGenContext& context) { return NULL; }
};

// <expr>
class NExpression : public Node {
};

// <stmt>
class NStatement : public Node {
};

// <stmt> : <expr>
class NExpressionStatement : public NStatement {
public:
    NExpression& expression;

    NExpressionStatement(NExpression& expression) :
        expression(expression) { }

    virtual llvm::Value* codeGen(CodeGenContext& context);
};

// <stmt> : return <expr>
class NReturnStatement : public NStatement {
public:
    NExpression& expression;

    NReturnStatement(NExpression& expression) :
        expression(expression) { }

    virtual llvm::Value* codeGen(CodeGenContext& context);
};

// <expr> : <ident>
class NIdentifier : public NExpression {
public:
    const std::string name;
    NIdentifier(const std::string& name) : name(name) { }
    virtual llvm::Value* codeGen(CodeGenContext& context);
};

// <expr> : <integer>
class NInteger : public NExpression {
public:
    const long long value;
    NInteger(const long long value) : value(value) { }
    virtual llvm::Value* codeGen(CodeGenContext& context);
};

// <expr> : <double>
class NDouble : public NExpression {
public:
    const double value;
    NDouble(const double value) : value(value) { }
    virtual llvm::Value* codeGen(CodeGenContext& context);
};

// <expr> : <ident> = <expr>
class NAssignment : public NExpression {
public:
    const NIdentifier& lhs;
    NExpression& rhs;

    NAssignment(const NIdentifier& lhs, NExpression& rhs) :
        lhs(lhs), rhs(rhs) { }

    virtual llvm::Value* codeGen(CodeGenContext& context);
};

// <expr> : <ident> (<call_args>)
class NMethodCall : public NExpression {
public:
    const NIdentifier& id;
    const ExpressionList arguments;

    NMethodCall(const NIdentifier& id, const ExpressionList& arguments) :
        id(id), arguments(arguments) { }

    virtual llvm::Value* codeGen(CodeGenContext& context);
};

// <expr> : <expr> <binop> <expr>
class NBinaryOperator : public NExpression {
public:
    int op;
    NExpression& lhs;
    NExpression& rhs;

    NBinaryOperator(NExpression& lhs, int op, NExpression& rhs) :
        lhs(lhs), op(op), rhs(rhs) { }

    virtual llvm::Value* codeGen(CodeGenContext& context);
};

// <block> : /* blank*/
//         | {<stmts>}
class NBlock : public Node {
public:
    StatementList statements;
    NBlock() { }
    virtual llvm::Value* codeGen(CodeGenContext& context);
};

// <var_decl> : <ident> <ident>
//            | <ident> <ident> = <expr>
class NVariableDeclaration : public NStatement {
public:
    const NIdentifier& type;
    const NIdentifier& id;
    NExpression *assignmentExpr;

    NVariableDeclaration(const NIdentifier& type, const NIdentifier& id) :
        type(type), id(id) { assignmentExpr = NULL; }

    NVariableDeclaration(
        const NIdentifier& type,
        const NIdentifier& id,
        NExpression *assignmentExpr
    ) : type(type), id(id), assignmentExpr(assignmentExpr) {}

    virtual llvm::Value* codeGen(CodeGenContext& context);
};

// <func_decl> : <ident> <ident> (<func_decl_args>) <block>
class NFunctionDeclaration : public NStatement {
public:
    const NIdentifier& type;
    const NIdentifier& id;
    const VariableList arguments;
    NBlock& block;

    NFunctionDeclaration(
        const NIdentifier& type,
        const NIdentifier& id,
        const VariableList& arguments,
        NBlock& block
    ) : type(type), id(id), arguments(arguments), block(block) {}

    virtual llvm::Value* codeGen(CodeGenContext& context);
};

// <extern_decl> : extern <ident> <ident> (<func_decl_args>)
class NExternDeclaration : public NStatement {
public:
    const NIdentifier& type;
    const NIdentifier& id;
    const VariableList arguments;

    NExternDeclaration(
        const NIdentifier& type,
        const NIdentifier& id,
        const VariableList& arguments
    ) : type(type), id(id), arguments(arguments) {}

    virtual llvm::Value* codeGen(CodeGenContext& context);
};
