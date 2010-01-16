/*
 *  ast.h
 *  Untitled
 *
 *  Created by Daniel on 16/01/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _AST_H

#include <list>

#include "variant.h"

namespace Basic {
    class ASTNode;
    class Expression;
    class Block;
    class Statement;
}

using namespace Basic;

class Basic::ASTNode {
public:
    virtual void execute(void) =0;
    int getLine(void) { return line; }
    int getColumn(void) { return column; }
    void setPosition(int line, int column) { this->line = line; this->column = column; }
protected:
    int line;
    int column;
};

class Basic::Expression : public ASTNode {
public:
    Expression();
    Variant getResult();
protected:
    
};

class Basic::Block : public ASTNode {
public:
    Block();
    ~Block();
    void appendStatement(const Statement &);
protected:
    std::list<Statement> statements;
};

class Basic::Statement : public ASTNode {
public:
    Statement(StatementType);
    ~Statement();
protected:
    
};




#endif
