/*
 *  parser.cpp
 *  Untitled
 *
 *  Created by Daniel on 10/01/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include <cstdarg>
#include <cstdio>

#include "parser.h"

int Parser::accept(Token t) {
    if (this->token == t) {
        this->accepted_token_value = tokeniser.getValue();
        this->accepted_token_line = tokeniser.getLine();
        this->accepted_token_column = tokeniser.getColumn();
        this->token = tokeniser.getToken();
        return 1;
    }
    else {
        return 0;
    }
}

int Parser::expect(Token t) {
    if (accept(t))  return 1;
    error(1, t);
    return 0;
}

void Parser::error(int argc, ...) {
    fprintf(stderr, "Syntax error at line %i, column %i: expected", tokeniser.getLine(), tokeniser.getColumn());
    va_list argv;
    va_start(argv, argc);
    for (int i=0; i<argc; i++) {
        int token = va_arg(argv, int);
        if (token >= Tk_MIN && token < Tk_MAX) {
            fprintf(stderr, " `%s',", Tokeniser::tokenDescriptions[va_arg(argv, int)]);
        }
    }
    va_end(argv);
    fprintf(stderr, " got `%s'\n", Tokeniser::tokenDescriptions[this->token]);
}


// <unit> ::= "function" <function-definition>
//          | "sub" <sub-definition>
//          | <statement>
void Parser::unit(void) {
    if (accept(TkFUNCTION)) {
        functionDefinition();
    }
    else if (accept(TkSUB)) {
        subDefinition();
    }
    else {
        statement();
    }
}

// <statement> ::= "print" <print-expression>
//               | "input" <identifier> [ "[" <expression> [ "," <expression> ] "]" ] [ "," <identifier> [ "["  <expression> [ "," <expression> ] "]" ] ]...
//               | [ "let" ] <identifier> [ "[" <expression> [ "," <expression> ] "]" ] "=" <expression>
//               | [ "call" ] <identifier> "(" <param-list> ")"
//               | "if" <expression> "then" <block> [ "elseif" <expression> "then" <block> ]... [ "else" <block> ] "end" "if"
//               | "do" <do-body>
//               | "for" <identifier> "=" <expression> "to" <expression> [ "step" <expression> ] <block> "next" [ <identifier> ]
//               | "dim" <dim-body> [ "," <dim-body> ]...
//               | "exit" [ <expression> ]
//               | <null>
void Parser::statement(void) {
    if (accept(TkPRINT)) {
        do {
            do {
                expression();
            } while (accept(TkCOMMA));
        } while (accept(TkSEMICOLON));
        accept(TkSEMICOLON);
    }
    else if (accept(TkINPUT)) {
        do {
            expect(TkIDENTIFIER);
            if (accept(TkLBRACKET)) {
                expression();
                if (accept(TkCOMMA)) {
                    expression();
                }
                expect(TkRBRACKET);
            }
        } while (accept(TkCOMMA));
    }
    else if (accept(TkLET)) {
        expect(TkIDENTIFIER);
        expect(TkEQUALS);
        if (accept(TkLBRACKET)) {
            expression();
            if (accept(TkCOMMA)) {
                expression();
            }
            expect(TkRBRACKET);
        }
        expression();
    }
    else if (accept(TkCALL)) {
        expect(TkIDENTIFIER);
        expect(TkLPAREN);
        paramList();
        expect(TkRPAREN);
    }
    else if (accept(TkIDENTIFIER)) {
        if (accept(TkEQUALS)) {
            expression();
        }
        else if (accept(TkLPAREN)) {
            paramList();
            expect(TkRPAREN);
        }
        else if (accept(TkLBRACKET)) {
            expression();
            expect(TkRBRACKET);
            expect(TkEQUALS);
            expression();
        }
        else {
            error(3, TkEQUALS, TkLPAREN, TkLBRACKET);
        }
    }
    else if (accept(TkIF)) {
        expression();
        expect(TkTHEN);
        block();
        while(accept(TkELSEIF)) {
            expression();
            expect(TkTHEN);
            block();
        }
        if (accept(TkELSE)) {
            block();
        }
        expect(TkEND);
        expect(TkIF);
    }
    else if (accept(TkDO)) {
        doBody();
    }
    else if (accept(TkFOR)) {
        expect(TkIDENTIFIER);
        expect(TkEQUALS);
        expression();
        expect(TkTO);
        expression();
        if (accept(TkSTEP)) {
            expression();
        }
        block();
        expect(TkNEXT);
        accept(TkIDENTIFIER);
    }
    else if (accept(TkDIM)) {
        do {
            dimBody();
        } while (accept(TkCOMMA));
    }
    else if (accept(TkEXIT)) {
        expression();
    }
}

// <dim-body> ::= <identifier> [ "[" <expression> [ "," <expression> ] "]" ]
void Parser::dimBody(void) {
    expect(TkIDENTIFIER);
    if (accept(TkLBRACKET)) {
        expression();
        if (accept(TkCOMMA)) {
            expression();
        }
        expect(TkRBRACKET);
    }
}

// <block> ::= <statement> [ ( ":" | <eol> ) <statement> ]...
void Parser::block(void) {
    do {
        statement();
    } while (accept(TkCOLON) || accept(TkEOL));
}
        
// <do-body> ::= ( "while" | "until" ) <expression> <block> "loop"
//             | <block> "loop" ( "while" | "until" ) <expression>
//             | <block> "done"
void Parser::doBody(void) {
    if (accept(TkWHILE)) {
        expression();
        block();
        expect(TkLOOP);        
    }
    else if (accept(TkUNTIL)) {
        expression();
        block();
        expect(TkLOOP);
    }
    else {
        block();
        if (accept(TkLOOP)) {
            if (accept(TkWHILE)) {
                expression();            
            }
            else if (accept(TkUNTIL)) {
                expression();
            }
            else {
                error(2, TkWHILE, TkUNTIL);
            }
        }
        else if (accept(TkDONE)) {
            ;
        }
        else {
            error(2, TkLOOP, TkDONE);
        }
    }
}

// <function-definition> ::= <identifier> "(" [ <accepted-param-list> ] ")" [ "as" <type> ] <block> "end" "function"
void Parser::functionDefinition(void) {
    expect(TkIDENTIFIER);
    expect(TkLPAREN);
    acceptedParamList();
    expect(TkRPAREN);
    if (accept(TkAS)) {
        type();
    }
    block();
    expect(TkEND);
    expect(TkFUNCTION);
}

// <sub-definition> ::= <identifier> "(" [ <accepted-param-list> ] ")" <block> "end" "sub"
void Parser::subDefinition(void) {
    expect(TkIDENTIFIER);
    expect(TkLPAREN);
    acceptedParamList();
    expect(TkRPAREN);
    block();
    expect(TkEND);
    expect(TkSUB);
}

// <accepted-param-list> ::= <accepted-param> [ "," <accepted-param> ]...
void Parser::acceptedParamList(void) {
    do {
        acceptedParam();
    } while (accept(TkCOMMA));
}

// <accepted-param> ::= <identifier> [ "as" <type> ] | <null>
void Parser::acceptedParam(void) {
    if (accept(TkIDENTIFIER)) {
        if (accept(TkAS)) {
            type();
        }
    }
}

// <param-list> ::= <expression> [ "," <expression> ]...
void Parser::paramList(void) {
    do {
        expression();
    } while (accept(TkCOMMA));
}

// <print-expression> ::= <expression> [ "," <print-expression> ]...
//                      | <null>
void Parser::printExpression(void) {
    // peek ahead for something that looks like the start of an expression before trying to parse one
    if (this->token == TkIDENTIFIER or this->token == TkLPAREN or this->token == TkLITERAL) {
        expression();
        if (accept(TkCOMMA)) {
            printExpression();
        }
    }
    else {
        ;
    }
}

// <primary-expression> ::= <identifier> "(" <param-list> ")"
//                        | <identifier> "[" <expression> [ "," <expression> ] "]"
//                        | <identifier>  
//                        | "(" <expression> ")"
//                        | <literal>
void Parser::primaryExpression(void) {
    if(accept(TkIDENTIFIER)) {
        if(accept(TkLPAREN)) {
            paramList();
            expect(TkRPAREN);
        }
        else if (accept(TkLBRACKET)) {
            expression();
            if (accept(TkCOMMA)) {
                expression();
            }
            expect(TkRBRACKET);
        }
        else {
            ;
        }
    }
    else if (accept(TkLPAREN)) {
        expression();
        expect(TkRPAREN);
    }
    else if (accept(TkLITERAL)) {
        ;
    }
    else {
        error(3, TkIDENTIFIER, TkLPAREN, TkLITERAL);
    }
}

// <unary-expression> ::= <unary-operator> <primary-expression> | <primary-expression>
// <unary-operator> ::= "not" | "-"
void Parser::unaryExpression(void) {
    if (accept(TkNOT)) {
        primaryExpression();        
    }
    else if (accept(TkMINUS)) {
        primaryExpression();
    }
    else {
        primaryExpression();
    }
}

// <multiplicative-expression> ::= <unary-expression> [ <multiplicative-operator> <unary-expression> ]...
// <multiplicative-operator> ::= "*" | "/" | "mod"
void Parser::multiplicativeExpression(void) {
    do {
        unaryExpression();
    } while (accept(TkMULTIPLY) || accept(TkDIVIDE) || accept(TkMOD));
}

// <additive-expression> ::= <multiplicative-expression> [ <additive-operator> <multiplicative-expression> ]...
// <additive-operator> ::= "+" | "-"
void Parser::additiveExpression(void) {
    do {
        multiplicativeExpression();
    } while (accept(TkPLUS) || accept(TkMINUS));
}

// <comparitive-expression> ::= <additive-expression> [ <comparitive-operator> <additive-expression> ]...
// <comparitive-operator> ::= "=" | "<>" | "<" | ">" | "<=" | ">="
void Parser::comparitiveExpression(void) {
    do {
        additiveExpression();
    } while (accept(TkEQUALS) || accept(TkNOTEQUALS) || accept(TkLT) || accept(TkGT) || accept(TkLTEQUALS) || accept(TkGTEQUALS));
}

// <and-expression> ::= <comparitive-expression> [ "and" <comparitive-expression> ]...
void Parser::andExpression(void) {
    do {
        comparitiveExpression();
    } while (accept(TkAND));
}

// <or-expression> ::= <and-expression> [ "or" <and-expression> ]...
void Parser::orExpression(void) {
    do {
        andExpression();
    } while (accept(TkOR));
}

// <expression> ::= <or-expression>
void Parser::expression(void) {
    orExpression();
}

// <type> ::= "integer" | "real" | "string"
void Parser::type(void) {
    if (accept(TkINTEGER)) {
        ;
    }
    else if (accept(TkREAL)) {
        ;
    }
    else if (accept(TkSTRING)) {
        ;
    }
    else {
        error(3, TkINTEGER, TkREAL, TkSTRING);
    }    
}
