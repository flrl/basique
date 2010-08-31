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
    if (accept(t)) {
        return 1;  
    } 
    else {
        error(1, t);
        return 0;        
    }
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

// <statement> ::= "print" <print-statement-body>
//               | "input" <input-statement-body>
//               | "let" <let-statement-body>
//               | "call" <call-statement-body>
//               | "if" <if-statement-body>
//               | "do" <do-statement-body>
//               | "for" <identifier> "=" <expression> "to" <expression> [ "step" <expression> ] <block> "next" [ <identifier> ]
//               | "dim" <dim-body> [ "," <dim-body> ]...
//               | "exit" [ <expression> ]
//               | <let-statement-body>
//               | <call-statement-body>
//               | <null>
Basic::Statement* Parser::statement(void) {
    if (accept(TkPRINT)) {
        return printStatementBody();
    }
    else if (accept(TkINPUT)) {
        return inputStatementBody();
    }
    else if (accept(TkLET)) {
        return letStatementBody();
    }
    else if (accept(TkCALL)) {
        return callStatementBody();
    }
    else if (accept(TkIF)) {
        return ifStatementBody();
    }
    else if (accept(TkDO)) {
        doStatementBody();
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
    else if (accept(TkIDENTIFIER)) {
        //        if (accept(TkEQUALS)) {
        //            expression();
        //        }
        //        else if (accept(TkLPAREN)) {
        //            paramList();
        //            expect(TkRPAREN);
        //        }
        //        else if (accept(TkLBRACKET)) {
        //            expression();
        //            expect(TkRBRACKET);
        //            expect(TkEQUALS);
        //            expression();
        //        }
        //        else {
        //            error(3, TkEQUALS, TkLPAREN, TkLBRACKET);
        //        }
    }
    else {
        return NULL;
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
Basic::Block* Parser::block(void) {
    Basic::Block *block = new Basic::Block();
    do {
        Basic::Statement *s = statement();
        if (s)  block->appendStatement(s);
    } while (accept(TkCOLON) || accept(TkEOL));
    return block;
}
        
// <do-statement-body> ::= ( "while" | "until" ) <expression> <block> "loop"
//                       | <block> "loop" ( "while" | "until" ) <expression>
//                       | <block> "done"
void Parser::doStatementBody(void) {
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

// <subscript> ::= "(" <expression> [ "," <expression> ]... ")"
Basic::ArraySubscript* Parser::arraySubscript(void) {
//    ArraySubscript subscript = new ArraySubscript();
    if (expect(TkLPAREN)) {
        // FIXME...
    }
    
    return NULL;
}

// <param-list> ::= <expression> [ "," <expression> ]...
Basic::ParamList* Parser::paramList(void) {
    Basic::ParamList *p = new Basic::ParamList();
    do {
        p->appendExpression(expression());
    } while (accept(TkCOMMA));
    return p;
}

// <print-statement-body> ::= <print-expression-list>
// <print-expression-list> ::= <expression> [ "," <print-expression-list> ]...
//                           | <null>
Basic::PrintStatement* Parser::printStatementBody(void) {
    Basic::PrintStatement *s = new Basic::PrintStatement();
    int comma = 0;
    int expressions = 0;
    do {
        if (this->token == TkIDENTIFIER or this->token == TkLPAREN or this->token == TkLITERAL) {
            s->appendExpression(expression());
            comma = 0;
            expressions++;
        }
        else {
            break;
        }
    } while ((comma = accept(TkCOMMA)));
    
    if (expressions == 0 or comma == 1)  s->setAppendEol(true);
    return s;
}

// FIXME <input-statement-body> ::= <identifier> [ "[" <expression> [ "," <expression> ] "]" ] [ "," <identifier> [ "["  <expression> [ "," <expression> ] "]" ] ]...
// <input-statement-body> ::= <identifier> [ "," <identifier> ]...
Basic::InputStatement* Parser::inputStatementBody(void) {
    if (expect(TkIDENTIFIER)) {
        Basic::InputStatement *s = new Basic::InputStatement();
        s->appendIdentifier(this->accepted_token_value.getStringValue());
        
        while (accept(TkCOMMA)) {
            if (expect(TkIDENTIFIER)) {
                s->appendIdentifier(this->accepted_token_value.getStringValue());
            }
            else {
                delete s;
                return NULL;
            }
        }
        
        return s;
    }
    else {
        return NULL;
    }
}

// <let-statement-body> ::= <identifier> [ <array-subscript> ] "=" <expression>
Basic::LetStatement* Parser::letStatementBody(void) {
    ArraySubscript *s = NULL;
    Expression *e = NULL;

    if (expect(TkIDENTIFIER)) {
        const char *identifier = this->accepted_token_value.getStringValue();
        if (this->token == TkLPAREN) {
            s = arraySubscript();
        }
        if (expect(TkEQUALS)) {
            if ((e = expression())) {
                return new LetStatement(identifier, s, e);
            }
        }
    }
    
    if (s)  delete s;
    if (e)  delete e;
    return NULL;
}

// <call-statement-body> ::= <identifier> "(" <param-list> ")"
Basic::CallStatement* Parser::callStatementBody(void) {
    ParamList *p = NULL;

    if (expect(TkIDENTIFIER)) {
        const char *identifier = this->accepted_token_value.getStringValue();
        if (expect(TkLPAREN)) {
            if((p = paramList())) {
                if (expect(TkRPAREN)) {
                    return new CallStatement(identifier, paramList());                    
                }
            }
        }
        
    }
    
    if (p)  delete p;
    return NULL;
}

// <if-statement-body> ::= <expression> "then" <block> [ "elseif" <expression> "then" <block> ]... [ "else" <block> ] "end" "if"
Basic::IfStatement* Parser::ifStatementBody(void) {
    IfStatement *s = NULL;
    Expression *e = NULL;
    
    if ((e = expression())) {
        s = new IfStatement();
        if (expect(TkTHEN)) {
            s->appendCondition(e, block());
        }
        while (accept(TkELSEIF)) {
            if ((e = expression())) {
                if (expect(TkTHEN)) {
                    s->appendCondition(e, block());
                }
            }
        }
        if (accept(TkELSE)) {
            s->setElseBlock(block());
        }
        if (expect(TkEND) and expect(TkIF)) {
            return s;
        }
    }
    
    if (s)  delete s;
    return NULL;
}


// <primary-expression> ::= <identifier> "(" <param-list> ")"
//                        | <identifier> "[" <expression> [ "," <expression> ] "]"
//                        | <identifier>  
//                        | "(" <expression> ")"
//                        | <literal>
Basic::Expression* Parser::primaryExpression(void) {
    if(accept(TkIDENTIFIER)) {
        const char *identifier = this->accepted_token_value.getStringValue();
        
        if(accept(TkLPAREN)) {
            Basic::ParamList *params = paramList();
            expect(TkRPAREN);
            return new Basic::FunctionCallExpression(identifier, params);
        }
        else if (accept(TkLBRACKET)) {
            Basic::Expression *first = expression();
            Basic::Expression *second = NULL;
            if (accept(TkCOMMA)) {
                second = expression();
            }
            expect(TkRBRACKET);
            return new Basic::ArrayIndexExpression(identifier, first, second);
        }
        else {
            return new Basic::VariableExpression(identifier);
        }
    }
    else if (accept(TkLPAREN)) {
        Basic::Expression *e = expression();
        expect(TkRPAREN);
        return e;
    }
    else if (accept(TkLITERAL)) {
        return new Basic::LiteralExpression(this->accepted_token_value);
    }
    else {
        error(3, TkIDENTIFIER, TkLPAREN, TkLITERAL);
        return NULL;
    }
}

// <unary-expression> ::= <unary-operator> <primary-expression> | <primary-expression>
// <unary-operator> ::= "not" | "-"
Basic::Expression* Parser::unaryExpression(void) {
    if (accept(TkNOT)) {
        return new Basic::UnaryExpression(TkNOT, primaryExpression());
    }
    else if (accept(TkMINUS)) {
        return new Basic::UnaryExpression(TkMINUS, primaryExpression());
    }
    else {
        return primaryExpression();
    }
}

// <multiplicative-expression> ::= <unary-expression> [ <multiplicative-operator> <unary-expression> ]...
// <multiplicative-operator> ::= "*" | "/" | "mod"
Basic::Expression* Parser::multiplicativeExpression(void) {
    Basic::Expression *first = unaryExpression();

    if (this->token == TkMULTIPLY or this->token == TkDIVIDE or this->token == TkMOD) {
        Basic::MultiplicativeExpression *e = new Basic::MultiplicativeExpression();
        e->appendTerm(first);
        e->appendOperator(this->token);
        while (accept(TkMULTIPLY) or accept(TkDIVIDE) or accept(TkMOD)) {
            e->appendTerm(unaryExpression());
            if (this->token == TkMULTIPLY or this->token == TkDIVIDE or this->token == TkMOD) {
                e->appendOperator(this->token);
            }
        }
        return e;
    }
    else {
        return first;
    }
}

// <additive-expression> ::= <multiplicative-expression> [ <additive-operator> <multiplicative-expression> ]...
// <additive-operator> ::= "+" | "-"
Basic::Expression* Parser::additiveExpression(void) {
    Basic::Expression *first = multiplicativeExpression();
    
    if (this->token == TkPLUS or this->token == TkMINUS) {
        Basic::AdditiveExpression *e = new Basic::AdditiveExpression();
        e->appendTerm(first);
        e->appendOperator(this->token);
        while (accept(TkPLUS) or accept(TkMINUS)) {
            e->appendTerm(multiplicativeExpression());
            if (this->token == TkPLUS or this->token == TkMINUS) {
                e->appendOperator(this->token);
            }
        }
        return e;
    }
    else {
        return first;
    }
}

// <comparitive-expression> ::= <additive-expression> [ <comparitive-operator> <additive-expression> ]
// <comparitive-operator> ::= "=" | "<>" | "<" | ">" | "<=" | ">="
Basic::Expression* Parser::comparitiveExpression(void) {
    Basic::Expression *first = additiveExpression();
    Token t = this->token;
    if (accept(TkEQUALS) || accept(TkNOTEQUALS) || accept(TkLT) || accept(TkGT) || accept(TkLTEQUALS) || accept(TkGTEQUALS)) {
        Basic::Expression *second = additiveExpression();
        return new Basic::ComparitiveExpression(first, t, second);
    }
    else {
        return first;
    }
}

// <and-expression> ::= <comparitive-expression> [ "and" <comparitive-expression> ]...
Basic::Expression* Parser::andExpression(void) {
    Basic::Expression *first = comparitiveExpression();
    
    if (this->token == TkAND) {
        Basic::AndExpression *e = new Basic::AndExpression();
        e->appendTerm(first);
        while (accept(TkAND)) {
            e->appendTerm(andExpression());
        }
        return e;
    }
    else {
        return first;
    }
}

// <or-expression> ::= <and-expression> [ "or" <and-expression> ]...
Basic::Expression* Parser::orExpression(void) {
    Basic::Expression *first = andExpression();
    
    if (this->token == TkOR) {
        Basic::OrExpression *e = new Basic::OrExpression();
        e->appendTerm(first);
        while (accept(TkOR)) {
            e->appendTerm(andExpression());
        }
        return e;
    }
    else {
        return first;
    }
}

// <expression> ::= <or-expression>
Basic::Expression* Parser::expression(void) {
    return orExpression();
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
