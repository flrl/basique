/*
 *  symboltable.cpp
 *  Untitled
 *
 *  Created by Daniel on 27/08/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "symboltable.h"

const SymbolTable::Entry *SymbolTable::find(const String &identifier) const {
    for (std::vector<Frame>::const_reverse_iterator frame = frames.rbegin(); frame != frames.rend(); frame++) {
        Frame::const_iterator result = frame->find(identifier);
        if (result != frame->end())  return &result->second;  // iterator of map derefs to pair<key_t, value_t>
    }

    return NULL;
}

bool SymbolTable::defined(const String &identifier) const {
    for (std::vector<Frame>::const_reverse_iterator frame = frames.rbegin(); frame != frames.rend(); frame++) {
        if (frame->count(identifier) > 0)  return true;
    }
    return false;
}

void SymbolTable::define_function(const String &identifier, Basic::FunctionDefinition *binding) {
    std::vector<Frame>::reverse_iterator frame = frames.rbegin();
    frame->insert(std::make_pair(identifier, std::make_pair(SymbolTable::FUNCTION, binding)));
}

void SymbolTable::define_subroutine(const String &identifier, Basic::SubDefinition *binding) {
    std::vector<Frame>::reverse_iterator frame = frames.rbegin();
    frame->insert(std::make_pair(identifier, std::make_pair(SymbolTable::SUBROUTINE, binding)));
}

void SymbolTable::define_variant(const String &identifier, Basic::Variant *binding) {
    std::vector<Frame>::reverse_iterator frame = frames.rbegin();
    frame->insert(std::make_pair(identifier, std::make_pair(SymbolTable::VARIANT, binding)));
}

void SymbolTable::define_array(const String &identifier, Basic::Array *binding) {
    std::vector<Frame>::reverse_iterator frame = frames.rbegin();
    frame->insert(std::make_pair(identifier, std::make_pair(SymbolTable::VARIANT_ARRAY, binding)));
}
