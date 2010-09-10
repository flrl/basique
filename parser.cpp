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

int Parser::accept(Basic::Token t) {
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

int Parser::expect(Basic::Token t) {
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
        if (token >= Basic::Tk_MIN && token < Basic::Tk_MAX) {
            fprintf(stderr, " `%s',", Basic::Tokeniser::tokenDescriptions[va_arg(argv, int)]);
        }
    }
    va_end(argv);
    fprintf(stderr, " got `%s'\n", Basic::Tokeniser::tokenDescriptions[this->token]);
}


// <unit> ::= "function" <function-definition-body> <eol>
//          | "sub" <sub-definition-body> <eol>
//          | <statement> <eol>
Basic::ASTNode* Parser::unit(void) {
    Basic::ASTNode *unit = NULL;

    if (accept(Basic::TkFUNCTION)) {
        unit = functionDefinitionBody();
    }
    else if (accept(Basic::TkSUB)) {
        unit = subDefinitionBody();
    }
    else {
        unit = statement();
    }

    if (unit) {
        if (expect(Basic::TkEOL)) {
            return unit;
        }
        else {
            delete unit;
            return NULL;
        }
    }
    else {
        return NULL;
    }
}

// <function-definition-body> ::= <identifier> "(" <accepted-param-list> ")" <block> "end" "function"
Basic::FunctionDefinition* Parser::functionDefinitionBody(void) {
    Basic::AcceptedParamList *a = NULL;
    Basic::Block *b = NULL;
    if (expect(Basic::TkIDENTIFIER)) {
        String identifier(accepted_token_value.getStringValue());
        if (expect(Basic::TkLPAREN)) {
            if (this->token != Basic::TkRPAREN) {
                if (not (a = acceptedParamList())) {
                    return NULL;
                }
            }

            if (expect(Basic::TkRPAREN)) {
                if ((b = block())) {
                    if (expect(Basic::TkEND) and expect(Basic::TkFUNCTION)) {
                        return new Basic::FunctionDefinition(identifier, a, b);
                    }
                }
            }
        }
    }
                        
    if (a)  delete a;
    if (b)  delete b;
    return NULL;
}

// <sub-definition-body> ::= <identifier> "(" <accepted-param-list> ")" <block> "end" "sub"
Basic::SubDefinition* Parser::subDefinitionBody(void) {
    Basic::AcceptedParamList *a = NULL;
    Basic::Block *b = NULL;
    if (expect(Basic::TkIDENTIFIER)) {
        String identifier(accepted_token_value.getStringValue());
        if (expect(Basic::TkLPAREN)) {
            if (this->token != Basic::TkRPAREN) {
                if (not (a = acceptedParamList())) {
                    return NULL;
                }
            }
            
            if (expect(Basic::TkRPAREN)) {
                if ((b = block())) {
                    if (expect(Basic::TkEND) and expect(Basic::TkSUB)) {
                        return new Basic::SubDefinition(identifier, a, b);
                    }
                }
            }
        }
    }
    
    if (a)  delete a;
    if (b)  delete b;
    return NULL;
}

