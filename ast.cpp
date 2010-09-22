/*
 *  ast.cpp
 *  Untitled
 *
 *  Created by Daniel on 16/01/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include <cassert>
#include <cstring>

#include "ast.h"
#include "symboltable.h"

#pragma mark execute() methods

void basic::LiteralExpression::execute() const {
    ; // m_result is set at initialisation
}

void basic::IdentifierExpression::execute() const {
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

void basic::InputStatement::execute() const {
    const size_t bufsize = 1024;
    Variant value;
    
    char *buffer = new char[bufsize];
    assert(buffer != NULL);
    
    // FIXME define/validate the destination variable before prompting for input
    
    // display a prompt if we have one
    if (m_prompt) {
        m_prompt->execute();
        fputs(m_prompt->getResult().getStringValue(), stdout);
        fflush(stdout);
    }
    
    // read some input
    fgets(buffer, bufsize, stdin);
    int len = strlen(buffer);
    if (buffer[len - 1] == '\n')  buffer[--len] = '\0';

    // convert value to "smallest" type that can contain it
    char *endptr;
    int int_value;
    double double_value;
    int_value = strtol(buffer, &endptr, 0);
    if (buffer[0] != '\0' and endptr[0] == '\0') {
        value.setIntValue(int_value);
    }
    else {
        double_value = strtod(buffer, &endptr);
        if (buffer[0] != '\0' and endptr[0] == '\0') {
            value.setDoubleValue(double_value);
        }
        else {
            value.setStringValue(buffer);
        }
    }
    delete[] buffer;
    
    // insert it into the symbol table
    SymbolTable::Entry *object = NULL;
    if (m_subscript) {
        // FIXME handle assigning to array subscripts
    }
    else {
        if ((object = g_symbol_table.find(m_identifier, SymbolTable::VARIANT))) {
            *object->variant = value;
        }
        else {
            Variant *binding = new Variant(value);
            binding = g_symbol_table.defineVariant(m_identifier, binding);
            if (binding)  delete binding;
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
            Variant *binding = new Variant(m_expression->getResult());
            binding = g_symbol_table.defineVariant(m_identifier, binding);
            if (binding)  delete binding;
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
    
    if (m_else_block)  m_else_block->execute();
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
    g_symbol_table.startScope();
    
    // set initial state
    m_start->execute();
    int start = m_start->getResult().getIntValue();
    m_end->execute();
    int end = m_end->getResult().getIntValue();
    int step = 1;
    if (m_step) {
        m_step->execute();
        step = m_step->getResult().getIntValue();
    }

    // set up iterator -- will be deleted by symbol table at the end of the scope
    Variant *i = new Variant(start);
    g_symbol_table.defineVariant(m_identifier, i);
    
    // sanity check to ensure that loop eventually finishes
    if ((start <= end and step > 0) or (start >= end and step < 0)) {
        // execute loop
        do {
            m_body->execute();
            i->setIntValue(i->getIntValue() + step);
        } while ((step > 0 and i->getIntValue() <= end) or (step < 0 and i->getIntValue() >= end));
    }
    
    g_symbol_table.endScope();
}

void basic::DimStatement::execute() const {
    
}

void basic::AcceptedParamList::execute() const {
    ;   // does nothing
}

void basic::ParamList::execute() const {
    ;   // does nothing
}

void basic::ArraySubscript::execute() const {
    
}

void basic::ArrayDimension::execute() const {
    
}

void basic::FunctionDefinition::execute() const {
    // FIXME what happens when the function is already defined? where should this case be detected and handled?
    g_symbol_table.defineFunction(m_identifier, this);
    m_installed = true;
}

void basic::SubDefinition::execute() const {
    // FIXME what happens when the function is already defined? where should this case be detected and handled?
    g_symbol_table.defineSubroutine(m_identifier, this);
    m_installed = true;
}


#pragma mark call() methods

bool basic::AcceptedParamList::call(const ParamList *params) const {
    if (m_identifiers.size() == params->size()) {
        for (unsigned i = 0; i < m_identifiers.size(); i++) {
            Variant *p = new Variant(params->evaluate(i));
            g_symbol_table.defineVariant(m_identifiers[i], p);
        }
        return true;
    }
    else {
        fprintf(stderr, "warning: wrong number of parameters: expected %zu, got %zu\n", m_identifiers.size(), params->size());
        return false;
    }
}

basic::Variant basic::FunctionDefinition::call(const ParamList *params) const {
    g_symbol_table.startScope();
    m_accepted_params->call(params);
    g_symbol_table.defineVariant(m_identifier, new Variant());
    m_body->execute();
    Variant result = *g_symbol_table.find(m_identifier, SymbolTable::VARIANT)->variant;
    g_symbol_table.endScope();
    
    return result;
}

void basic::SubDefinition::call(const ParamList *params) const {
    g_symbol_table.startScope();
    m_accepted_params->call(params);
    m_body->execute();
    g_symbol_table.endScope();
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
