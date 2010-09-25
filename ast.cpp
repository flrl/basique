/*
 *  ast.cpp
 *  Untitled
 *
 *  Created by Ellie on 16/01/10.
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
                if (m_params) {
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
                else {
                    fprintf(stderr, "warning: array `%s' used without subscript in expression at line %i, column %i\n",
                            m_identifier.c_str(), m_line, m_column);
                    m_result.setUndefined();
                }
                break;
            case SymbolTable::FILEHANDLE:
                fprintf(stderr, "warning: attempting to use file handle `%s' in expression at line %i, column %i\n",
                        m_identifier.c_str(), m_line, m_column);
                m_result.setUndefined();
                break;
            default:
                fprintf(stderr, "debug: unexpected symbol table entry type in identifier expression\n");
                m_result.setUndefined();
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
    for (std::list<basic::Expression*>::const_iterator e = m_terms.begin(); e != m_terms.end(); e++) {
        (*e)->execute();
        if ((*e)->getResult().getBoolValue() == false) {
            m_result.setBoolValue(false);
            return;
        }
    }
    m_result.setBoolValue(true);
}

void basic::OrExpression::execute() const {
    for (std::list<basic::Expression*>::const_iterator e = m_terms.begin(); e != m_terms.end(); e++) {
        (*e)->execute();
        if ((*e)->getResult().getBoolValue() == true) {
            m_result.setBoolValue(true);
            return;
        }
    }
    m_result.setBoolValue(false);
}

void basic::Block::execute() const {
    for (std::list<basic::Statement*>::const_iterator s = m_statements.begin(); s != m_statements.end(); s++) {
        (*s)->execute();
    }
}

void basic::PrintStatement::execute() const {
    FILE *output = stdout;
    
    if (m_file_identifier) {
        SymbolTable::Entry *object = NULL;
        if ((object = g_symbol_table.find(m_file_identifier, SymbolTable::FILEHANDLE))) {
            output = object->file->c_file();
        }
        else {
            fprintf(stderr, "error: identifier `%s' does not name a file handle in print at line %i, column %i\n", 
                    m_file_identifier.c_str(), m_line, m_column);
            return;
        }
    }
    
    for (std::list<basic::Expression*>::const_iterator e = m_expressions.begin(); e != m_expressions.end(); e++) {
        (*e)->execute();
        fputs((*e)->getResult().getStringValue(), output);
    }
    if (m_append_eol)  putc('\n', output);
    fflush(output);
}

void basic::InputStatement::execute() const {
    FILE *input = stdin;
    Variant *destination = NULL;

    // hook up the input file handle if there is one
    if (m_file_identifier) {
        SymbolTable::Entry *object = NULL;
        if ((object = g_symbol_table.find(m_file_identifier, SymbolTable::FILEHANDLE))) {
            if (object->file->isValid()) {
                input = object->file->c_file();                
            }
            else {
                fprintf(stderr, "debug: got an invalid file handle identifer in input statement\n");
                return;
            }
        }
        else {
            fprintf(stderr, "error: identifier `%s' does not name a file handle in print at line %i, column %i\n", 
                    m_file_identifier.c_str(), m_line, m_column);
            return;
        }
    }
    
    // hook up the destination in the symbol table
    SymbolTable::Entry *object = NULL;
    if (m_subscript) {
        if ((object = g_symbol_table.find(m_identifier, SymbolTable::ARRAY))) {
            Array::Index index;
            m_subscript->makeArrayIndex(&index);
            if (object->array->isValidIndex(index)) {
                destination = &((*object->array)[index]);
            }
            else {
                fprintf(stderr, "error: array index out of bounds at line %i, column %i\n", m_line, m_column);
                return;
            }
        }
        else {
            fprintf(stderr, "error: no such array `%s' at line %i, column %i\n", m_identifier.c_str(), m_line, m_column);
            return;
        }
    }
    else {
        if ((object = g_symbol_table.find(m_identifier, SymbolTable::VARIANT))) {
            destination = object->variant;
        }
        else {
            destination = new Variant();
            Variant *r = g_symbol_table.defineVariant(m_identifier, destination);
            if (r) {
                delete destination;
                return;
            } 
        }
    }
    assert(destination != NULL);
    
    // don't try to read if we're at the end of the file
    if (feof(input)) {
        destination->setUndefined();
        return;
    }
    
    // display a prompt if we have one and the input source is stdin // FIXME only if it's a tty
    if (m_prompt and input == stdin) {
        m_prompt->execute();
        fputs(m_prompt->getResult().getStringValue(), stdout);
        fflush(stdout);
    }
    
    // read some input
    const size_t bufsize = 1024;
    char *buffer = new char[bufsize];
    assert(buffer != NULL);
    if (fgets(buffer, bufsize, input) == NULL) {
        destination->setUndefined();
        delete[] buffer;
        return;
    }
    int len = strlen(buffer);
    if (buffer[len - 1] == '\n')  buffer[--len] = '\0';

    // convert value to "smallest" type that can contain it
    char *endptr;
    int int_value;
    double double_value;
    int_value = strtol(buffer, &endptr, 0);
    if (buffer[0] != '\0' and endptr[0] == '\0') {
        destination->setIntValue(int_value);
    }
    else {
        double_value = strtod(buffer, &endptr);
        if (buffer[0] != '\0' and endptr[0] == '\0') {
            destination->setDoubleValue(double_value);
        }
        else {
            destination->setStringValue(buffer);
        }
    }
    
    // clean up
    delete[] buffer;
}

void basic::LetStatement::execute() const {
    SymbolTable::Entry *object = NULL;
    m_expression->execute();

    if (m_subscript) {
        if ((object = g_symbol_table.find(m_identifier, SymbolTable::ARRAY))) {
            Array::Index index;
            m_subscript->makeArrayIndex(&index);
            if (object->array->isValidIndex(index)) {
                (*object->array)[index] = m_expression->getResult();
            }
            else {
                fprintf(stderr, "error: array index out of bounds at line %i, column %i\n", m_line, m_column);
            }
        }
        else {
            fprintf(stderr, "error: no such array `%s' at line %i, column %i\n", m_identifier.c_str(), m_line, m_column);
        }
    }
    else {
        if ((object = g_symbol_table.find(m_identifier, SymbolTable::VARIANT))) {
            *object->variant = m_expression->getResult();
        }
        else {
            Variant *binding = g_symbol_table.defineVariant(m_identifier, new Variant(m_expression->getResult()));
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
    if (g_symbol_table.defineVariant(m_identifier, i) == NULL) {        
        // sanity check to ensure that loop eventually finishes
        if ((start <= end and step > 0) or (start >= end and step < 0)) {
            // execute loop
            do {
                m_body->execute();
                i->setIntValue(i->getIntValue() + step);
            } while ((step > 0 and i->getIntValue() <= end) or (step < 0 and i->getIntValue() >= end));
        }
    }
    else {
        delete i;
    }
    
    g_symbol_table.endScope();
}

void basic::DimStatement::execute() const {
    for (std::list<Dimensionable>::const_iterator d = m_dimensionables.begin(); d != m_dimensionables.end(); d++) {
        if (d->second == NULL) {
            // it's a variant
            Variant *binding = g_symbol_table.defineVariant(d->first, new Variant());
            if (binding)  delete binding;
        }
        else {
            // it's an array
            std::vector<Array::DimensionSpecification> dimensions;
            d->second->makeArrayDimensionSpecificationVector(&dimensions);
            Array *binding = g_symbol_table.defineArray(d->first, new Array(dimensions));
            if (binding)  delete binding;
        }
    }
}

void basic::OpenStatement::execute() const {
    File::Mode mode;
    
    switch (m_mode) {
        case TkINPUT:   mode = File::INPUT;     break;
        case TkOUTPUT:  mode = File::OUTPUT;    break;
        case TkAPPEND:  mode = File::APPEND;    break;
        default:
            fprintf(stderr, "debug: invalid file mode `%s' in open statement\n", Tokeniser::tokenDescriptions[m_mode]);
            return;
    }
    
    m_filename->execute();
    String filename = m_filename->getResult().getStringValue();

    File *binding = g_symbol_table.defineFile(m_identifier, new File(filename, mode));
    if (binding)  delete binding;
}

void basic::CloseStatement::execute() const {
    if (not g_symbol_table.undefine(m_identifier, SymbolTable::FILEHANDLE)) {
        fprintf(stderr, "warning: attempt to close undefined file handle `%s'\n", m_identifier.c_str());
    }
}

void basic::AcceptedParamList::execute() const {
    ;   // does nothing
}

void basic::ParamList::execute() const {
    ;   // does nothing
}

void basic::ArraySubscript::execute() const {
    ;   // does nothing
}

void basic::ArrayDimension::execute() const {
    ;   // does nothing
}

void basic::FunctionDefinition::execute() const {
    if (g_symbol_table.defineFunction(m_identifier, this) == NULL) {
        m_installed = true;        
    }
}

void basic::SubDefinition::execute() const {
    if (g_symbol_table.defineSubroutine(m_identifier, this) == NULL) {
        m_installed = true;
    }
}


#pragma mark call() methods

bool basic::AcceptedParamList::call(const ParamList *params) const {
    if (m_identifiers.size() == params->size()) {
        for (unsigned i = 0; i < m_identifiers.size(); i++) {
            Variant *p = g_symbol_table.defineVariant(m_identifiers[i], new Variant(params->evaluate(i)));
            if (p) {
                delete p;
                return false;
            }
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
    
    Variant *function_return_value = new Variant();
    Variant *status = g_symbol_table.defineVariant(m_identifier, function_return_value);
    if (status == NULL) {
        m_body->execute();
        Variant value = *function_return_value;
        g_symbol_table.endScope();  // deletes function_return_value
        return value;
    }
    else {
        delete function_return_value;
        return Variant();
    }
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

#pragma mark Other stuff

void basic::ArraySubscript::makeArrayIndex(Array::Index *ptr) const {
    ptr->clear();
    ptr->reserve(m_expressions.size());
    for (std::list<Expression *>::const_iterator e = m_expressions.begin(); e != m_expressions.end(); e++) {
        (*e)->execute();
        ptr->push_back((*e)->getResult().getIntValue());
    }
}

void basic::ArrayDimension::makeArrayDimensionSpecificationVector(std::vector<Array::DimensionSpecification> *ptr) const {
    ptr->clear();
    ptr->reserve(m_dimensions.size());
    for (std::list<ArrayDimension::Specification>::const_iterator s = m_dimensions.begin(); s != m_dimensions.end(); s++) {
        int first, last;
        if (s->second == NULL) {
            s->first->execute();
            first = 0;
            last = s->first->getResult().getIntValue() - 1;
        }
        else {
            s->first->execute();
            s->second->execute();
            first = s->first->getResult().getIntValue();
            last = s->second->getResult().getIntValue();
        }
        ptr->push_back(std::make_pair(first, last));
    }    
}

