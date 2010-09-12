/*
 *  ast.cpp
 *  Untitled
 *
 *  Created by Ellie on 16/01/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "ast.h"
#include "symboltable.h"

void Basic::LiteralExpression::execute (void) {
    ; // m_result is set at initialisation
}

void Basic::IdentifierExpression::execute (void) {
    
}

void Basic::UnaryExpression::execute (void) {
    m_term->execute();
    Variant intermediate = m_term->getResult();
    if (m_op == TkNOT) {
        m_result.setBoolValue(!intermediate.getBoolValue());
    }
    else if (m_op == TkMINUS) {
        switch(intermediate.getType()) {
            case Basic::Variant::VaINT:
                m_result.setIntValue(0 - intermediate.getIntValue());
                break;
            case Basic::Variant::VaDOUBLE:
                m_result.setDoubleValue(0.0 - intermediate.getDoubleValue());
                break;
            default:
                fprintf(stderr, "warning: attempt to negate non-numeric value at line %i, column %i\n", m_line, m_column);
                m_result = intermediate;
        }
    }
    else {
        fprintf(stderr, "debug: invalid operator `%s' in UnaryExpression object\n", Tokeniser::tokenDescriptions[m_op]);
    }
}

void Basic::MultiplicativeExpression::execute (void) {
    m_first_term->execute();
    Variant intermediate = m_first_term->getResult();
    
    for (std::list<OperatorTerm>::const_iterator term = m_other_terms.begin(); term != m_other_terms.end(); term++) {
        term->term->execute();
        Variant t = term->term->getResult();
        
        switch (term->op) {
            case TkMULTIPLY:    intermediate *= t; break;
            case TkDIVIDE:      intermediate /= t; break;
            default:
                fprintf(stderr, "debug: invalid operator `%s' in MultiplicativeExpression object\n", Tokeniser::tokenDescriptions[term->op]);
        }
    }
    
    m_result = intermediate;
}

void Basic::AdditiveExpression::execute (void) {
    m_first_term->execute();
    Variant intermediate = m_first_term->getResult();
    
    for (std::list<OperatorTerm>::const_iterator term = m_other_terms.begin(); term != m_other_terms.end(); term++) {
        term->term->execute();
        Variant t = term->term->getResult();
        
        switch (term->op) {
            case TkPLUS:    intermediate += t; break;
            case TkMINUS:   intermediate -= t; break;
            default:
                fprintf(stderr, "debug: invalid operator `%s' in AdditiveExpression object\n", Tokeniser::tokenDescriptions[term->op]);
        }
    }
    
    m_result = intermediate;
}

void Basic::ComparitiveExpression::execute (void) {
    bool intermediate = false;
    switch (m_cmp) {
        case TkEQUALS:      intermediate = (m_first == m_second); break;
        case TkNOTEQUALS:   intermediate = (m_first != m_second); break;
        case TkLT:          intermediate = (m_first <  m_second); break;
        case TkGT:          intermediate = (m_first >  m_second); break;
        case TkLTEQUALS:    intermediate = (m_first <= m_second); break;
        case TkGTEQUALS:    intermediate = (m_first >= m_second); break;
        default: 
            fprintf(stderr, "debug: invalid operator `%s' in ComparitiveExpression object\n", Tokeniser::tokenDescriptions[m_cmp]);
    }
    m_result.setBoolValue(intermediate);
}

void Basic::AndExpression::execute (void) {
    bool intermediate = true;
    for (std::list<Basic::Expression*>::const_iterator e = m_terms.begin(); e != m_terms.end(); e++) {
        (*e)->execute();
        intermediate = (intermediate and (*e)->getResult().getBoolValue());
    }
    m_result.setBoolValue(intermediate);
}

void Basic::OrExpression::execute (void) {
    bool intermediate = false;
    for (std::list<Basic::Expression*>::const_iterator e = m_terms.begin(); e != m_terms.end(); e++) {
        (*e)->execute();
        intermediate = (intermediate and (*e)->getResult().getBoolValue());
    }
    m_result.setBoolValue(intermediate);
}

void Basic::Block::execute (void) {
    for (std::list<Basic::Statement*>::const_iterator s = m_statements.begin(); s != m_statements.end(); s++) {
        (*s)->execute();
    }
}

void Basic::PrintStatement::execute (void) {
    for (std::list<Basic::Expression*>::const_iterator e = m_expressions.begin(); e != m_expressions.end(); e++) {
        (*e)->execute();
        fputs((*e)->getResult().getStringValue(), stdout);
    }
    if (m_append_eol)  putchar('\n');
}

void Basic::InputStatement::execute (void) {
    
}

void Basic::LetStatement::execute (void) {
    
}

void Basic::CallStatement::execute (void) {
    
}

void Basic::IfStatement::execute (void) {
    for (std::list<ConditionalBlock>::const_iterator cb = m_conditional_blocks.begin(); cb != m_conditional_blocks.end(); cb++) {
        cb->condition->execute();
        if (cb->condition->getResult().getBoolValue() == true) {
            cb->block->execute();
            return;
        }
    }
    
    m_else_block->execute();
}

void Basic::DoStatement::execute (void) {
    bool condition_reached = false;
    while (condition_reached == false) {
        if (m_condition_when == DcPRECONDITION) {
            m_condition->execute();
            switch (m_condition_type) {
                case DcWHILE:
                    condition_reached = not m_condition->getResult().getBoolValue();
                    break;
                case DcUNTIL:
                    condition_reached = m_condition->getResult().getBoolValue();
                    break;
                case DcFOREVER:
                    condition_reached = false;
                    break;
                case DcONCE:
                    condition_reached = true;
                    break;
                default:
                    fprintf(stderr, "warning: got here (DcPreCondition)\n");
            }
            if (condition_reached) break;
        }
        
        m_body->execute();
        
        if (m_condition_when == DcPOSTCONDITION) {
            m_condition->execute();
            switch (m_condition_type) {
                case DcWHILE:
                    condition_reached = not m_condition->getResult().getBoolValue();
                    break;
                case DcUNTIL:
                    condition_reached = m_condition->getResult().getBoolValue();
                    break;
                case DcFOREVER:
                    condition_reached = false;
                    break;
                case DcONCE:
                    condition_reached = true;
                    break;
                default:
                    fprintf(stderr, "warning: got here (DcPostCondition)\n");
            }
        }
    }
}

void Basic::ForStatement::execute (void) {
    
}

void Basic::DimStatement::execute (void) {
    
}

void Basic::AcceptedParamList::execute (void) {
    
}

void Basic::ParamList::execute (void) {
    ;  // does nothing
}

void Basic::ArraySubscript::execute (void) {
    
}

void Basic::ArrayDimension::execute (void) {
    
}

void Basic::FunctionDefinition::execute (void) {
    
}

void Basic::SubDefinition::execute (void) {
    
}

#pragma mark Destructors

Basic::PrintStatement::~PrintStatement() {
    for (std::list<Basic::Expression *>::iterator e = m_expressions.begin(); e != m_expressions.end(); e++) {
        delete (*e);
    }
}

Basic::IfStatement::~IfStatement() {
    for (std::list<ConditionalBlock>::iterator b = m_conditional_blocks.begin(); b != m_conditional_blocks.end(); b++) {
        delete b->condition;
        delete b->block;
    }
    
    if (m_else_block)  delete m_else_block;
}

Basic::ForStatement::~ForStatement() {
    delete m_start;
    delete m_end;
    if (m_step)  delete m_step;
    delete m_body;
}

Basic::DimStatement::~DimStatement() {
    for (std::list<Dimensionable>::iterator d = m_dimensionables.begin(); d != m_dimensionables.end(); d++) {
        if (d->second)  delete d->second;
    }
}

Basic::ArraySubscript::~ArraySubscript() {
    for (std::list<Basic::Expression *>::iterator e = m_expressions.begin(); e != m_expressions.end(); e++) {
        delete (*e);
    }
}

Basic::ArrayDimension::~ArrayDimension() {
    for (std::list<ArrayDimension::Specification>::iterator d = m_dimensions.begin(); d != m_dimensions.end(); d++) {
        delete d->first;
        if (d->second)  delete d->second;
    }
}

Basic::AcceptedParamList::~AcceptedParamList() {
    // FIXME
}

Basic::ParamList::~ParamList() {
    for (std::vector<Expression *>::iterator e = m_expressions.begin(); e != m_expressions.end(); e++) {
        delete (*e);
    }
}

Basic::Block::~Block() {
    for (std::list<Basic::Statement *>::iterator s = m_statements.begin(); s != m_statements.end(); s++) {
        delete (*s);
    }
}

Basic::MultiplicativeExpression::~MultiplicativeExpression() {
    delete m_first_term;
    for (std::list<OperatorTerm>::iterator e = m_other_terms.begin(); e != m_other_terms.end(); e++) {
        delete (e->term);
    }
}

Basic::AdditiveExpression::~AdditiveExpression() {
    delete m_first_term;
    for (std::list<OperatorTerm>::iterator e = m_other_terms.begin(); e != m_other_terms.end(); e++) {
        delete (e->term);
    }
}


Basic::AndExpression::~AndExpression() {
    for (std::list<Basic::Expression *>::iterator e = m_terms.begin(); e != m_terms.end(); e++) {
        delete (*e);
    }
}

Basic::OrExpression::~OrExpression() {
    for (std::list<Basic::Expression *>::iterator e = m_terms.begin(); e != m_terms.end(); e++) {
        delete (*e);
    }    
}
