#include <iostream>
#include "codegen.hpp"
#include "ast.hpp"

using namespace std;

extern int yyparse();
extern NBlock* programBlock;

void open_file(const char* filename) {
    // openfile
    freopen(filename, "r", stdin);
}

void createCoreFunctions(CodeGenContext& context);

int main(int argc, char **argv) {
    if (argc > 1) {
        open_file(argv[1]);
    }
    yyparse();
    cout << programBlock << endl;

    InitializeNativeTarget();
    InitializeNativeTargetAsmPrinter();
    InitializeNativeTargetAsmParser();

    CodeGenContext context;

    createCoreFunctions(context);
    context.generateCode(*programBlock);

    context.runCode();

    return 0;
}
