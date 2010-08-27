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

#include <stdint.h>

#include "variant.h"

namespace SymbolTableEntryType {
    enum Enum {
        VARIANT,
        FUNCTION,
        SUB,
        // etc
    };
}

struct SymbolTableEntry {
    SymbolTableEntryType::Enum ste_type;
    intptr_t ste_value;
};

// FIXME should a symbol table entry simply be a variant, where a variant may also store subs and functions?
// FIXME -- though that would cause all kinds of complications in terms of syntax, arithmetic, etc

class SymbolTable {
public:
    SymbolTable();
    ~SymbolTable();
    
private:
    
};



#endif
