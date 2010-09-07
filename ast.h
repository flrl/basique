/*
 *  ast.h
 *  Untitled
 *
 *  Created by Ellie on 16/01/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _AST_H
#define _AST_H

#include <list>
#include <utility>

#include "string.h"
#include "tokeniser.h"
#include "variant.h"

namespace Basic {
    class ASTNode;

    class AcceptedParamList;
    class ParamList;
    class ArraySubscript;
    class ArrayDimension;
    
    class FunctionDefinition;
    class SubDefinition;
    
    class Expression;
    class LiteralExpression;
    class IdentifierExpression;
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

class Basic::FunctionDefinition : public ASTNode {
public:
    FunctionDefinition(const String &identifier, AcceptedParamList *a, Block *b) : identifier(identifier), accepted_params(a), body(b) { }
    ~FunctionDefinition();
    virtual void execute();
    void call();
private:
    const String identifier;
    AcceptedParamList *accepted_params;
    Block *body;
};

class Basic::SubDefinition : public ASTNode {
public:
    SubDefinition(const String &identifier, AcceptedParamList *a, Block *b) : identifier(identifier), accepted_params(a), body(b) { }
    ~SubDefinition();
    virtual void execute();
    void call();
private:
    const String identifier;
    AcceptedParamList *accepted_params;
    Block *body;
};

class Basic::Block : public ASTNode {
public:
    Block() { }
    ~Block();
    virtual void execute();
    void appendStatement(Statement *s) { statements.push_back(s); }
private:
    std::list<Statement *> statements;
};

class Basic::AcceptedParamList : public ASTNode {
public:
    AcceptedParamList() { }
    ~AcceptedParamList();
    virtual void execute();
    void appendIdentifier(const String &s) { identifiers.push_back(s); }
private:
    std::list<String> identifiers;
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

class Basic::IdentifierExpression : public Expression {
public:
    IdentifierExpression(const char *id, ParamList *p=NULL) : identifier(id), params(p) { }
    ~IdentifierExpression() { if (params) delete params; }
    virtual void execute();
private:
    const String identifier;
    ParamList *params;
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
    InputStatement(const char *id, ArraySubscript *s, Expression *e) : identifier(id), subscript(s), prompt(e) { }
    ~InputStatement() { if (subscript) delete subscript; if (prompt) delete prompt; }
    virtual void execute();
private:
    const String identifier;
    ArraySubscript *subscript;
    Expression *prompt;
};

class Basic::LetStatement : public Statement {
public:
    LetStatement(const char *id, ArraySubscript *s, Expression *e) : identifier(id), subscript(s), expression(e) { }
    ~LetStatement() { if (subscript) delete subscript; delete expression; }
    virtual void execute();
private:
    const String identifier;
    ArraySubscript *subscript;
    Expression *expression;
};

class Basic::CallStatement : public Statement {
public:
    CallStatement(const char *id, ParamList *params) : identifier(id), params(params) { }
    ~CallStatement() { if (params) delete params; }
    virtual void execute();
private:
    const String identifier;
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
    ForStatement(const char *id, Expression *s, Expression *e, Expression *t, Block *b) 
        : identifier(id), start(s), end(e), step(t), body(b) { }
    ~ForStatement();
    virtual void execute();
private:
    const String identifier;
    Expression *start;
    Expression *end;
    Expression *step;
    Block *body;
};

typedef std::pair<const String, Basic::ArrayDimension*> Dimensionable;

class Basic::DimStatement : public Statement {
public:
    DimStatement(const char *identifier, ArrayDimension *dim) { 
        dimensionables.push_back(std::make_pair(String(identifier), dim));
    }
    ~DimStatement();
    virtual void execute();
    void appendDimensionable(const char *identifier, ArrayDimension *dim) {
        dimensionables.push_back(std::make_pair(String(identifier), dim));
    }
private:
    std::list<Dimensionable> dimensionables;
};


#endif
