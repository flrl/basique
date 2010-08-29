/*
 *  parser.h
 *  Untitled
 *
 *  Created by Daniel on 10/01/10.
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
    Token token;
    Tokeniser tokeniser;
    
    Variant accepted_token_value;
    int accepted_token_line;
    int accepted_token_column;
    
    int accept(Token t);
    int expect(Token t);
    void error(int, ...);
    
    void unit(void);
    Basic::Statement* statement(void);
    void dimBody(void);
    Basic::Block* block(void);
    void doStatementBody(void);
    void functionDefinition(void);
    void subDefinition(void);
    void acceptedParamList(void);
    void acceptedParam(void);
    Basic::ParamList* paramList(void);
    Basic::PrintStatement* printStatementBody(void);
    Basic::InputStatement* inputStatementBody(void);
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

// <unit> ::= "function" <function-definition>
//          | "sub" <sub-definition>
//          | <statement>
// <statement> ::= "print" <print-statement-body>
//               | "input" <input-statement-body>
//               | [ "let" ] <identifier> [ <subscript> ] "=" <expression>
//               | [ "call" ] <call>
//               | "if" <expression> "then" <block> [ "elseif" <expression> "then" <block> ]... [ "else" <block> ] "end" "if"
//               | "do" <do-statement-body>
//               | "for" <identifier> "=" <expression> "to" <expression> [ "step" <expression> ] <block> "next" [ <identifier> ]
//               | "dim" <dim-body> [ "," <dim-body> ]...
//               | "exit" [ <expression> ]
//               | <null>
// <subscript> ::= "(" <expression> [ "," <expression> ]... ")"
// <call> ::= <identifier> "(" <param-list> ")"
// <print-statement-body> ::= <print-expression-list>
// <print-expression-list> ::= <expression> [ "," <print-expression-list> ]...
//                           | <null>
// FIXME <input-statement-body> ::= <identifier> [ "[" <expression> [ "," <expression> ] "]" ] [ "," <identifier> [ "["  <expression> [ "," <expression> ] "]" ] ]...
// <input-statement-body> ::= <identifier> [ "," <identifier> ]...
// <dim-body> ::= <identifier> [ "[" <expression> [ "," <expression> ] "]" ]
// <block> ::= <statement> [ ( ":" | <eol> ) <statement> ]...
// <do-statement-body> ::= ( "while" | "until" ) <expression> <block> "loop"
//                       | <block> "loop" ( "while" | "until" ) <expression>
//                       | <block> "done"
// <function-definition> ::= <identifier> "(" [ <accepted-param-list> ] ")" [ "as" <type> ] <block> "end" "function"
// <sub-definition> ::= <identifier> "(" [ <accepted-param-list> ] ")" <block> "end" "sub"
// <accepted-param-list> ::= <accepted-param> [ "," <accepted-param> ]...
// <accepted-param> ::= <identifier> [ "as" <type> ] | <null>
// <param-list> ::= <expression> [ "," <expression> ]...
// <primary-expression> ::= <call>
//                        | <identifier>  
//                        | "(" <expression> ")"
//                        | <literal>
// <call> ::= <identifier> "(" <param-list> ")"
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
