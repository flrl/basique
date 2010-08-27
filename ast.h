/*
 *  ast.h
 *  Untitled
 *
 *  Created by Ellie on 16/01/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _AST_H

#include <list>

#include "variant.h"

namespace Basic {
    class ASTNode;
    class Identifier;
    class Expression;
    class Block;
    class Statement;
    class PrintStatement;
    class SemicolonExpression;
    class CommaExpression;
    class ForStatement;
    class DoStatement;
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

class Basic::Identifier : public ASTNode {
public:
    Identifier();
protected:
};

class Basic::Expression : public ASTNode {
public:
    Expression();
    Variant getResult() { return result; }
protected:
    Variant result;
    
};

class Basic::Block : public ASTNode {
public:
    Block();
    ~Block();
    void appendStatement(const Statement &);
protected:
    std::list<ASTNode> statements;
};

class Basic::PrintStatement : public ASTNode {
public:
    PrintStatement();
protected:
    std::list<SemicolonExpression> semicolonExpressions;
};

class Basic::SemicolonExpression : public ASTNode {
public:
    SemicolonExpression();
protected:
    std::list<CommaExpression> commaExpressions;
};

class Basic::CommaExpression : public ASTNode {
public:
    CommaExpression();
protected:
    std::list<Expression> expressions;
};

class Basic::ForStatement : public ASTNode {
public:
    ForStatement();
protected:
    Identifier *identifier;
    Variant initialValue;
    Variant finalValue;
    Variant stepValue;
    Block *body;
};

enum DoConditionType {
    DcWHILE,
    DcUNTIL,
};

enum DoConditionWhen {
    DcPRECONDITION,
    DcPOSTCONDITION 
};

class Basic::DoStatement : public ASTNode {
public:
    DoStatement();
protected:
    DoConditionType conditionType;
    DoConditionWhen conditionWhen;
    Expression *condition;
    Block *body;
};


#endif
