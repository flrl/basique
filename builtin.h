/*
 *  builtin.h
 *  Untitled
 *
 *  Created by Ellie on 11/09/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _BUILTIN_H
#define _BUILTIN_H

#include "ast.h"

// FIXME should be in namespace Basic

class BuiltinFunction {
public:
    virtual ~BuiltinFunction() { }
    virtual void call(Basic::ParamList *) = 0;
    Basic::Variant getResult() { return _result; }
protected:
    Basic::Variant _result;
};


class BF_left : public BuiltinFunction {
public:
    ~BF_left() { }
    void call(Basic::ParamList *);
};

class BF_right : public BuiltinFunction {
public:
    ~BF_right() { }
    void call(Basic::ParamList *);
};

class BF_mid : public BuiltinFunction {
public:
    ~BF_mid() { }
    void call(Basic::ParamList *);
};

class BF_len : public BuiltinFunction {
public:
    ~BF_len() { }
    void call(Basic::ParamList *);
};


#endif
