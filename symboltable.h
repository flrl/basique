/*
 *  symboltable.h
 *  Untitled
 *
 *  Created by Ellie on 27/08/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _SYMBOLTABLE_H
#define _SYMBOLTABLE_H

#include <map>
#include <vector>
#include <utility>

#include <stdint.h>

#include "array.h"
#include "ast.h"
#include "string.h"
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

typedef std::pair<SymbolTableEntryType::Enum, void *> SymbolTableEntry;
typedef std::map<const String, SymbolTableEntry> SymbolTableEntryMap;

class SymbolTable {
public:
    SymbolTable();
    ~SymbolTable();
    
    void push_frame();
    void pop_frame();
    
    SymbolTableEntry lookup_symbol(const char *) const;
    bool defined(const char *) const;

    void create_symbol(const char *, SymbolTableEntryType::Enum, void *);

    void define_function(const String &, Basic::FunctionDefinition *);
    void define_subroutine(const String &, Basic::SubDefinition *);
    void define_variant(const String &, Variant *);
    void define_array(const String &, Array *);
    
private:
    std::vector<SymbolTableEntryMap> frames;
    
};



#endif
