/*
 *  interpreter.cpp
 *  Untitled
 *
 *  Created by Ellie on 18/09/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include <cassert>

#include "ast.h"
#include "interpreter.h"

basic::Interpreter::Interpreter(const String &filename) {
    m_tokeniser = new Tokeniser(filename);
    assert(m_tokeniser != NULL);
    m_parser = new Parser(m_tokeniser);
    assert(m_parser != NULL);
}

basic::Interpreter::Interpreter(FILE *file) {
    m_tokeniser = new Tokeniser(file);
    assert(m_tokeniser != NULL);
    m_parser = new Parser(m_tokeniser);
    assert(m_parser != NULL);
}

basic::Interpreter::~Interpreter() {
    delete m_parser;
    delete m_tokeniser;
}

void basic::Interpreter::interpret() const {
    Unit *parse_tree = NULL;
    
    while ((parse_tree = m_parser->parse())) {
        parse_tree->execute();
        if (not parse_tree->installed())  delete parse_tree;
    }
}
