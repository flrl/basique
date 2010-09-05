/*
 *  symboltable.h
 *  Untitled
 *
 *  Created by Daniel on 27/08/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _SYMBOLTABLE_H
#define _SYMBOLTABLE_H

#include <map>
#include <vector>
#include <utility>

#include <stdint.h>

#include "ast.h"
#include "immutablestring.h"
#include "variant.h"

namespace SymbolTableEntryType {
    enum Enum {
        UNDEFINED,
        BUILTIN_FUNCTION,
        FUNCTION,
        SUBROUTINE,
        VARIANT,
        VARIANT_ARRAY,
        // etc
    };
}

typedef std::pair<SymbolTableEntryType::Enum, intptr_t> SymbolTableEntry;
typedef std::map<ImmutableString, SymbolTableEntry> SymbolTableEntryMap;

class SymbolTable {
public:
    SymbolTable();
    ~SymbolTable();
    
    void push_frame();
    void pop_frame();
    
    void create_symbol(const char *, SymbolTableEntryType::Enum, intptr_t);
    SymbolTableEntry lookup_symbol(const char *);
    bool defined(const char *) const;
    
private:
    std::vector<SymbolTableEntryMap> frames;
    
};



#endif
