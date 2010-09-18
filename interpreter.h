/*
 *  interpreter.h
 *  Untitled
 *
 *  Created by Ellie on 18/09/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _INTERPRETER_H
#define _INTERPRETER_H

#include "parser.h"
#include "string.h"
#include "tokeniser.h"

namespace basic {
    class Interpreter;
}

class basic::Interpreter {
public:
    Interpreter(const String &);
    Interpreter(int);
    ~Interpreter();
    
    void interpret() const;
    
private:
    Parser *m_parser;
    Tokeniser *m_tokeniser;
};

#endif
