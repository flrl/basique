/*
 *  parser.h
 *  Untitled
 *
 *  Created by Ellie on 10/01/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef BASIQUE_PARSER_H
#define BASIQUE_PARSER_H

#include "ast.h"
#include "symboltable.h"
#include "tokeniser.h"
#include "variant.h"

namespace basic {
    class Parser;
}

class basic::Parser {
public:
    Parser(Tokeniser *tokeniser) : m_tokeniser(tokeniser) { m_token = m_tokeniser->getToken(); }
    ~Parser() {}
    
    Unit *parse() { return unit(); }
    
private:
    Tokeniser   *m_tokeniser;
    Token       m_token;
    Variant     m_accepted_token_value;
    int         m_accepted_token_line;
    int         m_accepted_token_column;
    Token       m_accepted_token;
    
    bool accept(Token t);
    bool expect(Token t);
    void error(int, ...);
    bool isValidExpressionToken(Token t) const;
        
    Unit* unit(void);
    ArraySubscript* arraySubscript(void);
    ArrayDimension* arrayDimension(void);
    FileHandle* fileHandle();
    Statement* statement(void);
    Block* block(void);
    FunctionDefinition* functionDefinitionBody(void);
    SubDefinition* subDefinitionBody(void);
    AcceptedParamList* acceptedParamList(void);
    ParamList* paramList(void);
    PrintStatement* printStatementBody(void);
    InputStatement* inputStatementBody(void);
    LetStatement* letStatementBody(void);
    CallStatement* callStatementBody(void);
    IfStatement* ifStatementBody(void);
    DoStatement* doStatementBody(void);
    ForStatement* forStatementBody(void);
    DimStatement* dimStatementBody(void);
    OpenStatement* openStatementBody();
    CloseStatement* closeStatementBody();
    
    Expression* primaryExpression(void);
    Expression* unaryExpression(void);
    Expression* multiplicativeExpression(void);
    Expression* additiveExpression(void);
    Expression* comparitiveExpression(void);
    Expression* andExpression(void);
    Expression* orExpression(void);
    Expression* expression(void);
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
//               | "open" <open-statement-body>
//               | "close" <close-statement-body>
//               | "exit" [ <expression> ]
// <array-subscript> ::= "(" <expression> [ "," <expression> ]... ")"
// <array-dimension> ::= "(" <expression> [ "to" <expression> ] [ "," <expression> [ "to" <expression> ] ]... ")"
// <print-statement-body> ::= [ <file-handle> ] <print-expression-list>
// <print-expression-list> ::= <expression> [ "," <expression> ]... [ "," ]
//                           | <null>
// <input-statement-body> ::= [ <file-handle> ] <identifier> [ <array-subscript> ] [ "," <expression> ]
// <let-statement-body> ::= <identifier> [ <array-subscript> ] "=" <expression>
// <call-statement-body> ::= <identifier> "(" <param-list> ")"
// <if-statement-body> ::= <expression> "then" <block> [ "elseif" <expression> "then" <block> ]... [ "else" <block> ] "end" "if"
// <do-statement-body> ::= ( "while" | "until" ) <expression> <block> "loop"
//                       | <block> "loop" ( "while" | "until" ) <expression>
//                       | <block> "loop"
//                       | <block> "done"
// <for-statement-body> ::= <identifier> "=" <expression> "to" <expression> [ "step" <expression> ] <block> "next" [ <identifier> ]
// <dim-statement-body> ::= <identifier> [ <array-dimension> ] [ "," <identifier> [ <array-dimension> ] ]...
// <open-statement-body> ::= <expression> "for" ( "input" | "output" | "append" ) "as" <file-handle>
// <close-statement-body> ::= <file-handle>
// <file-handle> ::= "#" <expression>
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
