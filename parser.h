/*
 *  parser.h
 *  Untitled
 *
 *  Created by Ellie on 10/01/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _PARSER_H
#define _PARSER_H

#include "ast.h"
#include "tokeniser.h"
#include "variant.h"

class Parser {
public:
    Parser(void);
    
private:
    Basic::Token token;
    Basic::Tokeniser tokeniser;
    
    Basic::Variant accepted_token_value;
    int accepted_token_line;
    int accepted_token_column;
    
    int accept(Basic::Token t);
    int expect(Basic::Token t);
    void error(int, ...);
    
    Basic::ASTNode* unit(void);
    Basic::ArraySubscript* arraySubscript(void);
    Basic::ArrayDimension* arrayDimension(void);
    Basic::Statement* statement(void);
    Basic::Block* block(void);
    Basic::FunctionDefinition* functionDefinitionBody(void);
    Basic::SubDefinition* subDefinitionBody(void);
    Basic::AcceptedParamList* acceptedParamList(void);
    Basic::ParamList* paramList(void);
    Basic::PrintStatement* printStatementBody(void);
    Basic::InputStatement* inputStatementBody(void);
    Basic::LetStatement* letStatementBody(void);
    Basic::CallStatement* callStatementBody(void);
    Basic::IfStatement* ifStatementBody(void);
    Basic::DoStatement* doStatementBody(void);
    Basic::ForStatement* forStatementBody(void);
    Basic::DimStatement* dimStatementBody(void);
    
    Basic::Expression* primaryExpression(void);
    Basic::Expression* unaryExpression(void);
    Basic::Expression* multiplicativeExpression(void);
    Basic::Expression* additiveExpression(void);
    Basic::Expression* comparitiveExpression(void);
    Basic::Expression* andExpression(void);
    Basic::Expression* orExpression(void);
    Basic::Expression* expression(void);
    void type(void);
};

// <unit> ::= "function" <function-definition-body> <eol>
//          | "sub" <sub-definition-body> <eol>
//          | <statement> <eol>
// <statement> ::= "print" <print-statement-body>
//               | "input" <input-statement-body>
//               | "let" <let-statement-body>
//               | "call" <call-statement-body>
//               | "if" <if-statement-body>
//               | "do" <do-statement-body>
//               | "for" <for-statement-body>
//               | "dim" <dim-statement-body>
//               | "exit" [ <expression> ]
// <array-subscript> ::= "(" <expression> [ "," <expression> ]... ")"
// <array-dimension> ::= "(" <expression> [ "to" <expression> ] [ "," <expression> [ "to" <expression> ] ]... ")"
// <print-statement-body> ::= <print-expression-list>
// <print-expression-list> ::= <expression> [ "," <expression> ]... [ "," ]
//                           | <null>
// <input-statement-body> ::= <identifier> [ <array-subscript> ] [ "," <expression> ]
// <let-statement-body> ::= <identifier> [ <array-subscript> ] "=" <expression>
// <call-statement-body> ::= <identifier> "(" <param-list> ")"
// <if-statement-body> ::= <expression> "then" <block> [ "elseif" <expression> "then" <block> ]... [ "else" <block> ] "end" "if"
// <do-statement-body> ::= ( "while" | "until" ) <expression> <block> "loop"
//                       | <block> "loop" ( "while" | "until" ) <expression>
//                       | <block> "loop"
//                       | <block> "done"
// <for-statement-body> ::= <identifier> "=" <expression> "to" <expression> [ "step" <expression> ] <block> "next" [ <identifier> ]
// <dim-statement-body> ::= <identifier> [ <array-dimension> ] [ "," <identifier> [ <array-dimension> ] ]...
// <block> ::= <block-statement-list>
// <block-statement-list> ::= <statement> [ ( ":" | <eol> ) <block-statement-list> ]...
//                          | <null>
// <function-definition-body> ::= <identifier> "(" <accepted-param-list> ")" <block> "end" "function"
// <sub-definition-body> ::= <identifier> "(" <accepted-param-list> ")" <block> "end" "sub"
// <accepted-param-list> ::= <identifier> [ "," <identifier> ]...
//                         | <null> 
// <param-list> ::= <expression> [ "," <expression> ]... | <null>
// <primary-expression> ::= <literal>
//                        | <identifier> [ "(" <param-list> ")" ]
//                        | "(" <expression> ")"
// <unary-expression> ::= <unary-operator> <primary-expression> | <primary-expression>
// <unary-operator> ::= "not" | "-"
// <multiplicative-expression> ::= <unary-expression> [ <multiplicative-operator> <unary-expression> ]...
// <multiplicative-operator> ::= "*" | "/" | "mod"
// <additive-expression> ::= <multiplicative-expression> [ <additive-operator> <multiplicative-expression> ]...
// <additive-operator> ::= "+" | "-"
// <comparitive-expression> ::= <additive-expression> [ <comparitive-operator> <additive-expression> ]
// <comparitive-operator> ::= "=" | "<>" | "<" | ">" | "<=" | ">="
// <and-expression> ::= <comparitive-expression> [ "and" <comparitive-expression> ]...
// <or-expression> ::= <and-expression> [ "or" <and-expression> ]...
// <expression> ::= <or-expression>
// <type> ::= "integer" | "real" | "string"


#endif
