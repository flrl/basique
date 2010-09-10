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

#include "array.h"
#include "ast.h"
#include "string.h"
#include "variant.h"

class SymbolTable {
public:
    enum EntryType {
        UNDEFINED,
        BUILTIN_FUNCTION,
        FUNCTION,
        SUBROUTINE,
        VARIANT,
        VARIANT_ARRAY,
        // etc        
    };
    typedef std::pair<EntryType, void *> Entry;
    typedef std::map<const String, Entry> Frame;
    
    SymbolTable();
    ~SymbolTable();
    
    void push_frame();
    void pop_frame();
    
    const Entry *find(const String &) const;
    Entry *find(const String &identifier) { return const_cast<Entry*>(find(identifier)); }

    bool defined(const String&) const;

    void define_function(const String &, Basic::FunctionDefinition *);
    void define_subroutine(const String &, Basic::SubDefinition *);
    void define_variant(const String &, Basic::Variant *);
    void define_array(const String &, Basic::Array *);
    
private:
    std::vector<Frame> frames;
    
};



#endif
