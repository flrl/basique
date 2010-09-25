/*
 *  parser.cpp
 *  Untitled
 *
 *  Created by Ellie on 10/01/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include <cstdarg>
#include <cstdio>

#include "parser.h"

bool basic::Parser::accept(Token t) {
    if (m_token == t) {
        m_accepted_token_value = m_tokeniser->getValue();
        m_accepted_token_line = m_tokeniser->getLine();
        m_accepted_token_column = m_tokeniser->getColumn();
        m_accepted_token = m_token;
        m_token = m_tokeniser->getToken();
        return true;
    }
    else {
        return false;
    }
}

inline bool basic::Parser::expect(Token t) {
    if (accept(t)) {
        return true;
    } 
    else {
        error(1, t);
        return false;     
    }
}

void basic::Parser::error(int argc, ...) {
    fprintf(stderr, "Syntax error at line %i, column %i: expected", m_tokeniser->getLine(), m_tokeniser->getColumn());
    va_list argv;
    va_start(argv, argc);
    for (int i=0; i<argc; i++) {
        int token = va_arg(argv, int);
        if (token >= Tk_MIN && token < Tk_MAX) {
            fprintf(stderr, " `%s',", Tokeniser::tokenDescriptions[token]);
        }
    }
    va_end(argv);
    fprintf(stderr, " got `%s'\n", Tokeniser::tokenDescriptions[m_token]);
}

inline bool basic::Parser::isValidExpressionToken(Token t) const {
    switch (t) {
        case TkLITERAL:
        case TkIDENTIFIER:
        case TkLPAREN:
        case TkNOT:
        case TkMINUS:
            return true;
        default:
            return false;
    }
}


// <unit> ::= "function" <function-definition-body> <eol>
//          | "sub" <sub-definition-body> <eol>
//          | <statement> <eol>
//          | <null> <eol>
basic::Unit* basic::Parser::unit(void) {
    Unit *unit = NULL;

    while (accept(TkEOL)) {
        ; // skip any empty lines
    }
    
    if (m_token == TkEOF /*accept(TkEOF)*/) {
        return NULL;
    }
    else if (accept(TkFUNCTION)) {
        unit = functionDefinitionBody();
    }
    else if (accept(TkSUB)) {
        unit = subDefinitionBody();
    }
    else {
        unit = statement();
    }

    if (unit) {
        // N.B. don't use accept() here, otherwise it will wait until there's another token ready before executing the current line
        if (m_token == TkEOL or m_token == TkEOF) {
            return unit;
        }
        else {
            error(2, TkEOL, TkEOF);
            delete unit;
            return NULL;
        }
    }
    else {
        return NULL;
    }
}

