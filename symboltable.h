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
#include "builtin.h"
#include "string.h"
#include "variant.h"

namespace Basic {
    class SymbolTable;
}

class Basic::SymbolTable {
public:
    enum EntryType {
        BUILTIN_FUNCTION,
        FUNCTION,
        SUBROUTINE,
        VARIANT,
        ARRAY,
        // etc        
    };
    struct Entry {
        Entry(EntryType type, void *binding) : type(type), binding(binding) { }
        EntryType type;
        void *binding;
    };
    typedef std::map<const String, Entry> Frame;
    
    SymbolTable();
    ~SymbolTable();
    
    void startScope();
    void endScope();
    
    const Entry *find(const String &) const;
    Entry *find(const String &identifier) { return const_cast<Entry*>(find(identifier)); }

    bool defined(const String&) const;

    void defineBuiltinFunction(const String &, BuiltinFunction *);
    void defineFunction(const String &, FunctionDefinition *);
    void defineSubroutine(const String &, SubDefinition *);
    void defineVariant(const String &, Variant *);
    void defineArray(const String &, Array *);
    
private:
    std::vector<Frame> m_frames;
    
};



#endif
