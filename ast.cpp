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

void basic::LiteralExpression::execute() const {
    ; // m_result is set at initialisation
}

void basic::IdentifierExpression::execute() const {
    // FIXME this needs lots of sanity checking
    SymbolTable::Entry *object = NULL;
    if ((object = g_symbol_table.find(m_identifier))) {        
        switch (object->type) {
            case SymbolTable::BUILTIN_FUNCTION:
                m_result = object->builtin_function(m_params);
                break;
            case SymbolTable::FUNCTION:
                m_result = object->function->call(m_params);
                break;
            case SymbolTable::SUBROUTINE:
                fprintf(stderr, "warning: attempting to call subroutine `%s' in expression at line %i, column %i\n",
                        m_identifier.c_str(), m_line, m_column);
                object->sub->call(m_params);
                m_result.setUndefined();
                break;
            case SymbolTable::VARIANT:
                m_result = *object->variant;
                break;
            case SymbolTable::ARRAY: 
                {
                    Array::Index index;
                    for (size_t i = 0; i < m_params->size(); i++) {
                        m_params->param(i)->execute();
                        index.push_back(m_params->param(i)->getResult().getIntValue());
                    }
                    const Array &array = *object->array;
                    if (array.isValidIndex(index)) {
                        m_result = array[index];                
                    }
                    else {
                        m_result.setUndefined();
                    }
                }
                break;
            default:
                fprintf(stderr, "debug: unexpected symbol table entry type\n");
        }
    }
    else {
        fprintf(stderr, "warning: identifier `%s' is undefined\n", m_identifier.c_str());
        m_result.setUndefined();
    }
}

