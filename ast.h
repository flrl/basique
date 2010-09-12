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
#include <vector>

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

    struct OperatorTerm;
}

// n.b. This *doesn't* claim ownership of the Expression object.  The owner of an OperatorTerm
// object *must* explicitly delete the contained Expression object when it's no longer needed.
struct Basic::OperatorTerm {
    OperatorTerm(Token op, Expression *term) : op(op), term(term) { }
    Token op;
    Expression *term;
};

class Basic::ASTNode {
public:
    virtual ~ASTNode() { }
    virtual void execute(void) =0;
    int getLine(void) { return line; }
    int getColumn(void) { return column; }
    void setPosition(int line, int column) { this->line = line; this->column = column; }
protected:
    int line;
    int column;
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

class Basic::Block : public ASTNode {
public:
    Block() { }
    ~Block();
    virtual void execute();
    void appendStatement(Statement *s) { statements.push_back(s); }
private:
    std::list<Statement *> statements;
};

class Basic::FunctionDefinition : public ASTNode {
public:
    FunctionDefinition(const String &identifier, AcceptedParamList *a, Block *b) : identifier(identifier), accepted_params(a), body(b) { }
    ~FunctionDefinition() { delete accepted_params; delete body; }
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
    ~SubDefinition() { delete accepted_params; delete body; }
    virtual void execute();
    void call();
private:
    const String identifier;
    AcceptedParamList *accepted_params;
    Block *body;
};

class Basic::ParamList : public ASTNode {
public:
    ParamList() { }
    ~ParamList();
    virtual void execute();
    void appendExpression(Expression *e) { expressions.push_back(e); }
    size_t size() const { return expressions.size(); }
    Expression *param (size_t index) { return expressions[index]; }
private:
    std::vector<Basic::Expression *> expressions;
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

class Basic::ArrayDimension : public ASTNode {
public:
    typedef std::pair<Expression*, Expression*> Specification;    
    
    ArrayDimension(Expression *d1, Expression *d2) { dimensions.push_back(std::make_pair(d1, d2)); }
    ~ArrayDimension();
    virtual void execute();
    void appendDimension(Expression *d1, Expression *d2) { dimensions.push_back(std::make_pair(d1, d2)); }
private:
    std::list<ArrayDimension::Specification> dimensions;
};

class Basic::Expression : public ASTNode {
public:
    Expression() { }
    virtual ~Expression() { }
    Variant getResult() { return value; }
protected:
    Variant value;
};

class Basic::IdentifierExpression : public Expression {
public:
    IdentifierExpression(const String &id, ParamList *p=NULL) : identifier(id), params(p) { }
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
    MultiplicativeExpression(Expression *e) : first_term(e) { }
    ~MultiplicativeExpression();
    virtual void execute();
    void appendTerm(Token op, Expression *term) { other_terms.push_back(OperatorTerm(op, term)); }
private:
    Expression *first_term;
    std::list<OperatorTerm> other_terms;
};

class Basic::AdditiveExpression : public Expression {
public:
    AdditiveExpression(Expression *e) : first_term(e) { }
    ~AdditiveExpression();
    virtual void execute();
    void appendTerm(Token op, Expression *term) { other_terms.push_back(OperatorTerm(op, term)); }
private:
    Expression* first_term;
    std::list<OperatorTerm> other_terms;
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


class Basic::Statement : public ASTNode { 
public:
    virtual ~Statement() { }
};

class Basic::PrintStatement : public Statement {
public:
    PrintStatement() : append_eol(true) { }
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
    InputStatement(const String &id, ArraySubscript *s, Expression *e) : identifier(id), subscript(s), prompt(e) { }
    ~InputStatement() { if (subscript) delete subscript; if (prompt) delete prompt; }
    virtual void execute();
private:
    const String identifier;
    ArraySubscript *subscript;
    Expression *prompt;
};

class Basic::LetStatement : public Statement {
public:
    LetStatement(const String &id, ArraySubscript *s, Expression *e) : identifier(id), subscript(s), expression(e) { }
    ~LetStatement() { if (subscript) delete subscript; delete expression; }
    virtual void execute();
private:
    const String identifier;
    ArraySubscript *subscript;
    Expression *expression;
};

class Basic::CallStatement : public Statement {
public:
    CallStatement(const String &id, ParamList *params) : identifier(id), params(params) { }
    ~CallStatement() { if (params) delete params; }
    virtual void execute();
private:
    const String identifier;
    ParamList *params;
};

class Basic::IfStatement : public Statement {
public:
    struct ConditionalBlock {
        ConditionalBlock(Expression *condition, Block *block) : condition(condition), block(block) { }
        Expression *condition;
        Block *block;
    };
    
    IfStatement() { }
    ~IfStatement();
    virtual void execute();
    void appendCondition(Expression *condition, Block *block) { conditional_blocks.push_back(ConditionalBlock(condition, block)); }
    void setElseBlock(Block *block) { if (else_block) delete else_block; else_block = block; }
private:
    std::list<ConditionalBlock> conditional_blocks;
    Block *else_block;
};

class Basic::DoStatement : public Statement {
public:
    enum Type { DcWHILE, DcUNTIL, DcFOREVER, DcONCE };
    enum When { DcPRECONDITION, DcPOSTCONDITION };
    
    DoStatement(Type t, When w, Expression *c, Block *b) 
        : condition_type(t), condition_when(w), condition(c), body(b) { }
    ~DoStatement() { if (condition)  delete condition; delete body; }
    virtual void execute();
private:
    Type condition_type;
    When condition_when;
    Expression *condition;
    Block *body;
};

class Basic::ForStatement : public Statement {
public:
    ForStatement(const String &id, Expression *s, Expression *e, Expression *t, Block *b) 
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
    DimStatement(const String &identifier, ArrayDimension *dim) { 
        dimensionables.push_back(std::make_pair(identifier, dim));
    }
    ~DimStatement();
    virtual void execute();
    void appendDimensionable(const String &identifier, ArrayDimension *dim) {
        dimensionables.push_back(std::make_pair(identifier, dim));
    }
private:
    std::list<Dimensionable> dimensionables;
};


#endif
