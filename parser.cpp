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


// <unit> ::= "function" <function-definition-body>
//          | "sub" <sub-definition-body>
//          | <statement>
Basic::ASTNode* Parser::unit(void) {
    if (accept(TkFUNCTION)) {
        return functionDefinitionBody();
    }
    else if (accept(TkSUB)) {
        return subDefinitionBody();
    }
    else {
        return statement();
    }
}

// <function-definition-body> ::= <identifier> "(" <accepted-param-list> ")" <block> "end" "function"
Basic::FunctionDefinition* Parser::functionDefinitionBody(void) {
    AcceptedParamList *a = NULL;
    Block *b = NULL;
    if (expect(TkIDENTIFIER)) {
        String identifier(accepted_token_value.getStringValue());
        if (expect(TkLPAREN)) {
            if (this->token != TkRPAREN) {
                if (not (a = acceptedParamList())) {
                    return NULL;
                }
            }

            if (expect(TkRPAREN)) {
                if ((b = block())) {
                    if (expect(TkEND) and expect(TkFUNCTION)) {
                        return new FunctionDefinition(identifier, a, b);
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
    AcceptedParamList *a = NULL;
    Block *b = NULL;
    if (expect(TkIDENTIFIER)) {
        String identifier(accepted_token_value.getStringValue());
        if (expect(TkLPAREN)) {
            if (this->token != TkRPAREN) {
                if (not (a = acceptedParamList())) {
                    return NULL;
                }
            }
            
            if (expect(TkRPAREN)) {
                if ((b = block())) {
                    if (expect(TkEND) and expect(TkSUB)) {
                        return new SubDefinition(identifier, a, b);
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
        return doStatementBody();
    }
    else if (accept(TkFOR)) {
        return forStatementBody();
    }
    else if (accept(TkDIM)) {
        return dimStatementBody();
    }
//    else if (accept(TkEXIT)) {
//        expression();
//    }
//    else if (accept(TkIDENTIFIER)) {
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
//    }
    else {
        error(10, TkPRINT, TkINPUT, TkLET, TkCALL, TkIF, TkDO, TkFOR, TkDIM, TkEXIT, TkIDENTIFIER);
        return NULL;
    }
}

// <block> ::= <block-statement-list>
// <block-statement-list> ::= <statement> [ ( ":" | <eol> ) <block-statement-list> ]...
//                          | <null>
Basic::Block* Parser::block(void) {
    Block *block = new Basic::Block();
    Statement *s = NULL;
    do {
        switch (this->token) {
            case TkPRINT:
            case TkINPUT:
            case TkLET:
            case TkCALL:
            case TkIF:
            case TkDO:
            case TkFOR:
            case TkDIM:
            case TkEXIT:
            case TkIDENTIFIER:
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
    } while (accept(TkCOLON) or accept(TkEOL));
    return block;    
}
        
// <accepted-param-list> ::= <identifier> [ "," <identifier> ]...
//                         | <null> 
Basic::AcceptedParamList* Parser::acceptedParamList(void) {
    AcceptedParamList *a = new Basic::AcceptedParamList();
    if (accept(TkIDENTIFIER)) {
        a->appendIdentifier(accepted_token_value.getStringValue());
        while(accept(TkCOMMA)) {
            if (expect(TkIDENTIFIER)) {
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
    Expression *e = NULL;
    if (expect(TkLPAREN)) {
        if ((e = expression())) {
            ArraySubscript *sub = new ArraySubscript(e);
            while (accept(TkCOMMA)) {
                if ((e = expression())) {
                    sub->appendExpression(e);
                }
                else {
                    delete sub;
                    return NULL;
                }
            }
            if (expect(TkRPAREN)) {
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
    Expression *a = NULL;
    Expression *b = NULL;
    if (expect(TkLPAREN)) {
        if ((a = expression())) {
            if (accept(TkTO)) {
                if (not (b = expression())) {
                    delete a;
                    return NULL;
                }
            }
            ArrayDimension *dim = new ArrayDimension(a, b);
            
            while (accept(TkCOMMA)) {
                if ((a = expression())) {
                    if (accept(TkTO)) {
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
    Expression *e = expression();
    if (e) {
        p->appendExpression(e);
        while (accept(TkCOMMA)) {
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
// <print-expression-list> ::= <expression> [ "," <print-expression-list> ]...
//                           | <null>
Basic::PrintStatement* Parser::printStatementBody(void) {
    Basic::PrintStatement *s = new Basic::PrintStatement();
    Expression *e = NULL;
    int comma = 0;
    int expressions = 0;
    do {
        switch (this->token) {
            case TkLITERAL:
            case TkIDENTIFIER:
            case TkLPAREN:
                if ((e = expression())) {
                    s->appendExpression(e);
                    comma = 0;
                    expressions++;
                }
                else {
                    delete s;
                    return NULL;
                }
                break;
            default:
                ;
        }
    } while ((comma = accept(TkCOMMA)));
    
    if (expressions == 0 or comma == 1)  s->setAppendEol(true);
    return s;
}

// <input-statement-body> ::= <identifier> [ <array-subscript> ] [ "," <expression> ]
Basic::InputStatement* Parser::inputStatementBody(void) {
    if (expect(TkIDENTIFIER)) {
        const char *identifier = this->accepted_token_value.getStringValue();
        ArraySubscript *subscript = NULL;
        Expression *prompt = NULL;
        if (this->token == TkLPAREN) {
            if (not (subscript = arraySubscript())) {
                return NULL;
            }
        }
        if (accept(TkCOMMA)) {
            if (not (prompt = expression())) {
                if (subscript)  delete subscript;
                return NULL;
            }
        }
        return new InputStatement(identifier, subscript, prompt);
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
        if (this->token == TkLPAREN and not (s = arraySubscript())) {
            return NULL;
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
                    return new CallStatement(identifier, p);     
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
    Block *b = NULL;
    
    if ((e = expression())) {
        s = new IfStatement();
        if (expect(TkTHEN) and (b = block())) {
            s->appendCondition(e, b);
            
            while (accept(TkELSEIF)) {
                if ((e = expression())) {
                    if (expect(TkTHEN) and (b = block())) {
                        s->appendCondition(e, b);
                    }
                }
            }
            if (accept(TkELSE) and (b=block())) {
                s->setElseBlock(b);
            }
            if (expect(TkEND) and expect(TkIF)) {
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
    DoConditionType t;
    DoConditionWhen w;
    Expression *c = NULL;
    Block *b = NULL;
    
    if (accept(TkWHILE)) {
        t = DcWHILE;
        w = DcPRECONDITION;
        if ((c = expression())) {
            if ((b = block())) {
                if (expect(TkLOOP)) {
                    return new DoStatement(t, w, c, b);
                }
            }
        }
    }
    else if (accept(TkUNTIL)) {
        t = DcUNTIL;
        w = DcPRECONDITION;
        if ((c = expression())) {
            if ((b = block())) {
                if (expect(TkLOOP)) {
                    return new DoStatement(t, w, c, b);
                }
            }
        }
    }
    else if ((b = block())) {
        if (accept(TkDONE)) {
            t = DcONCE;
            w = DcPOSTCONDITION;
            return new DoStatement(t, w, NULL, b);
        }
        else if (accept(TkLOOP)) {
            w = DcPOSTCONDITION;
            if (accept(TkWHILE) and ((c = expression()))) {
                return new DoStatement(DcWHILE, w, c, b);
            }
            else if (accept(TkUNTIL) and ((c = expression()))) {
                return new DoStatement(DcUNTIL, w, c, b);
            }
            else {
                return new DoStatement(DcFOREVER, w, c, b);
            }
        }
        else {
            error(2, TkLOOP, TkDONE);
        }
    }

    if (c)  delete c;
    if (b)  delete b;
    return NULL;
}

// <for-statement-body> ::= <identifier> "=" <expression> "to" <expression> [ "step" <expression> ] <block> "next" [ <identifier> ]
Basic::ForStatement* Parser::forStatementBody(void) {
    Expression *start = NULL;
    Expression *end = NULL;
    Expression *step = NULL;
    Block *body = NULL;
    if (expect(TkIDENTIFIER)) {
        const char *identifier = this->accepted_token_value.getStringValue();
        if (expect(TkEQUALS)) {
            if ((start = expression())) {
                if (expect(TkTO)) {
                    if ((end = expression())) {
                        if (accept(TkSTEP)) {
                            if (not (step = expression())) {
                                delete end;
                                delete start;
                                return NULL;
                            }
                        }
                        if ((body = block())) {
                            if (expect(TkNEXT)) {
                                accept(TkIDENTIFIER);  // FIXME just ignoring it for now
                                return new ForStatement(identifier, start, end, step, body);
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
    ArrayDimension *dim = NULL;
    if (expect(TkIDENTIFIER)) {
        const char *identifier = this->accepted_token_value.getStringValue();
        dim = arrayDimension();
        DimStatement *s = new DimStatement(identifier, dim);
        while (accept(TkCOMMA)) {
            if (expect(TkIDENTIFIER)) {
                const char *identifier = this->accepted_token_value.getStringValue();
                if (this->token == TkLPAREN) {
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
    if (accept(TkLITERAL)) {
        return new LiteralExpression(this->accepted_token_value);
    }
    else if (accept(TkIDENTIFIER)) {
        const char *identifier = this->accepted_token_value.getStringValue();
        if (accept(TkLPAREN)) {
            ParamList *p = NULL;
            if ((p = paramList())) {
                ParamList *p = paramList();
                if (expect(TkRPAREN)) {
                    return new IdentifierExpression(identifier, p);
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
            return new IdentifierExpression(identifier);
        }
    }
    else if (accept(TkLPAREN)) {
        Expression *e = NULL;
        if ((e = expression())) {
            if (expect(TkRPAREN)) {
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
        error(3, TkLITERAL, TkIDENTIFIER, TkLPAREN);
        return NULL;
    }
}

// <unary-expression> ::= <unary-operator> <primary-expression> | <primary-expression>
// <unary-operator> ::= "not" | "-"
Basic::Expression* Parser::unaryExpression(void) {
    Expression *e = NULL;
    if (accept(TkNOT)) {
        if ((e = primaryExpression())) {
            return new Basic::UnaryExpression(TkNOT, e);            
        }
    }
    else if (accept(TkMINUS)) {
        if ((e = primaryExpression())) {
            return new Basic::UnaryExpression(TkMINUS, e);            
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
    Expression *term = NULL;
    
    if ((term = unaryExpression())) {
        if (this->token == TkMULTIPLY or this->token == TkDIVIDE or this->token == TkMOD) {
            Basic::MultiplicativeExpression *e = new Basic::MultiplicativeExpression();
            e->appendTerm(term);
            e->appendOperator(this->token);
            while (accept(TkMULTIPLY) or accept(TkDIVIDE) or accept(TkMOD)) {
                if ((term = unaryExpression())) {
                    e->appendTerm(term);
                    if (this->token == TkMULTIPLY or this->token == TkDIVIDE or this->token == TkMOD) {
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
        if (this->token == TkPLUS or this->token == TkMINUS) {
            Basic::AdditiveExpression *e = new Basic::AdditiveExpression();
            e->appendTerm(term);
            e->appendOperator(this->token);
            while (accept(TkPLUS) or accept(TkMINUS)) {
                if ((term = multiplicativeExpression())) {
                    e->appendTerm(multiplicativeExpression());
                    if (this->token == TkPLUS or this->token == TkMINUS) {
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
    Expression *first = NULL;
    Expression *second = NULL;
    
    if ((first = additiveExpression())) {
        Token t = this->token;
        if (accept(TkEQUALS) || accept(TkNOTEQUALS) || accept(TkLT) || accept(TkGT) || accept(TkLTEQUALS) || accept(TkGTEQUALS)) {
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
        if (this->token == TkAND) {
            Basic::AndExpression *e = new Basic::AndExpression();
            e->appendTerm(term);
            while (accept(TkAND)) {
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
        if (this->token == TkOR) {
            Basic::OrExpression *e = new Basic::OrExpression();
            e->appendTerm(term);
            while (accept(TkOR)) {
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
