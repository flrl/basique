/*
 *  ast.cpp
 *  Untitled
 *
 *  Created by Ellie on 16/01/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "ast.h"

void Basic::FunctionCallExpression::execute (void) {
    
}

void Basic::ArrayIndexExpression::execute (void) {
    
}

void Basic::VariableExpression::execute (void) {
    
}

void Basic::LiteralExpression::execute (void) {

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
    
}

void Basic::AdditiveExpression::execute (void) {
    
}

void Basic::ComparitiveExpression::execute (void) {
    
}

void Basic::AndExpression::execute (void) {
    
}

void Basic::OrExpression::execute (void) {
    
}

void Basic::Block::execute (void) {
    for (std::list<Statement*>::iterator s = statements.begin(); s != statements.end(); s++) {
        (*s)->execute();
    }    
}

void Basic::PrintStatement::execute (void) {

}

void Basic::ParamList::execute (void) {
    ;  // does nothing
}