// <statement> ::= "print" <print-statement-body>
//               | "input" <input-statement-body>
//               | "let" <let-statement-body>
//               | "call" <call-statement-body>
//               | "if" <if-statement-body>
//               | "do" <do-statement-body>
//               | "for" <for-statement-body>
//               | "dim" <dim-statement-body>
//               | "exit" [ <expression> ] // FIXME
//               | <let-statement-body>
//               | <call-statement-body>
Basic::Statement* Parser::statement(void) {
    if (accept(Basic::TkPRINT)) {
        return printStatementBody();
    }
    else if (accept(Basic::TkINPUT)) {
        return inputStatementBody();
    }
    else if (accept(Basic::TkLET)) {
        return letStatementBody();
    }
    else if (accept(Basic::TkCALL)) {
        return callStatementBody();
    }
    else if (accept(Basic::TkIF)) {
        return ifStatementBody();
    }
    else if (accept(Basic::TkDO)) {
        return doStatementBody();
    }
    else if (accept(Basic::TkFOR)) {
        return forStatementBody();
    }
    else if (accept(Basic::TkDIM)) {
        return dimStatementBody();
    }
//    else if (accept(Basic::TkEXIT)) {
//        expression();
//    }
//    else if (accept(Basic::TkIDENTIFIER)) {
//        if (accept(Basic::TkEQUALS)) {
//            expression();
//        }
//        else if (accept(Basic::TkLPAREN)) {
//            paramList();
//            expect(Basic::TkRPAREN);
//        }
//        else if (accept(Basic::TkLBRACKET)) {
//            expression();
//            expect(Basic::TkRBRACKET);
//            expect(Basic::TkEQUALS);
//            expression();
//        }
//        else {
//            error(3, Basic::TkEQUALS, Basic::TkLPAREN, Basic::TkLBRACKET);
//        }
//    }
    else {
        error(10, Basic::TkPRINT, Basic::TkINPUT, Basic::TkLET, Basic::TkCALL, Basic::TkIF, Basic::TkDO, Basic::TkFOR, Basic::TkDIM, Basic::TkEXIT, Basic::TkIDENTIFIER);
        return NULL;
    }
}

// <block> ::= <block-statement-list>
// <block-statement-list> ::= <statement> [ ( ":" | <eol> ) <block-statement-list> ]...
//                          | <null>
Basic::Block* Parser::block(void) {
    Basic::Block *block = new Basic::Block();
    Basic::Statement *s = NULL;
    do {
        switch (this->token) {
            case Basic::TkPRINT:
            case Basic::TkINPUT:
            case Basic::TkLET:
            case Basic::TkCALL:
            case Basic::TkIF:
            case Basic::TkDO:
            case Basic::TkFOR:
            case Basic::TkDIM:
            case Basic::TkEXIT:
            case Basic::TkIDENTIFIER:
                if ((s = statement())) {
                    block->appendStatement(s);
                }
                else {
                    delete block;
                    return NULL;
                }
                break;
                
            default:
                ;
        }
    } while (accept(Basic::TkCOLON) or accept(Basic::TkEOL));
    return block;    
}
        
// <accepted-param-list> ::= <identifier> [ "," <identifier> ]...
//                         | <null> 
Basic::AcceptedParamList* Parser::acceptedParamList(void) {
    Basic::AcceptedParamList *a = new Basic::AcceptedParamList();
    if (accept(Basic::TkIDENTIFIER)) {
        a->appendIdentifier(accepted_token_value.getStringValue());
        while(accept(Basic::TkCOMMA)) {
            if (expect(Basic::TkIDENTIFIER)) {
                a->appendIdentifier(accepted_token_value.getStringValue());
            }
            else {
                delete a;
                return NULL;
            }
        }
    }
    return a;
}

// <array-subscript> ::= "(" <expression> [ "," <expression> ]... ")"
Basic::ArraySubscript* Parser::arraySubscript(void) {
    Basic::Expression *e = NULL;
    if (expect(Basic::TkLPAREN)) {
        if ((e = expression())) {
            Basic::ArraySubscript *sub = new Basic::ArraySubscript(e);
            while (accept(Basic::TkCOMMA)) {
                if ((e = expression())) {
                    sub->appendExpression(e);
                }
                else {
                    delete sub;
                    return NULL;
                }
            }
            if (expect(Basic::TkRPAREN)) {
                return sub;                
            }
            else {
                delete sub;
                return NULL;
            }
        }
        else {
            return NULL;
        }
    }
    else {
        return NULL;
    }
}

// <array-dimension> ::= "(" <expression> [ "to" <expression> ] [ "," <expression> [ "to" <expression> ] ]... ")"
Basic::ArrayDimension* Parser::arrayDimension(void) {
    Basic::Expression *a = NULL;
    Basic::Expression *b = NULL;
    if (expect(Basic::TkLPAREN)) {
        if ((a = expression())) {
            if (accept(Basic::TkTO)) {
                if (not (b = expression())) {
                    delete a;
                    return NULL;
                }
            }
            Basic::ArrayDimension *dim = new Basic::ArrayDimension(a, b);
            
            while (accept(Basic::TkCOMMA)) {
                if ((a = expression())) {
                    if (accept(Basic::TkTO)) {
                        if ((b = expression())) {
                            dim->appendDimension(a, b);
                        }
                        else {
                            delete a;
                            delete dim;
                            return NULL;
                        }
                    }
                    else {
                        dim->appendDimension(a, NULL);
                    }
                }
            }
            
            return dim;
        }
    }
    
    return NULL;
}

