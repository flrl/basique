/*
 *  symboltable.h
 *  Untitled
 *
 *  Created by Daniel on 27/08/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef BASIQUE_SYMBOLTABLE_H
#define BASIQUE_SYMBOLTABLE_H

#include <map>
#include <vector>
#include <utility>

#include <stdint.h>

#include "array.h"
#include "ast.h"
#include "builtin.h"
#include "file.h"
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
        FILEHANDLE       = 32,
        // etc        
    };
    struct Entry {
        Entry(EntryType type, builtin::function *builtin_function) : type(type), builtin_function(builtin_function) {}
        Entry(EntryType type, const FunctionDefinition *function) : type(type), function(function) {}
        Entry(EntryType type, const SubDefinition *sub) : type(type), sub(sub) {}
        Entry(EntryType type, Variant *variant) : type(type), variant(variant) {}
        Entry(EntryType type, Array *array) : type(type), array(array) {}
        Entry(EntryType type, File *file) : type(type), file(file) {}

        EntryType type;
        union { 
            builtin::function *builtin_function;
            const FunctionDefinition *function;
            const SubDefinition *sub;
            Variant *variant;
            Array *array;
            File *file;
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

    // These return NULL on success, or the second parameter on failure.
    builtin::function *defineBuiltinFunction(const String &, builtin::function *);
    const FunctionDefinition *defineFunction(const String &, const FunctionDefinition *);
    const SubDefinition *defineSubroutine(const String &, const SubDefinition *);
    Variant *defineVariant(const String &, Variant *);
    /*FIXME const*/ Array *defineArray(const String &, Array *);
    File *defineFile(const String &, File *);
    
    bool undefine(const String &, unsigned int mask=0);
    
private:
    static const unsigned int all_entry_types = BUILTIN_FUNCTION | FUNCTION | SUBROUTINE | VARIANT | ARRAY | FILEHANDLE;
    std::vector<Frame> m_frames;
    
    void installBuiltins();
    const Entry *find_helper(const String &, unsigned int=all_entry_types) const;
};

extern basic::SymbolTable g_symbol_table;

#endif
