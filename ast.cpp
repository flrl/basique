/*
 *  ast.cpp
 *  Untitled
 *
 *  Created by Daniel on 16/01/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "ast.h"
#include "symboltable.h"

void Basic::FunctionCallExpression::execute (void) {
    
}

void Basic::ArrayIndexExpression::execute (void) {
    
}

void Basic::VariableExpression::execute (void) {
    
}

void Basic::LiteralExpression::execute (void) {
    ;
}

void Basic::UnaryExpression::execute (void) {
    this->term->execute();
    Variant intermediate = term->getResult();
    if (this->op == TkNOT) {
        this->value.setBoolValue(!intermediate.getBoolValue());
    }
    else if (this->op == TkMINUS) {
        switch(intermediate.getType()) {
            case is_int:
                this->value.setIntValue(0 - intermediate.getIntValue());
                break;
            case is_double:
                this->value.setDoubleValue(0.0 - intermediate.getDoubleValue());
                break;
            default:
                fprintf(stderr, "warning: attempt to negate non-numeric value at line %i, column %i\n", this-line, this->column);
                this->value = intermediate;
        }
    }
    else {
        fprintf(stderr, "debug: invalid operator `%s' in UnaryExpression object\n", Tokeniser::tokenDescriptions[this->op]);
    }
}

void Basic::MultiplicativeExpression::execute (void) {
    // FIXME this is where i really want operator overloads for variant    
}

void Basic::AdditiveExpression::execute (void) {
    std::list<Expression*>::iterator term = terms.begin();
    (*term)->execute();
    Variant intermediate = (*term)->getResult();
    term++;
    
    std::list<Token>::iterator op = operators.begin();
    
    do {
        (*term)->execute();
        Variant t = (*term)->getResult();
        
        switch (*op) {
            case TkPLUS:    intermediate = intermediate + t; break;
            case TkMINUS:   intermediate = intermediate - t; break;
            default:
                fprintf(stderr, "debug: invalid operator `%s' in AdditiveExpression object\n", Tokeniser::tokenDescriptions[*op]);
        }
        term++;
        op++;
    } while (term != terms.end() and op != operators.end());
    
    this->value = intermediate;
}

void Basic::ComparitiveExpression::execute (void) {
//    if (accept(TkEQUALS) || accept(TkNOTEQUALS) || accept(TkLT) || accept(TkGT) || accept(TkLTEQUALS) || accept(TkGTEQUALS)) {
    bool intermediate = false;
    switch (this->cmp) {
        case TkEQUALS:      intermediate = (first == second); break;
        case TkNOTEQUALS:   intermediate = (first != second); break;
        case TkLT:          intermediate = (first <  second); break;
        case TkGT:          intermediate = (first >  second); break;
        case TkLTEQUALS:    intermediate = (first <= second); break;
        case TkGTEQUALS:    intermediate = (first >= second); break;
        default: 
            fprintf(stderr, "debug: invalid operator `%s' in ComparitiveExpression object\n", Tokeniser::tokenDescriptions[this->cmp]);
    }
    this->value.setBoolValue(intermediate);
}

void Basic::AndExpression::execute (void) {
    bool intermediate = true;
    for (std::list<Expression*>::iterator e = terms.begin(); e != terms.end(); e++) {
        (*e)->execute();
        intermediate = (intermediate and (*e)->getResult().getBoolValue());
    }
    this->value.setBoolValue(intermediate);
}

void Basic::OrExpression::execute (void) {
    bool intermediate = false;
    for (std::list<Expression*>::iterator e = terms.begin(); e != terms.end(); e++) {
        (*e)->execute();
        intermediate = (intermediate and (*e)->getResult().getBoolValue());
    }
    this->value.setBoolValue(intermediate);
}

void Basic::Block::execute (void) {
    for (std::list<Statement*>::iterator s = statements.begin(); s != statements.end(); s++) {
        (*s)->execute();
    }
}

void Basic::PrintStatement::execute (void) {
    for (std::list<Expression*>::iterator e = expressions.begin(); e != expressions.end(); e++) {
        (*e)->execute();
        fputs((*e)->getResult().getStringValue(), stdout);
    }
    if (append_eol)  putchar('\n');
}

void Basic::ParamList::execute (void) {
    ;  // does nothing
}
