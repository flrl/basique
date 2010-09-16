/*
 *  builtin.h
 *  Untitled
 *
 *  Created by Daniel on 11/09/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _BUILTIN_H
#define _BUILTIN_H

#include "ast.h"
#include "variant.h"

namespace Basic {
    namespace builtin {
        typedef Variant(function)(const ParamList *);
        
        Variant left(const ParamList *);
        Variant right(const ParamList *);
        Variant mid(const ParamList *);
        Variant len(const ParamList *);
    }
}

#endif
