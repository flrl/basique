/*
 *  symboltable.cpp
 *  Untitled
 *
 *  Created by Daniel on 27/08/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "symboltable.h"

SymbolTableEntry SymbolTable::lookup_symbol(const char *identifier) {
    for (std::vector<SymbolTableEntryMap>::reverse_iterator frame = frames.rbegin(); frame != frames.rend(); frame++) {
        SymbolTableEntryMap::iterator result = frame->find(ImmutableString(identifier));
        if (result != frame->end())  return result->second;  // iterator of map derefs to pair<key_t, value_t>
    }

    return std::make_pair(SymbolTableEntryType::UNDEFINED, 0);
}

void SymbolTable::create_symbol(const char *identifier, SymbolTableEntryType::Enum ste_type, intptr_t ste_value) {
    std::vector<SymbolTableEntryMap>::reverse_iterator frame = frames.rbegin();
    frame->insert(std::make_pair(ImmutableString(identifier), std::make_pair(ste_type, ste_value)));
}

bool SymbolTable::defined(const char *identifier) const {
    for (std::vector<SymbolTableEntryMap>::const_reverse_iterator frame = frames.rbegin(); frame != frames.rend(); frame++) {
        if (frame->count(ImmutableString(identifier)) > 0)  return true;
    }
    return false;
}
