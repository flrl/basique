/*
 *  builtin.h
 *  Untitled
 *
 *  Created by Ellie on 11/09/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef BASIQUE_BUILTIN_H
#define BASIQUE_BUILTIN_H

#include "ast.h"
#include "variant.h"

namespace basic {
    namespace builtin {
        typedef Variant(function)(const ParamList *);
        
        Variant left(const ParamList *);
        Variant right(const ParamList *);
        Variant mid(const ParamList *);
        Variant len(const ParamList *);
        Variant type(const ParamList *);
    }
}

#endif
