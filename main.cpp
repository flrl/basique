#include <cstdlib>

#include "interpreter.h"

basic::SymbolTable g_symbol_table;

int main (int argc, char * const argv[]) {
    basic::Interpreter *interpreter = NULL;

    if (argc > 1) {
        // FIXME loop over input files
        interpreter = new basic::Interpreter(argv[1]);
    }
    else {
        setlinebuf(stdin);
        interpreter = new basic::Interpreter(stdin);
    }

    interpreter->interpret();
    
    return 0;
}
