#include "ast.hpp"
#include "codegen.hpp"
#include "parser.hpp"

using namespace std;

/* Compile the AST into a module */
void CodeGenContext::generateCode(NBlock& root) {
    std::cout << "Generating code...\n";

    /* Create the top level interpreter function to call as entry */
    vector<Type*> argTypes;
    FunctionType *ftype = FunctionType::get(Type::getVoidTy(MyContext), ArrayRef(argTypes), false);
    mainFunction = Function::Create(ftype, GlobalValue::InternalLinkage, "main", module);
    BasicBlock *bblock = BasicBlock::Create(MyContext, "entry", mainFunction, 0);

    /* Push a new variable/block context */
    pushBlock(bblock);
    root.codeGen(*this); /* emit bytecode for the toplevel block */
    ReturnInst::Create(MyContext, bblock);
    popBlock();

    /* Print the bytecode in a human-readable format
       to see if our program compiled properly
     */
    std::cout << "Code is generated.\n";
    // module->dump();

    legacy::PassManager pm;
    // TODO: remove legacy code
    pm.add(createPrintModulePass(outs()));
    pm.run(*module);
}

/* Executes the AST by running the main function */
GenericValue CodeGenContext::runCode() {
    std::cout << "Running code...\n";
    ExecutionEngine *ee = EngineBuilder( unique_ptr<Module>(module) ).create();
    ee->finalizeObject();
    vector<GenericValue> noargs;
    GenericValue v = ee->runFunction(mainFunction, noargs);
    std::cout << "Code was run.\n";
    return v;
}

/* Returns an LLVM type based on the identifier */
static Type *typeOf(const NIdentifier& type) {
    if (type.name == "int") {
        return Type::getInt64Ty(MyContext);
    } else if (type.name == "double") {
        return Type::getDoubleTy(MyContext);
    }
    return Type::getVoidTy(MyContext);
}

/* -- Code Generation -- */

// <integer>
Value* NInteger::codeGen(CodeGenContext& context) {
    std::cout << "Creating integer: " << value << endl;
    return ConstantInt::get(Type::getInt64Ty(MyContext), value, true);
}

// <double>
Value* NDouble::codeGen(CodeGenContext& context) {
    std::cout << "Creating double: " << value << endl;
    return ConstantFP::get(Type::getDoubleTy(MyContext), value);
}

// <ident>
Value* NIdentifier::codeGen(CodeGenContext& context) {
    std::cout << "Creating identifier reference: " << name << endl;
    if (context.locals().find(name) == context.locals().end()) {
        std::cerr << "undeclared variable " << name << endl;
        exit(1);
    }

    // return nullptr;
    return new LoadInst(context.locals()[name]->getType(), context.locals()[name], name, false, context.currentBlock());
}

// <ident> (<call_args>)
Value* NMethodCall::codeGen(CodeGenContext& context) {
    Function *function = context.module->getFunction(id.name);
    if (function == NULL) {
        std::cerr << "no such function " << id.name << endl;
        exit(1);
    }
    std::vector<Value*> args;
    ExpressionList::const_iterator it;
    for (it = arguments.begin(); it != arguments.end(); it++) {
        args.push_back((**it).codeGen(context));
    }
    CallInst *call = CallInst::Create(function, ArrayRef(args), "", context.currentBlock());
    std::cout << "Creating method call: " << id.name << endl;
    return call;
}

// <expr> : <expr> <binop> <expr>
Value* NBinaryOperator::codeGen(CodeGenContext& context) {
    std::cout << "Creating binary operation " << op << endl;
    Instruction::BinaryOps instr;
    // <binop> : + | - | * | / | < | <= | > | >= | != | ==
    switch (op) {
        // +
        case TPLUS:     instr = Instruction::Add; break;
        // -
        case TMINUS: 	instr = Instruction::Sub; break;
        // *
        case TMUL: 		instr = Instruction::Mul; break;
        // /
        case TDIV: 		instr = Instruction::SDiv; break;

        /* TODO comparison */

        // unimplemented binary operators, exit(1)
        default:        cout << "unimplemented binary operator " << op << endl; exit(1);
    }

    // implemented binary operators
    return BinaryOperator::Create(
        instr, lhs.codeGen(context),
        rhs.codeGen(context), "",
        context.currentBlock()
    );
}

