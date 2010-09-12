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

namespace Basic {
    class BuiltinFunction;
    
    class BF_left;
    class BF_right;
    class BF_mid;
    class BF_len;
};

class Basic::BuiltinFunction {
public:
    virtual ~BuiltinFunction() { }
    virtual void call(const ParamList *) = 0;
    Variant getResult() { return m_result; }
protected:
    Variant m_result;
};


class Basic::BF_left : public BuiltinFunction {
public:
    void call(const ParamList *);
};

class Basic::BF_right : public BuiltinFunction {
public:
    void call(const ParamList *);
};

class Basic::BF_mid : public BuiltinFunction {
public:
    void call(const ParamList *);
};

class Basic::BF_len : public BuiltinFunction {
public:
    void call(const ParamList *);
};


#endif
