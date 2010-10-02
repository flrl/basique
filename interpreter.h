/*
 *  interpreter.h
 *  Untitled
 *
 *  Created by Daniel on 18/09/10.
 *  Copyright 2010 Daniel Hyde. All rights reserved.
 *
 */

#ifndef BASIQUE_INTERPRETER_H
#define BASIQUE_INTERPRETER_H

#include <cstdio>

#include "parser.h"
#include "string.h"
#include "tokeniser.h"

namespace basic {
    class Interpreter;
}

class basic::Interpreter {
public:
    Interpreter(const String &);
    Interpreter(FILE *);
    ~Interpreter();
    
    void interpret() const;
    
private:
    Parser *m_parser;
    Tokeniser *m_tokeniser;
};

#endif