// <function-definition-body> ::= <identifier> "(" <accepted-param-list> ")" <block> "end" "function"
basic::FunctionDefinition* basic::Parser::functionDefinitionBody(void) {
    AcceptedParamList *a = NULL;
    Block *b = NULL;
    
    int line = m_accepted_token_line;
    int column = m_accepted_token_column;
    
    if (expect(TkIDENTIFIER)) {
        String identifier(m_accepted_token_value.getStringValue());
        if (expect(TkLPAREN)) {
            if ((a = acceptedParamList())) {
                if (expect(TkRPAREN)) {
                    if ((b = block())) {
                        if (expect(TkEND) and expect(TkFUNCTION)) {
                            FunctionDefinition *f = new FunctionDefinition(identifier, a, b);
                            f->setPosition(line, column);
                            return f;
                        }
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
basic::SubDefinition* basic::Parser::subDefinitionBody(void) {
    AcceptedParamList *a = NULL;
    Block *b = NULL;

    int line = m_accepted_token_line;
    int column = m_accepted_token_column;
    
    if (expect(TkIDENTIFIER)) {
        String identifier(m_accepted_token_value.getStringValue());
        if (expect(TkLPAREN)) {
            if ((a = acceptedParamList())) {        
                if (expect(TkRPAREN)) {
                    if ((b = block())) {
                        if (expect(TkEND) and expect(TkSUB)) {
                            SubDefinition *s = new SubDefinition(identifier, a, b);
                            s->setPosition(line, column);
                            return s;
                        }
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
//               | "open" <open-statement-body>
//               | "close" <close-statement-body>
//               | "exit" [ <expression> ] // FIXME
//               | <let-statement-body> // FIXME
//               | <call-statement-body> // FIXME
basic::Statement* basic::Parser::statement(void) {
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
    else if (accept(TkOPEN)) {
        return openStatementBody();
    }
    else if (accept(TkCLOSE)) {
        return closeStatementBody();
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
        error(10, TkPRINT, TkINPUT, TkLET, TkCALL, TkIF, TkDO, TkFOR, TkDIM, TkOPEN, TkCLOSE, TkEXIT, TkIDENTIFIER);
        return NULL;
    }
}

// <block> ::= <block-statement-list>
// <block-statement-list> ::= <statement> [ ( ":" | <eol> ) <block-statement-list> ]...
//                          | <null>
basic::Block* basic::Parser::block(void) {
    Block *block = new Block();
    block->setPosition(m_accepted_token_line, m_accepted_token_column);
    
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
            case TkOPEN:
            case TkCLOSE:
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
basic::AcceptedParamList* basic::Parser::acceptedParamList(void) {
    AcceptedParamList *a = new AcceptedParamList();
    a->setPosition(m_accepted_token_line, m_accepted_token_column);
    
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
basic::ArraySubscript* basic::Parser::arraySubscript(void) {
    Expression *e = NULL;

    if (expect(TkLPAREN)) {
        int line = m_accepted_token_line;
        int column = m_accepted_token_column;

        if ((e = expression())) {
            ArraySubscript *sub = new ArraySubscript(e);
            sub->setPosition(line, column);
            
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
basic::ArrayDimension* basic::Parser::arrayDimension(void) {
    Expression *a = NULL;
    Expression *b = NULL;
    if (expect(TkLPAREN)) {
        int line = m_accepted_token_line;
        int column = m_accepted_token_column;
                
        if ((a = expression())) {
            if (accept(TkTO)) {
                if (not (b = expression())) {
                    delete a;
                    return NULL;
                }
            }
            ArrayDimension *dim = new ArrayDimension(a, b);
            dim->setPosition(line, column);
            
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
            
            if (expect(TkRPAREN)) {
                return dim;                
            }
            else {
                delete dim;
                return NULL;
            }
        }
    }
    
    return NULL;
}

// <param-list> ::= <expression> [ "," <expression> ]... 
//                | <null>
basic::ParamList* basic::Parser::paramList(void) {
    ParamList *p = new ParamList();
    p->setPosition(m_accepted_token_line, m_accepted_token_column);

    if (isValidExpressionToken(m_token)) {
        Expression *e = NULL;
        if ((e = expression())) {
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
        else {
            delete p;
            return NULL;
        }
    }
    
    return p;
}

// <print-statement-body> ::= [ "#" <identifier> ] <print-expression-list>
// <print-expression-list> ::= <expression> [ "," <expression> ]... [ "," ]
//                           | <null>
basic::PrintStatement* basic::Parser::printStatementBody(void) {
    int line = m_accepted_token_line;
    int column = m_accepted_token_column;
        
    String file_identifier;
    if (accept(TkHASH)) {
        if (expect(TkIDENTIFIER)) {
            file_identifier = m_accepted_token_value.getStringValue();
        }
        else {
            return NULL;
        }
    }
    
    PrintStatement *s = new PrintStatement(file_identifier);
    s->setPosition(line, column);

    if (isValidExpressionToken(m_token)) {
        Expression *e = NULL;
        if ((e = expression())) {
            s->appendExpression(e);
            while (accept(TkCOMMA)) {
                if (isValidExpressionToken(m_token)) {
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

// <input-statement-body> ::= [ "#" <identifier> ] <identifier> [ <array-subscript> ] [ "," <expression> ]
basic::InputStatement* basic::Parser::inputStatementBody(void) {
    int line = m_accepted_token_line;
    int column = m_accepted_token_column;
    
    String file_identifier;
    if (accept(TkHASH)) {
        if (expect(TkIDENTIFIER)) {
            file_identifier = m_accepted_token_value.getStringValue();
        }
        else {
            return NULL;
        }
    }
    
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
        InputStatement *s = new InputStatement(file_identifier, identifier, subscript, prompt);
        s->setPosition(line, column);
        return s;
    }
    else {
        return NULL;
    }
}

// <let-statement-body> ::= <identifier> [ <array-subscript> ] "=" <expression>
basic::LetStatement* basic::Parser::letStatementBody(void) {
    ArraySubscript *subscript = NULL;
    Expression *e = NULL;

    int line = m_accepted_token_line;
    int column = m_accepted_token_column;
        
    if (expect(TkIDENTIFIER)) {
        String identifier(m_accepted_token_value.getStringValue());
        if (m_token == TkLPAREN and not (subscript = arraySubscript())) {
            return NULL;
        }
        if (expect(TkEQUALS)) {
            if ((e = expression())) {
                LetStatement *s = new LetStatement(identifier, subscript, e);
                s->setPosition(line, column);
                return s;
            }
        }
    }
    
    if (subscript)  delete subscript;
    if (e)  delete e;
    return NULL;
}

// <call-statement-body> ::= <identifier> "(" <param-list> ")"
basic::CallStatement* basic::Parser::callStatementBody(void) {
    ParamList *p = NULL;

    int line = m_accepted_token_line;
    int column = m_accepted_token_column;
        
    if (expect(TkIDENTIFIER)) {
        String identifier(m_accepted_token_value.getStringValue());
        if (expect(TkLPAREN)) {
            if((p = paramList())) {
                if (expect(TkRPAREN)) {
                    CallStatement *s = new CallStatement(identifier, p);
                    s->setPosition(line, column);
                    return s;
                }
            }
        }
    }
    
    if (p)  delete p;
    return NULL;
}

// <if-statement-body> ::= <expression> "then" <block> [ "elseif" <expression> "then" <block> ]... [ "else" <block> ] "end" "if"
basic::IfStatement* basic::Parser::ifStatementBody(void) {
    IfStatement *s = NULL;
    Expression *e = NULL;
    Block *b = NULL;
    
    int line = m_accepted_token_line;
    int column = m_accepted_token_column;
        
    if ((e = expression())) {
        s = new IfStatement();
        s->setPosition(line, column);
        
        if (expect(TkTHEN) and (b = block())) {
            s->appendCondition(e, b);
            
            while (accept(TkELSEIF)) {
                if ((e = expression())) {
                    if (expect(TkTHEN) and (b = block())) {
                        s->appendCondition(e, b);
                    }
                }
                else {
                    if (s)  delete s;
                    return NULL;
                }
            }
            if (accept(TkELSE) and (b = block())) {
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
basic::DoStatement* basic::Parser::doStatementBody(void) {
    DoStatement::Type t;
    DoStatement::When w;
    Expression *c = NULL;
    Block *b = NULL;

    int line = m_accepted_token_line;
    int column = m_accepted_token_column;
        
    if (accept(TkWHILE)) {
        t = DoStatement::DcWHILE;
        w = DoStatement::DcPRECONDITION;
        if ((c = expression())) {
            if ((b = block())) {
                if (expect(TkLOOP)) {
                    DoStatement *s = new DoStatement(t, w, c, b);
                    s->setPosition(line, column);
                    return s;
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
                    DoStatement *s = new DoStatement(t, w, c, b);
                    s->setPosition(line, column);
                    return s;
                }
            }
        }
    }
    else if ((b = block())) {
        if (accept(TkDONE)) {
            t = DoStatement::DcONCE;
            w = DoStatement::DcPOSTCONDITION;
            DoStatement *s = new DoStatement(t, w, NULL, b);
            s->setPosition(line, column);
            return s;
        }
        else if (accept(TkLOOP)) {
            w = DoStatement::DcPOSTCONDITION;
            if (accept(TkWHILE) and ((c = expression()))) {
                DoStatement *s = new DoStatement(DoStatement::DcWHILE, w, c, b);
                s->setPosition(line, column);
                return s;
            }
            else if (accept(TkUNTIL) and ((c = expression()))) {
                DoStatement *s = new DoStatement(DoStatement::DcUNTIL, w, c, b);
                s->setPosition(line, column);
                return s;
            }
            else {
                DoStatement *s = new DoStatement(DoStatement::DcFOREVER, w, c, b);
                s->setPosition(line, column);
                return s;
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
basic::ForStatement* basic::Parser::forStatementBody(void) {
    Expression *start = NULL;
    Expression *end = NULL;
    Expression *step = NULL;
    Block *body = NULL;
    
    int line = m_accepted_token_line;
    int column = m_accepted_token_column;
        
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
                                ForStatement *s = new ForStatement(identifier, start, end, step, body);
                                s->setPosition(line, column);
                                return s;
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
basic::DimStatement* basic::Parser::dimStatementBody(void) {
    ArrayDimension *dim = NULL;

    int line = m_accepted_token_line;
    int column = m_accepted_token_column;
    
    if (expect(TkIDENTIFIER)) {
        String identifier(m_accepted_token_value.getStringValue());
        if (m_token == TkLPAREN) {
            if (not (dim = arrayDimension())) {
                return NULL;
            }
        }
        DimStatement *s = new DimStatement(identifier, dim);
        s->setPosition(line, column);
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

// <open-statement-body> ::= <expression> "for" ( "input" | "output" | "append" ) "as" [ "#" ] <identifier>
basic::OpenStatement* basic::Parser::openStatementBody() {
    Expression *e = NULL;
    Token mode = TkINVALID;

    int line = m_accepted_token_line;
    int column = m_accepted_token_column;
        
    if ((e = expression())) {
        if (expect(TkFOR)) {
            if (accept(TkINPUT) or accept(TkOUTPUT) or accept(TkAPPEND)) {
                mode = m_accepted_token;
                if (expect(TkAS)) {
                    accept(TkHASH);
                    if (expect(TkIDENTIFIER)) {
                        OpenStatement *s = new OpenStatement(e, mode, m_accepted_token_value.getStringValue());
                        s->setPosition(line, column);
                        return s;
                    }
                }
            }
        }
    }

    if (e)  delete e;
    return NULL;
}

// <close-statement-body> ::= [ "#" ] <identifier>
basic::CloseStatement* basic::Parser::closeStatementBody() {
    int line = m_accepted_token_line;
    int column = m_accepted_token_column;
        
    if (m_token == TkHASH)  accept(TkHASH);

    if (expect(TkIDENTIFIER)) {
        CloseStatement *s = new CloseStatement(m_accepted_token_value.getStringValue());
        s->setPosition(line, column);
        return s;
    }
    else {
        return NULL;
    }
}

// <primary-expression> ::= <literal>
//                        | <identifier> [ "(" <param-list> ")" ]
//                        | "(" <expression> ")"
basic::Expression* basic::Parser::primaryExpression(void) {
    if (accept(TkLITERAL)) {
        LiteralExpression *e = new LiteralExpression(m_accepted_token_value);
        e->setPosition(m_accepted_token_line, m_accepted_token_column);
        return e;
    }
    else if (accept(TkIDENTIFIER)) {
        int line = m_accepted_token_line;
        int column = m_accepted_token_column;
        
        String identifier(m_accepted_token_value.getStringValue());
        if (accept(TkLPAREN)) {
            ParamList *p = NULL;
            if ((p = paramList())) {
                if (expect(TkRPAREN)) {
                    IdentifierExpression *e = new IdentifierExpression(identifier, p);
                    e->setPosition(line, column);
                    return e;
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
            IdentifierExpression *e = new IdentifierExpression(identifier);
            e->setPosition(line, column);
            return e;
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
basic::Expression* basic::Parser::unaryExpression(void) {
    Expression *e = NULL;
    if (accept(TkNOT) or accept(TkMINUS)) {
        int line = m_accepted_token_line;
        int column = m_accepted_token_column;
        Token op = m_accepted_token;
        
        if ((e = primaryExpression())) {
            UnaryExpression *e = new UnaryExpression(op, e);
            e->setPosition(line, column);
            return e;
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
basic::Expression* basic::Parser::multiplicativeExpression(void) {
    Expression *term = NULL;
    
    if ((term = unaryExpression())) {
        if (m_token == TkMULTIPLY or m_token == TkDIVIDE or m_token == TkMOD) {
            MultiplicativeExpression *e = new MultiplicativeExpression(term);
            e->setPosition(term->getLine(), term->getColumn());
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
basic::Expression* basic::Parser::additiveExpression(void) {
    Expression *term = NULL;
    
    
    if ((term = multiplicativeExpression())) {
        if (m_token == TkPLUS or m_token == TkMINUS) {
            AdditiveExpression *e = new AdditiveExpression(term);
            e->setPosition(term->getLine(), term->getColumn());
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
basic::Expression* basic::Parser::comparitiveExpression(void) {
    Expression *first = NULL;
    Expression *second = NULL;
    
    if ((first = additiveExpression())) {
        Token t = m_token;
        if (accept(TkEQUALS) || accept(TkNOTEQUALS) || accept(TkLT) || accept(TkGT) || accept(TkLTEQUALS) || accept(TkGTEQUALS)) {
            if ((second = additiveExpression())) {
                ComparitiveExpression *e = new ComparitiveExpression(first, t, second);
                e->setPosition(first->getLine(), first->getColumn());
                return e;
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
basic::Expression* basic::Parser::andExpression(void) {
    Expression *term = NULL;
    
    if ((term = comparitiveExpression())) {
        if (m_token == TkAND) {
            AndExpression *e = new AndExpression();
            e->setPosition(term->getLine(), term->getColumn());
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
basic::Expression* basic::Parser::orExpression(void) {
    Expression *term = NULL;
    
    if ((term = andExpression())) {
        if (m_token == TkOR) {
            OrExpression *e = new OrExpression();
            e->setPosition(term->getLine(), term->getColumn());
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
basic::Expression* basic::Parser::expression(void) {
    return orExpression();
}
