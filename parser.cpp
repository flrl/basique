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

bool Basic::Parser::accept(Token t) {
    if (m_token == t) {
        m_accepted_token_value = m_tokeniser.getValue();
        m_accepted_token_line = m_tokeniser.getLine();
        m_accepted_token_column = m_tokeniser.getColumn();
        m_accepted_token = m_token;
        m_token = m_tokeniser.getToken();
        return true;
    }
    else {
        return false;
    }
}

bool Basic::Parser::expect(Token t) {
    if (accept(t)) {
        return true;
    } 
    else {
        error(1, t);
        return false;     
    }
}

void Basic::Parser::error(int argc, ...) {
    fprintf(stderr, "Syntax error at line %i, column %i: expected", m_tokeniser.getLine(), m_tokeniser.getColumn());
    va_list argv;
    va_start(argv, argc);
    for (int i=0; i<argc; i++) {
        int token = va_arg(argv, int);
        if (token >= Tk_MIN && token < Tk_MAX) {
            fprintf(stderr, " `%s',", Tokeniser::tokenDescriptions[va_arg(argv, int)]);
        }
    }
    va_end(argv);
    fprintf(stderr, " got `%s'\n", Tokeniser::tokenDescriptions[m_token]);
}


// <unit> ::= "function" <function-definition-body> <eol>
//          | "sub" <sub-definition-body> <eol>
//          | <statement> <eol>
Basic::ASTNode* Basic::Parser::unit(void) {
    ASTNode *unit = NULL;

    if (accept(TkFUNCTION)) {
        unit = functionDefinitionBody();
    }
    else if (accept(TkSUB)) {
        unit = subDefinitionBody();
    }
    else {
        unit = statement();
    }

    if (unit) {
        if (expect(TkEOL)) {
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
Basic::FunctionDefinition* Basic::Parser::functionDefinitionBody(void) {
    AcceptedParamList *a = NULL;
    Block *b = NULL;
    if (expect(TkIDENTIFIER)) {
        String identifier(m_accepted_token_value.getStringValue());
        if (expect(TkLPAREN)) {
            if (m_token != TkRPAREN) {
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
Basic::SubDefinition* Basic::Parser::subDefinitionBody(void) {
    AcceptedParamList *a = NULL;
    Block *b = NULL;
    if (expect(TkIDENTIFIER)) {
        String identifier(m_accepted_token_value.getStringValue());
        if (expect(TkLPAREN)) {
            if (m_token != TkRPAREN) {
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
Basic::Statement* Basic::Parser::statement(void) {
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
Basic::Block* Basic::Parser::block(void) {
    Block *block = new Block();
    Statement *s = NULL;
    do {
        switch (m_token) {
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
Basic::AcceptedParamList* Basic::Parser::acceptedParamList(void) {
    AcceptedParamList *a = new AcceptedParamList();
    if (accept(TkIDENTIFIER)) {
        a->appendIdentifier(m_accepted_token_value.getStringValue());
        while(accept(TkCOMMA)) {
            if (expect(TkIDENTIFIER)) {
                a->appendIdentifier(m_accepted_token_value.getStringValue());
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
Basic::ArraySubscript* Basic::Parser::arraySubscript(void) {
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
Basic::ArrayDimension* Basic::Parser::arrayDimension(void) {
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
Basic::ParamList* Basic::Parser::paramList(void) {
    ParamList *p = new ParamList();
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
// <print-expression-list> ::= <expression> [ "," <expression> ]... [ "," ]
//                           | <null>
Basic::PrintStatement* Basic::Parser::printStatementBody(void) {
    PrintStatement *s = new PrintStatement();

    if (m_token == TkLITERAL or m_token == TkIDENTIFIER or m_token == TkLPAREN) {
        Expression *e = NULL;
        if ((e = expression())) {
            s->appendExpression(e);
            while (accept(TkCOMMA)) {
                if (m_token == TkLITERAL or m_token == TkIDENTIFIER or m_token == TkLPAREN) {
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
Basic::InputStatement* Basic::Parser::inputStatementBody(void) {
    if (expect(TkIDENTIFIER)) {
        String identifier(m_accepted_token_value.getStringValue());
        ArraySubscript *subscript = NULL;
        Expression *prompt = NULL;
        if (m_token == TkLPAREN) {
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
Basic::LetStatement* Basic::Parser::letStatementBody(void) {
    ArraySubscript *s = NULL;
    Expression *e = NULL;

    if (expect(TkIDENTIFIER)) {
        String identifier(m_accepted_token_value.getStringValue());
        if (m_token == TkLPAREN and not (s = arraySubscript())) {
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
Basic::CallStatement* Basic::Parser::callStatementBody(void) {
    ParamList *p = NULL;

    if (expect(TkIDENTIFIER)) {
        String identifier(m_accepted_token_value.getStringValue());
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
Basic::IfStatement* Basic::Parser::ifStatementBody(void) {
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
Basic::DoStatement* Basic::Parser::doStatementBody(void) {
    DoStatement::Type t;
    DoStatement::When w;
    Expression *c = NULL;
    Block *b = NULL;
    
    if (accept(TkWHILE)) {
        t = DoStatement::DcWHILE;
        w = DoStatement::DcPRECONDITION;
        if ((c = expression())) {
            if ((b = block())) {
                if (expect(TkLOOP)) {
                    return new DoStatement(t, w, c, b);
                }
            }
        }
    }
    else if (accept(TkUNTIL)) {
        t = DoStatement::DcUNTIL;
        w = DoStatement::DcPRECONDITION;
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
            t = DoStatement::DcONCE;
            w = DoStatement::DcPOSTCONDITION;
            return new DoStatement(t, w, NULL, b);
        }
        else if (accept(TkLOOP)) {
            w = DoStatement::DcPOSTCONDITION;
            if (accept(TkWHILE) and ((c = expression()))) {
                return new DoStatement(DoStatement::DcWHILE, w, c, b);
            }
            else if (accept(TkUNTIL) and ((c = expression()))) {
                return new DoStatement(DoStatement::DcUNTIL, w, c, b);
            }
            else {
                return new DoStatement(DoStatement::DcFOREVER, w, c, b);
            }
        }
        else {
            error(2, TkLOOP, TkDONE);
        }
    }
    else {
        error(2, TkWHILE, TkUNTIL);  // FIXME error should say "expected while, until or block"
    }

    if (c)  delete c;
    if (b)  delete b;
    return NULL;
}

// <for-statement-body> ::= <identifier> "=" <expression> "to" <expression> [ "step" <expression> ] <block> "next" [ <identifier> ]
Basic::ForStatement* Basic::Parser::forStatementBody(void) {
    Expression *start = NULL;
    Expression *end = NULL;
    Expression *step = NULL;
    Block *body = NULL;
    if (expect(TkIDENTIFIER)) {
        String identifier(m_accepted_token_value.getStringValue());
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
Basic::DimStatement* Basic::Parser::dimStatementBody(void) {
    ArrayDimension *dim = NULL;
    if (expect(TkIDENTIFIER)) {
        String identifier(m_accepted_token_value.getStringValue());
        dim = arrayDimension();
        DimStatement *s = new DimStatement(identifier, dim);
        while (accept(TkCOMMA)) {
            if (expect(TkIDENTIFIER)) {
                String identifier(m_accepted_token_value.getStringValue());
                if (m_token == TkLPAREN) {
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
Basic::Expression* Basic::Parser::primaryExpression(void) {
    if (accept(TkLITERAL)) {
        return new LiteralExpression(m_accepted_token_value);
    }
    else if (accept(TkIDENTIFIER)) {
        String identifier(m_accepted_token_value.getStringValue());
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
Basic::Expression* Basic::Parser::unaryExpression(void) {
    Expression *e = NULL;
    if (accept(TkNOT)) {
        if ((e = primaryExpression())) {
            return new UnaryExpression(TkNOT, e);            
        }
    }
    else if (accept(TkMINUS)) {
        if ((e = primaryExpression())) {
            return new UnaryExpression(TkMINUS, e);            
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
Basic::Expression* Basic::Parser::multiplicativeExpression(void) {
    Expression *term = NULL;
    
    if ((term = unaryExpression())) {
        if (m_token == TkMULTIPLY or m_token == TkDIVIDE or m_token == TkMOD) {
            MultiplicativeExpression *e = new MultiplicativeExpression(term);
            while (accept(TkMULTIPLY) or accept(TkDIVIDE) or accept(TkMOD)) {
                Token op = m_accepted_token;
                if ((term = unaryExpression())) {
                    e->appendTerm(op, term);
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
Basic::Expression* Basic::Parser::additiveExpression(void) {
    Expression *term = NULL;
    
    
    if ((term = multiplicativeExpression())) {
        if (m_token == TkPLUS or m_token == TkMINUS) {
            AdditiveExpression *e = new AdditiveExpression(term);
            while (accept(TkPLUS) or accept(TkMINUS)) {
                Token op = m_accepted_token;
                if ((term = multiplicativeExpression())) {
                    e->appendTerm(op, term);
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
Basic::Expression* Basic::Parser::comparitiveExpression(void) {
    Expression *first = NULL;
    Expression *second = NULL;
    
    if ((first = additiveExpression())) {
        Token t = m_token;
        if (accept(TkEQUALS) || accept(TkNOTEQUALS) || accept(TkLT) || accept(TkGT) || accept(TkLTEQUALS) || accept(TkGTEQUALS)) {
            if ((second = additiveExpression())) {
                return new ComparitiveExpression(first, t, second);                
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
Basic::Expression* Basic::Parser::andExpression(void) {
    Expression *term = NULL;
    
    if ((term = comparitiveExpression())) {
        if (m_token == TkAND) {
            AndExpression *e = new AndExpression();
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
Basic::Expression* Basic::Parser::orExpression(void) {
    Expression *term = NULL;
    
    if ((term = andExpression())) {
        if (m_token == TkOR) {
            OrExpression *e = new OrExpression();
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
Basic::Expression* Basic::Parser::expression(void) {
    return orExpression();
}