void basic::UnaryExpression::execute() const {
    m_term->execute();
    Variant intermediate = m_term->getResult();
    if (m_op == TkNOT) {
        m_result.setBoolValue(not intermediate.getBoolValue());
    }
    else if (m_op == TkMINUS) {
        switch(intermediate.getType()) {
            case basic::Variant::VaINT:
                m_result.setIntValue(0 - intermediate.getIntValue());
                break;
            case basic::Variant::VaDOUBLE:
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

void basic::MultiplicativeExpression::execute() const {
    m_first_term->execute();
    Variant intermediate = m_first_term->getResult();
    
    for (std::list<OperatorTermWrapper>::const_iterator term = m_other_terms.begin(); term != m_other_terms.end(); term++) {
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

void basic::AdditiveExpression::execute() const {
    m_first_term->execute();
    Variant intermediate = m_first_term->getResult();
    
    for (std::list<OperatorTermWrapper>::const_iterator term = m_other_terms.begin(); term != m_other_terms.end(); term++) {
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

void basic::ComparitiveExpression::execute() const {
    bool intermediate = false;
    m_first->execute();
    Variant v1 = m_first->getResult();
    m_second->execute();
    Variant v2 = m_second->getResult();
    switch (m_cmp) {
        case TkEQUALS:      intermediate = (v1 == v2); break;
        case TkNOTEQUALS:   intermediate = (v1 != v2); break;
        case TkLT:          intermediate = (v1 <  v2); break;
        case TkGT:          intermediate = (v1 >  v2); break;
        case TkLTEQUALS:    intermediate = (v1 <= v2); break;
        case TkGTEQUALS:    intermediate = (v1 >= v2); break;
        default: 
            fprintf(stderr, "debug: invalid operator `%s' in ComparitiveExpression object\n", Tokeniser::tokenDescriptions[m_cmp]);
    }
    m_result.setBoolValue(intermediate);
}

void basic::AndExpression::execute() const {
    bool intermediate = true;
    for (std::list<basic::Expression*>::const_iterator e = m_terms.begin(); e != m_terms.end(); e++) {
        (*e)->execute();
        intermediate = (intermediate and (*e)->getResult().getBoolValue());
    }
    m_result.setBoolValue(intermediate);
}

void basic::OrExpression::execute() const {
    bool intermediate = false;
    for (std::list<basic::Expression*>::const_iterator e = m_terms.begin(); e != m_terms.end(); e++) {
        (*e)->execute();
        intermediate = (intermediate and (*e)->getResult().getBoolValue());
    }
    m_result.setBoolValue(intermediate);
}

void basic::Block::execute() const {
    for (std::list<basic::Statement*>::const_iterator s = m_statements.begin(); s != m_statements.end(); s++) {
        (*s)->execute();
    }
}

void basic::PrintStatement::execute() const {
    for (std::list<basic::Expression*>::const_iterator e = m_expressions.begin(); e != m_expressions.end(); e++) {
        (*e)->execute();
        fputs((*e)->getResult().getStringValue(), stdout);
    }
    if (m_append_eol)  putchar('\n');
    fflush(stdout);
}

void basic::InputStatement::execute() const { // FIXME tidy this up
    const size_t bufsize = 1024;
    char *buffer = new char[bufsize];
    if (m_prompt) {
        m_prompt->execute();
        fputs(m_prompt->getResult().getStringValue(), stdout);
        fflush(stdout);
    }
    fgets(buffer, bufsize, stdin);
    int len = strlen(buffer);
    if (buffer[len - 1] == '\n')  buffer[--len] = '\0';
    Variant value(buffer);
    delete[] buffer;
    
    // FIXME convert value into "smallest" type that won't lose information
    // FIXME e.g.  0 | 1 => bool
    // FIXME       all digits, no decimal place => int
    // FIXME       digits and decimal place => double
    // FIXME       anything else => string
    
    SymbolTable::Entry *object = NULL;
    if (m_subscript) {
        // FIXME handle assigning to array subscripts
    }
    else {
        if ((object = g_symbol_table.find(m_identifier, SymbolTable::VARIANT))) {
            *object->variant = value;
        }
        else {
            g_symbol_table.defineVariant(m_identifier, new Variant(value));
        }
    }
}

void basic::LetStatement::execute() const {
    SymbolTable::Entry *object = NULL;
    m_expression->execute();

    if (m_subscript) {
        // FIXME handle assigning to array subscripts
    }
    else {
        if ((object = g_symbol_table.find(m_identifier, SymbolTable::VARIANT))) {
            *object->variant = m_expression->getResult();
        }
        else {
            g_symbol_table.defineVariant(m_identifier, new Variant(m_expression->getResult()));
        }
    }
}

void basic::CallStatement::execute() const {
    SymbolTable::Entry *object = NULL;
    if ((object = g_symbol_table.find(m_identifier, SymbolTable::BUILTIN_FUNCTION | SymbolTable::FUNCTION | SymbolTable::SUBROUTINE))) {
        switch (object->type) {
            case SymbolTable::BUILTIN_FUNCTION:
                object->builtin_function(m_params);
                break;
            case SymbolTable::FUNCTION:
                object->function->call(m_params);
                break;
            case SymbolTable::SUBROUTINE:
                object->sub->call(m_params);
                break;
            default:
                fprintf(stderr, "debug: unexpected object type in call for identifier `%s`\n", m_identifier.c_str());
        }
    }
    else {
        fprintf(stderr, "warning: undefined identifer `%s' in call\n", m_identifier.c_str());
    }
}

void basic::IfStatement::execute() const {
    for (std::list<ConditionalBlock>::const_iterator cb = m_conditional_blocks.begin(); cb != m_conditional_blocks.end(); cb++) {
        cb->condition->execute();
        if (cb->condition->getResult().getBoolValue() == true) {
            cb->block->execute();
            return;
        }
    }
    
    m_else_block->execute();
}

void basic::DoStatement::execute() const {
    g_symbol_table.startScope();
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
                    fprintf(stderr, "warning: got here (DoStatement->DcPRECONDITION)\n");
                    condition_reached = true;
            }
            if (condition_reached) break;
        }
        
        m_body->execute();
        
        if (m_condition_when == DcPOSTCONDITION) {
            switch (m_condition_type) {
                case DcWHILE:
                    m_condition->execute();
                    condition_reached = not m_condition->getResult().getBoolValue();
                    break;
                case DcUNTIL:
                    m_condition->execute();
                    condition_reached = m_condition->getResult().getBoolValue();
                    break;
                case DcFOREVER:
                    condition_reached = false;
                    break;
                case DcONCE:
                    condition_reached = true;
                    break;
                default:
                    fprintf(stderr, "warning: got here (DoStatment->DcPRECONDITION)\n");
                    condition_reached = true;
            }
        }
    }
    g_symbol_table.endScope();
}

void basic::ForStatement::execute() const {
    
}

void basic::DimStatement::execute() const {
    
}

void basic::AcceptedParamList::execute() const {
    
}

void basic::ParamList::execute() const {
    ;  // does nothing
}

void basic::ArraySubscript::execute() const {
    
}

void basic::ArrayDimension::execute() const {
    
}

void basic::FunctionDefinition::execute() const {
    
}

basic::Variant basic::FunctionDefinition::call(const ParamList *params) const {
    SymbolTable::Entry *object = NULL;
    if ((object = g_symbol_table.find(m_identifier, SymbolTable::FUNCTION))) {
        return object->function->call(params);
    }
    else {
        fprintf(stderr, "warning: function `%s' is not defined\n", m_identifier.c_str());
        return Variant();
    }
}

void basic::SubDefinition::execute() const {
    
}

void basic::SubDefinition::call(const ParamList *params) const {
    SymbolTable::Entry *object = NULL;
    if ((object = g_symbol_table.find(m_identifier, SymbolTable::SUBROUTINE))) {
        object->sub->call(params);
    }
    else {
        fprintf(stderr, "warning: subroutine `%s' is not defined\n", m_identifier.c_str());        
    }
}

#pragma mark Destructors

basic::PrintStatement::~PrintStatement() {
    for (std::list<basic::Expression *>::iterator e = m_expressions.begin(); e != m_expressions.end(); e++) {
        delete (*e);
    }
}

basic::IfStatement::~IfStatement() {
    for (std::list<ConditionalBlock>::iterator b = m_conditional_blocks.begin(); b != m_conditional_blocks.end(); b++) {
        delete b->condition;
        delete b->block;
    }
    
    if (m_else_block)  delete m_else_block;
}

basic::ForStatement::~ForStatement() {
    delete m_start;
    delete m_end;
    if (m_step)  delete m_step;
    delete m_body;
}

basic::DimStatement::~DimStatement() {
    for (std::list<Dimensionable>::iterator d = m_dimensionables.begin(); d != m_dimensionables.end(); d++) {
        if (d->second)  delete d->second;
    }
}

basic::ArraySubscript::~ArraySubscript() {
    for (std::list<basic::Expression *>::iterator e = m_expressions.begin(); e != m_expressions.end(); e++) {
        delete (*e);
    }
}

basic::ArrayDimension::~ArrayDimension() {
    for (std::list<ArrayDimension::Specification>::iterator d = m_dimensions.begin(); d != m_dimensions.end(); d++) {
        delete d->first;
        if (d->second)  delete d->second;
    }
}

basic::AcceptedParamList::~AcceptedParamList() {
    // FIXME
}

basic::ParamList::~ParamList() {
    for (std::vector<Expression *>::iterator e = m_expressions.begin(); e != m_expressions.end(); e++) {
        delete (*e);
    }
}

basic::Block::~Block() {
    for (std::list<basic::Statement *>::iterator s = m_statements.begin(); s != m_statements.end(); s++) {
        delete (*s);
    }
}

basic::MultiplicativeExpression::~MultiplicativeExpression() {
    delete m_first_term;
    for (std::list<OperatorTermWrapper>::iterator e = m_other_terms.begin(); e != m_other_terms.end(); e++) {
        delete (e->term);
    }
}

basic::AdditiveExpression::~AdditiveExpression() {
    delete m_first_term;
    for (std::list<OperatorTermWrapper>::iterator e = m_other_terms.begin(); e != m_other_terms.end(); e++) {
        delete (e->term);
    }
}


basic::AndExpression::~AndExpression() {
    for (std::list<basic::Expression *>::iterator e = m_terms.begin(); e != m_terms.end(); e++) {
        delete (*e);
    }
}

basic::OrExpression::~OrExpression() {
    for (std::list<basic::Expression *>::iterator e = m_terms.begin(); e != m_terms.end(); e++) {
        delete (*e);
    }    
}