// <param-list> ::= <expression> [ "," <expression> ]... 
//                | <null>
Basic::ParamList* Parser::paramList(void) {
    Basic::ParamList *p = new Basic::ParamList();
    Basic::Expression *e = expression();
    if (e) {
        p->appendExpression(e);
        while (accept(Basic::TkCOMMA)) {
            if ((e = expression())) {
                p->appendExpression(e);
            }
            else {
                delete p;
                return NULL;
            }
        }
    }
    
    return p;
}

// <print-statement-body> ::= <print-expression-list>
// <print-expression-list> ::= <expression> [ "," <expression> ]... [ "," ]
//                           | <null>
Basic::PrintStatement* Parser::printStatementBody(void) {
    Basic::PrintStatement *s = new Basic::PrintStatement();

    if (this->token == Basic::TkLITERAL or this->token == Basic::TkIDENTIFIER or this->token == Basic::TkLPAREN) {
        Basic::Expression *e = NULL;
        if ((e = expression())) {
            s->appendExpression(e);
            while (accept(Basic::TkCOMMA)) {
                if (this->token == Basic::TkLITERAL or this->token == Basic::TkIDENTIFIER or this->token == Basic::TkLPAREN) {
                    if ((e = expression())) {
                        s->appendExpression(e);
                    }
                    else {
                        delete s;
                        return NULL;
                    }
                }
                else {
                    // trailing comma
                    s->setAppendEol(false);
                    break;
                }
            }
        }
        else {
            delete s;
            return NULL;
        }
    }

    return s;
}

// <input-statement-body> ::= <identifier> [ <array-subscript> ] [ "," <expression> ]
Basic::InputStatement* Parser::inputStatementBody(void) {
    if (expect(Basic::TkIDENTIFIER)) {
        String identifier(this->accepted_token_value.getStringValue());
        Basic::ArraySubscript *subscript = NULL;
        Basic::Expression *prompt = NULL;
        if (this->token == Basic::TkLPAREN) {
            if (not (subscript = arraySubscript())) {
                return NULL;
            }
        }
        if (accept(Basic::TkCOMMA)) {
            if (not (prompt = expression())) {
                if (subscript)  delete subscript;
                return NULL;
            }
        }
        return new Basic::InputStatement(identifier, subscript, prompt);
    }
    else {
        return NULL;
    }
}

// <let-statement-body> ::= <identifier> [ <array-subscript> ] "=" <expression>
Basic::LetStatement* Parser::letStatementBody(void) {
    Basic::ArraySubscript *s = NULL;
    Basic::Expression *e = NULL;

    if (expect(Basic::TkIDENTIFIER)) {
        String identifier(this->accepted_token_value.getStringValue());
        if (this->token == Basic::TkLPAREN and not (s = arraySubscript())) {
            return NULL;
        }
        if (expect(Basic::TkEQUALS)) {
            if ((e = expression())) {
                return new Basic::LetStatement(identifier, s, e);
            }
        }
    }
    
    if (s)  delete s;
    if (e)  delete e;
    return NULL;
}

// <call-statement-body> ::= <identifier> "(" <param-list> ")"
Basic::CallStatement* Parser::callStatementBody(void) {
    Basic::ParamList *p = NULL;

    if (expect(Basic::TkIDENTIFIER)) {
        String identifier(this->accepted_token_value.getStringValue());
        if (expect(Basic::TkLPAREN)) {
            if((p = paramList())) {
                if (expect(Basic::TkRPAREN)) {
                    return new Basic::CallStatement(identifier, p);     
                }
            }
        }
        
    }
    
    if (p)  delete p;
    return NULL;
}

