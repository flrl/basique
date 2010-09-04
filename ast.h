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
#include <utility>

#include "tokeniser.h"
#include "variant.h"

namespace Basic {
    class ASTNode;

    class ParamList;
    class ArraySubscript;
    class ArrayDimension;
    
    class Expression;
    class FunctionCallExpression;
    class ArrayIndexExpression;
    class VariableExpression;
    class LiteralExpression;
    class UnaryExpression;
    class MultiplicativeExpression;
    class AdditiveExpression;
    class ComparitiveExpression;
    class AndExpression;
    class OrExpression;
    
    class Statement;
    class PrintStatement;
    class InputStatement;
    class LetStatement;
    class CallStatement;
    class IfStatement;
    class DoStatement;
    class ForStatement;
    class DimStatement;

    class Block;
    
    class Identifier;
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

class Basic::Block : public ASTNode {
public:
    Block() { }
    ~Block();
    virtual void execute();
    void appendStatement(Statement *s) { statements.push_back(s); }
protected:
    std::list<Statement *> statements;
};

class Basic::ParamList : public ASTNode {
public:
    ParamList() { }
    ~ParamList();
    virtual void execute();
    void appendExpression(Expression *e) { expressions.push_back(e); }
private:
    std::list<Expression *> expressions;
};

class Basic::ArraySubscript : public ASTNode {
public:
    ArraySubscript(Expression *e) { expressions.push_back(e); }
    ~ArraySubscript();
    virtual void execute();
    void appendExpression(Expression *e) { expressions.push_back(e); }
private:
    std::list<Expression *> expressions;
};

typedef std::pair<Basic::Expression*, Basic::Expression*> ArrayDimensionSpecification;

class Basic::ArrayDimension : public ASTNode {
public:
    ArrayDimension(Expression *d1, Expression *d2) { dimensions.push_back(std::make_pair(d1, d2)); }
    ~ArrayDimension();
    virtual void execute();
    void appendDimension(Expression *d1, Expression *d2) { dimensions.push_back(std::make_pair(d1, d2)); }
private:
    std::list<ArrayDimensionSpecification> dimensions;
};

class Basic::Expression : public ASTNode {
public:
    Expression() { }
    Variant getResult() { return value; }
protected:
    Variant value;
};

class Basic::FunctionCallExpression : public Expression {
public:
    FunctionCallExpression(const char *id, ParamList *p) : params(p) {
        strncpy(this->identifier, id, MAX_IDENTIFIER_LENGTH);        
    }
    ~FunctionCallExpression() { delete params; }
    virtual void execute();
private:
    char identifier[MAX_IDENTIFIER_LENGTH + 1];
    ParamList *params;
};

class Basic::ArrayIndexExpression : public Expression {
public:
    ArrayIndexExpression(const char *id, Expression *first, Expression *second) : first(first), second(second) {
        strncpy(this->identifier, id, MAX_IDENTIFIER_LENGTH);
    }
    ~ArrayIndexExpression() { delete first; delete second; }
    virtual void execute();
private:
    Expression *first;
    Expression *second;
    char identifier[MAX_IDENTIFIER_LENGTH + 1];
};

class Basic::VariableExpression : public Expression {
public:
    VariableExpression(const char *id) { strncpy(this->identifier, id, MAX_IDENTIFIER_LENGTH); }
    virtual void execute();
private:
    char identifier[MAX_IDENTIFIER_LENGTH + 1];
};

class Basic::LiteralExpression : public Expression {
public:
    LiteralExpression(Variant &v) { this->value = v; }
    virtual void execute();
};

class Basic::UnaryExpression : public Expression {
public:
    UnaryExpression(Token t, Expression *e) : op(t), term(e) { }
    ~UnaryExpression() { delete term; }
    virtual void execute();
private:
    Token op;
    Expression *term;
};

class Basic::MultiplicativeExpression : public Expression {
public:
    MultiplicativeExpression() { }
    ~MultiplicativeExpression();
    virtual void execute();
    void appendTerm(Expression *term) { terms.push_back(term); }
    void appendOperator(Token t) { operators.push_back(t); }
private:
    std::list<Expression *> terms;
    std::list<Token> operators;
};

class Basic::AdditiveExpression : public Expression {
public:
    AdditiveExpression() { }
    ~AdditiveExpression();
    virtual void execute();
    void appendTerm(Expression *term) { terms.push_back(term); }
    void appendOperator(Token t) { operators.push_back(t); }
private:
    std::list<Expression *> terms;
    std::list<Token> operators;
};

class Basic::ComparitiveExpression : public Expression {
public:
    ComparitiveExpression(Expression *first, Token cmp, Expression *second) : first(first), cmp(cmp), second(second) { }
    ~ComparitiveExpression() { delete first; delete second; }
    virtual void execute();
private:
    Expression *first;
    Token cmp;
    Expression *second;
};

class Basic::AndExpression : public Expression {
public:
    AndExpression() { }
    ~AndExpression();
    virtual void execute();
    void appendTerm(Expression *term) { terms.push_back(term); }
private:
    std::list<Expression *> terms;
};

class Basic::OrExpression : public Expression {
public:
    OrExpression() { }
    ~OrExpression();
    virtual void execute();
    void appendTerm(Expression *term) { terms.push_back(term); }
private:
    std::list<Expression *> terms;
};


class Basic::Statement : public ASTNode { };

class Basic::PrintStatement : public Statement {
public:
    PrintStatement() { }
    ~PrintStatement();
    virtual void execute();
    void appendExpression(Expression *e) { expressions.push_back(e); }
    void setAppendEol(bool b) { append_eol = b; }
    
private:
    std::list<Expression *> expressions;
    bool append_eol;
};

class Basic::InputStatement : public Statement {
public:
    InputStatement() { }
    ~InputStatement();
    virtual void execute();
    void appendIdentifier(const char *s) {
        char *identifier = new char[1 + strlen(s)];
        strcpy(identifier, s);
        identifiers.push_back(identifier);
    }
private:
    std::list<char *> identifiers;
};

class Basic::LetStatement : public Statement {
public:
    LetStatement(const char *i, ArraySubscript *s, Expression *e) : subscript(s), expression(e) { 
        identifier = new char[1 + strlen(i)];
        strcpy(identifier, i);
    }
    ~LetStatement() { delete[] identifier; if (subscript) delete subscript; delete expression; }
    virtual void execute();
private:
    char *identifier;
    ArraySubscript *subscript;
    Expression *expression;
};

class Basic::CallStatement : public Statement {
public:
    CallStatement(const char *i, ParamList *params) : params(params) {
        identifier = new char[1 + strlen(i)];
        strcpy(identifier, i);
    }
    ~CallStatement() { delete[] identifier; if (params) delete params; }
    virtual void execute();
private:
    char *identifier;
    ParamList *params;
};

class Basic::IfStatement : public Statement {
public:
    IfStatement() { }
    ~IfStatement();
    virtual void execute();
    void appendCondition(Expression *condition, Block *block) { conditions.push_back(condition); blocks.push_back(block); }
    void setElseBlock(Block *block) { if (else_block) delete else_block; else_block = block; }
private:
    std::list<Expression *> conditions;
    std::list<Block *> blocks;
    Block *else_block;
};

enum DoConditionType {
    DcWHILE,
    DcUNTIL,
    DcFOREVER,
    DcONCE,
};

enum DoConditionWhen {
    DcPRECONDITION,
    DcPOSTCONDITION 
};

class Basic::DoStatement : public Statement {
public:
    DoStatement(DoConditionType t, DoConditionWhen w, Expression *c, Block *b) 
        : condition_type(t), condition_when(w), condition(c), body(b) { }
    ~DoStatement() { if (condition)  delete condition; delete body; }
    virtual void execute();
private:
    DoConditionType condition_type;
    DoConditionWhen condition_when;
    Expression *condition;
    Block *body;
};

class Basic::ForStatement : public Statement {
public:
    ForStatement(const char *id, Expression *s, Expression *e, Expression *t, Block *b) : start(s), end(e), step(t), body(b) { 
        identifier = new char[1 + strlen(id)];
        strcpy(identifier, id);
    }
    ~ForStatement();
    virtual void execute();
private:
    char *identifier;
    Expression *start;
    Expression *end;
    Expression *step;
    Block *body;
};

typedef std::pair<char *, Basic::ArrayDimension*> Dimensionable;

class Basic::DimStatement : public Statement {
public:
    DimStatement(const char *id, ArrayDimension *dim) { 
        char *identifier = new char[1 + strlen(id)];
        strcpy(identifier, id);
        dimensionables.push_back(std::make_pair(identifier, dim));
    }
    ~DimStatement();
    virtual void execute();
    void appendDimensionable(const char *id, ArrayDimension *dim) {
        char *identifier = new char[1 + strlen(id)];
        strcpy(identifier, id);
        dimensionables.push_back(std::make_pair(identifier, dim));
    }
private:
    std::list<Dimensionable> dimensionables;
};


#endif
