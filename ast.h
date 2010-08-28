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

#include "tokeniser.h"
#include "variant.h"

namespace Basic {
    class ASTNode;

    class ParamList;
    
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

    class Block;
    
    class Identifier;
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

class Basic::ParamList : public ASTNode {
public:
    ParamList() { }
    ~ParamList();
    virtual void execute();
    void appendExpression(Expression *e) { expressions.push_back(e); }
private:
    std::list<Expression *> expressions;
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
    
protected:
    std::list<Expression *> expressions;
    bool append_eol;
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





class Basic::ForStatement : public ASTNode {
public:
    ForStatement();
    virtual void execute();
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
    virtual void execute();
protected:
    DoConditionType conditionType;
    DoConditionWhen conditionWhen;
    Expression *condition;
    Block *body;
};


#endif