// <expr> : <ident> = <ident>
Value* NAssignment::codeGen(CodeGenContext& context) {
    std::cout << "Creating assignment for " << lhs.name << endl;
    if (context.locals().find(lhs.name) == context.locals().end()) {
        std::cerr << "undeclared variable " << lhs.name << endl;
        exit(1);
    }
    return new StoreInst(rhs.codeGen(context), context.locals()[lhs.name], false, context.currentBlock());
}

// <block> : {}
//         | {<stmts>}
Value* NBlock::codeGen(CodeGenContext& context) {
    StatementList::const_iterator it;
    Value *last = NULL;
    for (it = statements.begin(); it != statements.end(); it++) {
        std::cout << "Generating code for " << typeid(*it).name() << endl;
        last = (**it).codeGen(context);
    }
    std::cout << "Creating block" << endl;
    return last;
}

// <stmt> : <expr>
Value* NExpressionStatement::codeGen(CodeGenContext& context) {
    std::cout << "Generating code for " << typeid(expression).name() << endl;
    return expression.codeGen(context);
}

// <stmt> : return <expr>
Value* NReturnStatement::codeGen(CodeGenContext& context) {
    std::cout << "Generating return code for " << typeid(expression).name() << endl;
    Value *returnValue = expression.codeGen(context);
    context.setCurrentReturnValue(returnValue);
    return returnValue;
}

// <var_decl> : <ident> <ident>
//            | <ident> <ident> = <expr>
Value* NVariableDeclaration::codeGen(CodeGenContext& context) {
    std::cout << "Creating variable declaration " << type.name << " " << id.name << endl;
    AllocaInst *alloc = new AllocaInst(typeOf(type), 4, id.name.c_str(), context.currentBlock());
    context.locals()[id.name] = alloc;
    if (assignmentExpr != NULL) {
        NAssignment assn(id, *assignmentExpr);
        assn.codeGen(context);
    }
    return alloc;
}

// <extern_decl> : extern <ident> <ident> (<func_decl_args>)
Value* NExternDeclaration::codeGen(CodeGenContext& context) {
    vector<Type*> argTypes;
    VariableList::const_iterator it;
    for (it = arguments.begin(); it != arguments.end(); it++) {
        argTypes.push_back(typeOf((**it).type));
    }
    FunctionType *ftype = FunctionType::get(typeOf(type), ArrayRef(argTypes), false);
    Function *function = Function::Create(ftype, GlobalValue::ExternalLinkage, id.name.c_str(), context.module);
    return function;
}

// <func_decl> : <ident> <ident> (<func_decl_args>) <block>
// int fname(args) { ... }
Value* NFunctionDeclaration::codeGen(CodeGenContext& context) {
    vector<Type*> argTypes;
    VariableList::const_iterator it;
    for (it = arguments.begin(); it != arguments.end(); it++) {
        argTypes.push_back(typeOf((**it).type));
    }
    FunctionType *ftype = FunctionType::get(typeOf(type), ArrayRef(argTypes), false);
    Function *function = Function::Create(ftype, GlobalValue::InternalLinkage, id.name.c_str(), context.module);
    BasicBlock *bblock = BasicBlock::Create(MyContext, "entry", function, 0);

    context.pushBlock(bblock);

    Function::arg_iterator argsValues = function->arg_begin();
    Value* argumentValue;

    for (it = arguments.begin(); it != arguments.end(); it++) {
        (**it).codeGen(context);

        argumentValue = &*argsValues++;
        argumentValue->setName((*it)->id.name.c_str());
        StoreInst *inst = new StoreInst(argumentValue, context.locals()[(*it)->id.name], false, bblock);
    }

    block.codeGen(context);
    ReturnInst::Create(MyContext, context.getCurrentReturnValue(), bblock);

    context.popBlock();
    std::cout << "Creating function: " << id.name << endl;
    return function;
}
