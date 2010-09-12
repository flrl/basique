/*
 *  ast.h
 *  Untitled
 *
 *  Created by Daniel on 16/01/10.
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

    struct OperatorTermWrapper;
}

// n.b. This *doesn't* claim ownership of the Expression object.  The owner of an 
// OperatorTermWrapper object *must* explicitly delete the contained Expression 
// object when it's no longer needed.
struct Basic::OperatorTermWrapper {
    OperatorTermWrapper(Token op, Expression *term) : op(op), term(term) { }
    Token op;
    Expression *term;
};

class Basic::ASTNode {
public:
    virtual ~ASTNode() { }
    virtual void execute(void) const =0;
    int getLine(void) { return m_line; }
    int getColumn(void) { return m_column; }
    void setPosition(int line, int column) { m_line = line; m_column = column; }
protected:
    int m_line;
    int m_column;
};

class Basic::AcceptedParamList : public ASTNode {
public:
    AcceptedParamList() { }
    ~AcceptedParamList();
    virtual void execute() const;
    void appendIdentifier(const String &s) { m_identifiers.push_back(s); }
private:
    std::list<String> m_identifiers;
};

class Basic::Block : public ASTNode {
public:
    Block() { }
    ~Block();
    virtual void execute() const;
    void appendStatement(Statement *s) { m_statements.push_back(s); }
private:
    std::list<Statement *> m_statements;
};

class Basic::FunctionDefinition : public ASTNode {
public:
    FunctionDefinition(const String &identifier, AcceptedParamList *a, Block *b) : m_identifier(identifier), m_accepted_params(a), m_body(b) { }
    ~FunctionDefinition() { delete m_accepted_params; delete m_body; }
    virtual void execute() const;
    void call(const ParamList *) const;
    Variant getResult() const { return m_result; }
private:
    const String m_identifier;
    AcceptedParamList *m_accepted_params;
    Block *m_body;
    Variant m_result;
};

class Basic::SubDefinition : public ASTNode {
public:
    SubDefinition(const String &identifier, AcceptedParamList *a, Block *b) : m_identifier(identifier), m_accepted_params(a), m_body(b) { }
    ~SubDefinition() { delete m_accepted_params; delete m_body; }
    virtual void execute() const;
    void call(const ParamList *) const;
private:
    const String m_identifier;
    AcceptedParamList *m_accepted_params;
    Block *m_body;
};

class Basic::ParamList : public ASTNode {
public:
    ParamList() { }
    ~ParamList();
    virtual void execute() const;
    void appendExpression(Expression *e) { m_expressions.push_back(e); }
    size_t size() const { return m_expressions.size(); }
    const Expression *param(size_t index) const { return m_expressions[index]; }
private:
    std::vector<Basic::Expression *> m_expressions;
};

class Basic::ArraySubscript : public ASTNode {
public:
    ArraySubscript(Expression *e) { m_expressions.push_back(e); }
    ~ArraySubscript();
    virtual void execute() const;
    void appendExpression(Expression *e) { m_expressions.push_back(e); }
private:
    std::list<Expression *> m_expressions;
};

class Basic::ArrayDimension : public ASTNode {
public:
    typedef std::pair<Expression*, Expression*> Specification;    
    
    ArrayDimension(Expression *d1, Expression *d2) { m_dimensions.push_back(std::make_pair(d1, d2)); }
    ~ArrayDimension();
    virtual void execute() const;
    void appendDimension(Expression *d1, Expression *d2) { m_dimensions.push_back(std::make_pair(d1, d2)); }
private:
    std::list<ArrayDimension::Specification> m_dimensions;
};

class Basic::Expression : public ASTNode {
public:
    Expression() { }
    virtual ~Expression() { }
    Variant getResult() const { return m_result; }
protected:
    mutable Variant m_result;
};

class Basic::IdentifierExpression : public Expression {
public:
    IdentifierExpression(const String &id, ParamList *p=NULL) : m_identifier(id), m_params(p) { }
    ~IdentifierExpression() { if (m_params) delete m_params; }
    virtual void execute() const;
private:
    const String m_identifier;
    ParamList *m_params;
};

class Basic::LiteralExpression : public Expression {
public:
    LiteralExpression(Variant &v) { m_result = v; }
    virtual void execute() const;
};

class Basic::UnaryExpression : public Expression {
public:
    UnaryExpression(Token t, Expression *e) : m_op(t), m_term(e) { }
    ~UnaryExpression() { delete m_term; }
    virtual void execute() const;
private:
    Token m_op;
    Expression *m_term;
};

class Basic::MultiplicativeExpression : public Expression {
public:
    MultiplicativeExpression(Expression *e) : m_first_term(e) { }
    ~MultiplicativeExpression();
    virtual void execute() const;
    void appendTerm(Token op, Expression *term) { m_other_terms.push_back(OperatorTermWrapper(op, term)); }
private:
    Expression *m_first_term;
    std::list<OperatorTermWrapper> m_other_terms;
};

class Basic::AdditiveExpression : public Expression {
public:
    AdditiveExpression(Expression *e) : m_first_term(e) { }
    ~AdditiveExpression();
    virtual void execute() const;
    void appendTerm(Token op, Expression *term) { m_other_terms.push_back(OperatorTermWrapper(op, term)); }
private:
    Expression* m_first_term;
    std::list<OperatorTermWrapper> m_other_terms;
};

class Basic::ComparitiveExpression : public Expression {
public:
    ComparitiveExpression(Expression *first, Token cmp, Expression *second) : m_first(first), m_cmp(cmp), m_second(second) { }
    ~ComparitiveExpression() { delete m_first; delete m_second; }
    virtual void execute() const;
private:
    Expression *m_first;
    Token m_cmp;
    Expression *m_second;
};

class Basic::AndExpression : public Expression {
public:
    AndExpression() { }
    ~AndExpression();
    virtual void execute() const;
    void appendTerm(Expression *term) { m_terms.push_back(term); }
private:
    std::list<Expression *> m_terms;
};

class Basic::OrExpression : public Expression {
public:
    OrExpression() { }
    ~OrExpression();
    virtual void execute() const;
    void appendTerm(Expression *term) { m_terms.push_back(term); }
private:
    std::list<Expression *> m_terms;
};


class Basic::Statement : public ASTNode { 
public:
    virtual ~Statement() { }
};

class Basic::PrintStatement : public Statement {
public:
    PrintStatement() : m_append_eol(true) { }
    ~PrintStatement();
    virtual void execute() const;
    void appendExpression(Expression *e) { m_expressions.push_back(e); }
    void setAppendEol(bool b) { m_append_eol = b; }
    
private:
    std::list<Expression *> m_expressions;
    bool m_append_eol;
};

class Basic::InputStatement : public Statement {
public:
    InputStatement(const String &id, ArraySubscript *s, Expression *e) : m_identifier(id), m_subscript(s), m_prompt(e) { }
    ~InputStatement() { if (m_subscript) delete m_subscript; if (m_prompt) delete m_prompt; }
    virtual void execute() const;
private:
    const String m_identifier;
    ArraySubscript *m_subscript;
    Expression *m_prompt;
};

class Basic::LetStatement : public Statement {
public:
    LetStatement(const String &id, ArraySubscript *s, Expression *e) : m_identifier(id), m_subscript(s), m_expression(e) { }
    ~LetStatement() { if (m_subscript) delete m_subscript; delete m_expression; }
    virtual void execute() const;
private:
    const String m_identifier;
    ArraySubscript *m_subscript;
    Expression *m_expression;
};

class Basic::CallStatement : public Statement {
public:
    CallStatement(const String &id, ParamList *params) : m_identifier(id), m_params(params) { }
    ~CallStatement() { if (m_params) delete m_params; }
    virtual void execute() const;
private:
    const String m_identifier;
    ParamList *m_params;
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
    virtual void execute() const;
    void appendCondition(Expression *condition, Block *block) { m_conditional_blocks.push_back(ConditionalBlock(condition, block)); }
    void setElseBlock(Block *block) { if (m_else_block) delete m_else_block; m_else_block = block; }
private:
    std::list<ConditionalBlock> m_conditional_blocks;
    Block *m_else_block;
};

class Basic::DoStatement : public Statement {
public:
    enum Type { DcWHILE, DcUNTIL, DcFOREVER, DcONCE };
    enum When { DcPRECONDITION, DcPOSTCONDITION };
    
    DoStatement(Type t, When w, Expression *c, Block *b) 
        : m_condition_type(t), m_condition_when(w), m_condition(c), m_body(b) { }
    ~DoStatement() { if (m_condition) delete m_condition; delete m_body; }
    virtual void execute() const;
private:
    Type m_condition_type;
    When m_condition_when;
    Expression *m_condition;
    Block *m_body;
};

class Basic::ForStatement : public Statement {
public:
    ForStatement(const String &id, Expression *s, Expression *e, Expression *t, Block *b) 
        : m_identifier(id), m_start(s), m_end(e), m_step(t), m_body(b) { }
    ~ForStatement();
    virtual void execute() const;
private:
    const String m_identifier;
    Expression *m_start;
    Expression *m_end;
    Expression *m_step;
    Block *m_body;
};

typedef std::pair<const String, Basic::ArrayDimension*> Dimensionable;

class Basic::DimStatement : public Statement {
public:
    DimStatement(const String &identifier, ArrayDimension *dim) { 
        m_dimensionables.push_back(std::make_pair(identifier, dim));
    }
    ~DimStatement();
    virtual void execute() const;
    void appendDimensionable(const String &identifier, ArrayDimension *dim) {
        m_dimensionables.push_back(std::make_pair(identifier, dim));
    }
private:
    std::list<Dimensionable> m_dimensionables;
};


#endif
