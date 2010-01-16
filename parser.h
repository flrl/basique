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

#include "tokeniser.h"
#include "variant.h"

class Parser {
public:
    Parser(void);
    
private:
    Token token;
    Tokeniser tokeniser;
    
    int accept(Token t);
    int expect(Token t);
    void error(int, ...);
    
    void unit(void);
    void statement(void);
    void dimBody(void);
    void block(void);
    void doBody(void);
    void functionDefinition(void);
    void subDefinition(void);
    void acceptedParamList(void);
    void acceptedParam(void);
    void paramList(void);
    void primaryExpression(void);
    void unaryExpression(void);
    void multiplicativeExpression(void);
    void additiveExpression(void);
    void comparitiveExpression(void);
    void andExpression(void);
    void orExpression(void);
    void expression(void);
    void type(void);
};

// <unit> ::= "function" <function-definition>
//          | "sub" <sub-definition>
//          | <statement>
// <statement> ::= "print" <expression> [ "," <expression> ]... [ ";" <expression> [ "," <expression> ]... ]... [ ";" ]
//               | "input" <identifier> [ "," <identifier> ]...
//               | [ "let" ] <identifier> [ "[" <expression> "]" ] "=" <expression>
//               | [ "call" ] <identifier> "(" <param-list> ")"
//               | "if" <expression> "then" <block> [ "elseif" <expression> "then" <block> ]... [ "else" <block> ] "end" "if"
//               | "do" <do-body>
//               | "for" <identifier> "=" <expression> "to" <expression> [ "step" <expression> ] <block> "next" [ <identifier> ]
//               | "dim" <dim-body> [ "," <dim-body> ]...
//               | "end"
// <dim-body> ::= <identifier> [ "[" <expression> [ "," <expression> ] "]" ]
// <block> ::= <statement> [ ( ":" | <eol> ) <statement> ]...
// <do-body> ::= ( "while" | "until" ) <expression> <block> "loop"
//             | <block> "loop" ( "while" | "until" ) <expression>
// <function-definition> ::= <identifier> "(" [ <accepted-param-list> ] ")" [ "as" <type> ] <block> "end" "function"
// <sub-definition> ::= <identifier> "(" [ <accepted-param-list> ] ")" <block> "end" "sub"
// <accepted-param-list> ::= <accepted-param> [ "," <accepted-param> ]...
// <accepted-param> ::= <identifier> [ "as" <type> ] | <null>
// <param-list> ::= <expression> [ "," <expression> ]...
// <primary-expression> ::= <identifier> "(" <param-list> ")"
//                        | <identifier> "[" <expression> [ "," <expression> ] "]"
//                        | <identifier>  
//                        | "(" <expression> ")"
//                        | <literal>
// <unary-expression> ::= <unary-operator> <primary-expression> | <primary-expression>
// <unary-operator> ::= "not" | "-"
// <multiplicative-expression> ::= <unary-expression> [ <multiplicative-operator> <unary-expression> ]...
// <multiplicative-operator> ::= "*" | "/" | "mod"
// <additive-expression> ::= <multiplicative-expression> [ <additive-operator> <multiplicative-expression> ]...
// <additive-operator> ::= "+" | "-"
// <comparitive-expression> ::= <additive-expression> [ <comparitive-operator> <additive-expression> ]...
// <comparitive-operator> ::= "=" | "<>" | "<" | ">" | "<=" | ">="
// <and-expression> ::= <comparitive-expression> [ "and" <comparitive-expression> ]...
// <or-expression> ::= <and-expression> [ "or" <and-expression> ]...
// <expression> ::= <or-expression>
// <type> ::= "integer" | "real" | "string"


#endif