// <if-statement-body> ::= <expression> "then" <block> [ "elseif" <expression> "then" <block> ]... [ "else" <block> ] "end" "if"
Basic::IfStatement* Parser::ifStatementBody(void) {
    Basic::IfStatement *s = NULL;
    Basic::Expression *e = NULL;
    Basic::Block *b = NULL;
    
    if ((e = expression())) {
        s = new Basic::IfStatement();
        if (expect(Basic::TkTHEN) and (b = block())) {
            s->appendCondition(e, b);
            
            while (accept(Basic::TkELSEIF)) {
                if ((e = expression())) {
                    if (expect(Basic::TkTHEN) and (b = block())) {
                        s->appendCondition(e, b);
                    }
                }
            }
            if (accept(Basic::TkELSE) and (b=block())) {
                s->setElseBlock(b);
            }
            if (expect(Basic::TkEND) and expect(Basic::TkIF)) {
                return s;
            }
        }
    }
    
    if (s)  delete s;
    return NULL;
}

// <do-statement-body> ::= ( "while" | "until" ) <expression> <block> "loop"
//                       | <block> "loop" ( "while" | "until" ) <expression>
//                       | <block> "loop"
//                       | <block> "done"
Basic::DoStatement* Parser::doStatementBody(void) {
    Basic::DoStatement::Type t;
    Basic::DoStatement::When w;
    Basic::Expression *c = NULL;
    Basic::Block *b = NULL;
    
    if (accept(Basic::TkWHILE)) {
        t = Basic::DoStatement::WHILE;
        w = Basic::DoStatement::PRECONDITION;
        if ((c = expression())) {
            if ((b = block())) {
                if (expect(Basic::TkLOOP)) {
                    return new Basic::DoStatement(t, w, c, b);
                }
            }
        }
    }
    else if (accept(Basic::TkUNTIL)) {
        t = Basic::DoStatement::UNTIL;
        w = Basic::DoStatement::PRECONDITION;
        if ((c = expression())) {
            if ((b = block())) {
                if (expect(Basic::TkLOOP)) {
                    return new Basic::DoStatement(t, w, c, b);
                }
            }
        }
    }
    else if ((b = block())) {
        if (accept(Basic::TkDONE)) {
            t = Basic::DoStatement::ONCE;
            w = Basic::DoStatement::POSTCONDITION;
            return new Basic::DoStatement(t, w, NULL, b);
        }
        else if (accept(Basic::TkLOOP)) {
            w = Basic::DoStatement::POSTCONDITION;
            if (accept(Basic::TkWHILE) and ((c = expression()))) {
                return new Basic::DoStatement(Basic::DoStatement::WHILE, w, c, b);
            }
            else if (accept(Basic::TkUNTIL) and ((c = expression()))) {
                return new Basic::DoStatement(Basic::DoStatement::UNTIL, w, c, b);
            }
            else {
                return new Basic::DoStatement(Basic::DoStatement::FOREVER, w, c, b);
            }
        }
        else {
            error(2, Basic::TkLOOP, Basic::TkDONE);
        }
    }

    if (c)  delete c;
    if (b)  delete b;
    return NULL;
}

// <for-statement-body> ::= <identifier> "=" <expression> "to" <expression> [ "step" <expression> ] <block> "next" [ <identifier> ]
Basic::ForStatement* Parser::forStatementBody(void) {
    Basic::Expression *start = NULL;
    Basic::Expression *end = NULL;
    Basic::Expression *step = NULL;
    Basic::Block *body = NULL;
    if (expect(Basic::TkIDENTIFIER)) {
        String identifier(this->accepted_token_value.getStringValue());
        if (expect(Basic::TkEQUALS)) {
            if ((start = expression())) {
                if (expect(Basic::TkTO)) {
                    if ((end = expression())) {
                        if (accept(Basic::TkSTEP)) {
                            if (not (step = expression())) {
                                delete end;
                                delete start;
                                return NULL;
                            }
                        }
                        if ((body = block())) {
                            if (expect(Basic::TkNEXT)) {
                                accept(Basic::TkIDENTIFIER);  // FIXME just ignoring it for now
                                return new Basic::ForStatement(identifier, start, end, step, body);
                            }
                        }
                    }
                }
            }
        }
    }

    if (body)  delete body;
    if (step)  delete step;
    if (end)   delete end;
    if (start) delete start;
    return NULL;
}

