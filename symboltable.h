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

namespace basic {
    class SymbolTable;
}

class basic::SymbolTable {
public:
    enum EntryType {
        BUILTIN_FUNCTION =  1,
        FUNCTION         =  2,
        SUBROUTINE       =  4,
        VARIANT          =  8,
        ARRAY            = 16,
        // etc        
    };
    struct Entry {
        Entry(EntryType type, builtin::function *builtin_function) : type(type), builtin_function(builtin_function) {}
        Entry(EntryType type, FunctionDefinition *function) : type(type), function(function) {}
        Entry(EntryType type, SubDefinition *sub) : type(type), sub(sub) {}
        Entry(EntryType type, Variant *variant) : type(type), variant(variant) {}
        Entry(EntryType type, Array *array) : type(type), array(array) {}

        EntryType type;
        union { 
            builtin::function *builtin_function;
            FunctionDefinition *function;
            SubDefinition *sub;
            Variant *variant;
            Array *array;
        };
    };
    typedef std::map<const String, Entry> Frame;
    
    SymbolTable();
    ~SymbolTable();
    
    void startScope();
    void endScope();
    
    const Entry *find(const String &identifier, unsigned int mask=all_entry_types) const { return find_helper(identifier, mask); }
    Entry *find(const String &identifier, unsigned int mask=all_entry_types) { return const_cast<Entry*>(find_helper(identifier, mask)); }

    bool defined(const String&) const;

    void defineBuiltinFunction(const String &, builtin::function *);
    void defineFunction(const String &, FunctionDefinition *);
    void defineSubroutine(const String &, SubDefinition *);
    void defineVariant(const String &, Variant *);
    void defineArray(const String &, Array *);
    
private:
    static const unsigned int all_entry_types = BUILTIN_FUNCTION | FUNCTION | SUBROUTINE | VARIANT | ARRAY;
    std::vector<Frame> m_frames;
    
    void installBuiltins();
    const Entry *find_helper(const String &, unsigned int=all_entry_types) const;
};

extern basic::SymbolTable g_symbol_table;

#endif
