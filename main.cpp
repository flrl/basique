#include <cstdlib>

#include "interpreter.h"

int main (int argc, char * const argv[]) {
    // insert code here...

    if (argc < 2)  exit(1);
    const char *filename = argv[1];
    
    basic::Interpreter interpreter(filename);
    interpreter.interpret();
    
    return 0;
}