// <dim-statement-body> ::= <identifier> [ <array-dimension> ] [ "," <identifier> [ <array-dimension> ] ]...
Basic::DimStatement* Parser::dimStatementBody(void) {
    Basic::ArrayDimension *dim = NULL;
    if (expect(Basic::TkIDENTIFIER)) {
        String identifier(this->accepted_token_value.getStringValue());
        dim = arrayDimension();
        Basic::DimStatement *s = new Basic::DimStatement(identifier, dim);
        while (accept(Basic::TkCOMMA)) {
            if (expect(Basic::TkIDENTIFIER)) {
                String identifier(this->accepted_token_value.getStringValue());
                if (this->token == Basic::TkLPAREN) {
                    if ((dim = arrayDimension())) {
                        s->appendDimensionable(identifier, dim);
                    }
                    else {
                        delete s;
                        return NULL;
                    }
                }
                else {
                    s->appendDimensionable(identifier, NULL);                    
                }
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

// <primary-expression> ::= <literal>
//                        | <identifier> [ "(" <param-list> ")" ]
//                        | "(" <expression> ")"
Basic::Expression* Parser::primaryExpression(void) {
    if (accept(Basic::TkLITERAL)) {
        return new Basic::LiteralExpression(this->accepted_token_value);
    }
    else if (accept(Basic::TkIDENTIFIER)) {
        String identifier(this->accepted_token_value.getStringValue());
        if (accept(Basic::TkLPAREN)) {
            Basic::ParamList *p = NULL;
            if ((p = paramList())) {
                Basic::ParamList *p = paramList();
                if (expect(Basic::TkRPAREN)) {
                    return new Basic::IdentifierExpression(identifier, p);
                }
                else {
                    delete p;
                    return NULL;
                }                
            }
            else {
                return NULL;
            }
        }
        else {
            return new Basic::IdentifierExpression(identifier);
        }
    }
    else if (accept(Basic::TkLPAREN)) {
        Basic::Expression *e = NULL;
        if ((e = expression())) {
            if (expect(Basic::TkRPAREN)) {
                return e;
            }
            else {
                delete e;
                return NULL;
            }
        }
        else {
            return NULL;
        }
    }
    else {
        error(3, Basic::TkLITERAL, Basic::TkIDENTIFIER, Basic::TkLPAREN);
        return NULL;
    }
}

// <unary-expression> ::= <unary-operator> <primary-expression> | <primary-expression>
// <unary-operator> ::= "not" | "-"
Basic::Expression* Parser::unaryExpression(void) {
    Basic::Expression *e = NULL;
    if (accept(Basic::TkNOT)) {
        if ((e = primaryExpression())) {
            return new Basic::UnaryExpression(Basic::TkNOT, e);            
        }
    }
    else if (accept(Basic::TkMINUS)) {
        if ((e = primaryExpression())) {
            return new Basic::UnaryExpression(Basic::TkMINUS, e);            
        }
    }
    else {
        return primaryExpression();
    }
    
    if (e)  delete e;
    return NULL;
}

// <multiplicative-expression> ::= <unary-expression> [ <multiplicative-operator> <unary-expression> ]...
// <multiplicative-operator> ::= "*" | "/" | "mod"
Basic::Expression* Parser::multiplicativeExpression(void) {
    Basic::Expression *term = NULL;
    
    if ((term = unaryExpression())) {
        if (this->token == Basic::TkMULTIPLY or this->token == Basic::TkDIVIDE or this->token == Basic::TkMOD) {
            Basic::MultiplicativeExpression *e = new Basic::MultiplicativeExpression();
            e->appendTerm(term);
            e->appendOperator(this->token);
            while (accept(Basic::TkMULTIPLY) or accept(Basic::TkDIVIDE) or accept(Basic::TkMOD)) {
                if ((term = unaryExpression())) {
                    e->appendTerm(term);
                    if (this->token == Basic::TkMULTIPLY or this->token == Basic::TkDIVIDE or this->token == Basic::TkMOD) {
                        e->appendOperator(this->token);
                    }                    
                }
                else {
                    delete e;
                    return NULL;
                }
            }
            return e;
        }
        else {
            return term;
        }        
    }
    else {
        return NULL;
    }
}

// <additive-expression> ::= <multiplicative-expression> [ <additive-operator> <multiplicative-expression> ]...
// <additive-operator> ::= "+" | "-"
Basic::Expression* Parser::additiveExpression(void) {
    Basic::Expression *term = NULL;
    
    if ((term = multiplicativeExpression())) {
        if (this->token == Basic::TkPLUS or this->token == Basic::TkMINUS) {
            Basic::AdditiveExpression *e = new Basic::AdditiveExpression();
            e->appendTerm(term);
            e->appendOperator(this->token);
            while (accept(Basic::TkPLUS) or accept(Basic::TkMINUS)) {
                if ((term = multiplicativeExpression())) {
                    e->appendTerm(multiplicativeExpression());
                    if (this->token == Basic::TkPLUS or this->token == Basic::TkMINUS) {
                        e->appendOperator(this->token);
                    }                    
                }
                else {
                    delete e;
                    return NULL;
                }
            }
            return e;
        }
        else {
            return term;
        }
    }
    else {
        return NULL;
    }
}

// <comparitive-expression> ::= <additive-expression> [ <comparitive-operator> <additive-expression> ]
// <comparitive-operator> ::= "=" | "<>" | "<" | ">" | "<=" | ">="
Basic::Expression* Parser::comparitiveExpression(void) {
    Basic::Expression *first = NULL;
    Basic::Expression *second = NULL;
    
    if ((first = additiveExpression())) {
        Basic::Token t = this->token;
        if (accept(Basic::TkEQUALS) || accept(Basic::TkNOTEQUALS) || accept(Basic::TkLT) || accept(Basic::TkGT) || accept(Basic::TkLTEQUALS) || accept(Basic::TkGTEQUALS)) {
            if ((second = additiveExpression())) {
                return new Basic::ComparitiveExpression(first, t, second);                
            }
            else {
                delete first;
                return NULL;
            }
        }
        else {
            return first;
        }        
    }
    else {
        return NULL;
    }
}

// <and-expression> ::= <comparitive-expression> [ "and" <comparitive-expression> ]...
Basic::Expression* Parser::andExpression(void) {
    Basic::Expression *term = NULL;
    
    if ((term = comparitiveExpression())) {
        if (this->token == Basic::TkAND) {
            Basic::AndExpression *e = new Basic::AndExpression();
            e->appendTerm(term);
            while (accept(Basic::TkAND)) {
                if ((term = comparitiveExpression())) {
                    e->appendTerm(term);                    
                }
                else {
                    delete e;
                    return NULL;
                }
            }
            return e;
        }
        else {
            return term;
        }        
    }
    else {
        return NULL;
    }
}

// <or-expression> ::= <and-expression> [ "or" <and-expression> ]...
Basic::Expression* Parser::orExpression(void) {
    Basic::Expression *term = NULL;
    
    if ((term = andExpression())) {
        if (this->token == Basic::TkOR) {
            Basic::OrExpression *e = new Basic::OrExpression();
            e->appendTerm(term);
            while (accept(Basic::TkOR)) {
                if ((term = andExpression())) {
                    e->appendTerm(term);
                }
                else {
                    delete e;
                    return NULL;
                }
            }
            return e;
        }
        else {
            return term;
        }        
    }
    else {
        return NULL;
    }
}

// <expression> ::= <or-expression>
Basic::Expression* Parser::expression(void) {
    return orExpression();
}

// <type> ::= "integer" | "real" | "string"
void Parser::type(void) {
    if (accept(Basic::TkINTEGER)) {
        ;
    }
    else if (accept(Basic::TkREAL)) {
        ;
    }
    else if (accept(Basic::TkSTRING)) {
        ;
    }
    else {
        error(3, Basic::TkINTEGER, Basic::TkREAL, Basic::TkSTRING);
    }    
}
