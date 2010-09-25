/*
 *  builtin.cpp
 *  Untitled
 *
 *  Created by Daniel on 11/09/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include <cstring>

#include "builtin.h"

// str left(str, len)
basic::Variant basic::builtin::left(const ParamList *params) {
    Variant result = Variant();
    if (params->size() != 2)  return result;

    String str(params->evaluate(0).getStringValue());
    
    int len = params->evaluate(1).getIntValue();
    if (len < 1)  return result;

    char *buffer = new char[1 + len];
    memset(buffer, 0, 1 + len);
    strncpy(buffer, str, len);
    result.setStringValue(buffer);
    delete[] buffer;

    return result;
}

// str right(str, len)
basic::Variant basic::builtin::right(const ParamList *params) {
    Variant result = Variant();
    if (params->size() != 2)  return result;
    
    String str(params->evaluate(0).getStringValue());
    
    int len = params->evaluate(1).getIntValue();
    if (len < 1)  return result;
    
    const char *p = str;
    p += str.length() - len;
    result.setStringValue(p);
    
    return result;
}

// str mid(str, start, len)
basic::Variant basic::builtin::mid(const ParamList *params) {
    Variant result = Variant();
    if (params->size() != 3)  return result;
    
    String str(params->evaluate(0).getStringValue());
    
    int start = params->evaluate(1).getIntValue();
    if (start < 0)  return result;
    
    int len = params->evaluate(2).getIntValue();
    if (len < 1)  return result;
    
    const char *p = str;
    p += start;
    char *buffer = new char[1 + len];
    memset(buffer, 0, 1 + len);
    strncpy(buffer, p, len);
    result.setStringValue(buffer);
    delete[] buffer;
    
    return result;
}

// int len(str)
basic::Variant basic::builtin::len(const ParamList *params) {
    Variant result = Variant();
    if (params->size() != 1)  return result;
    
    String str(params->evaluate(0).getStringValue());
    
    result.setIntValue(str.length());
    
    return result;
}

// str type(expression)
basic::Variant basic::builtin::type(const ParamList *params) {
    if (params->size() != 1)  return Variant();
    
    switch (params->evaluate(0).getType()) {
        case Variant::VaBOOL:
            return Variant("bool");
        case Variant::VaINT:
            return Variant("integer");
        case Variant::VaDOUBLE:
            return Variant("double");
        case Variant::VaSTRING:
            return Variant("string");
        case Variant::VaUNDEF:
            return Variant("undefined");
        default:
            fprintf(stderr, "debug: unhandled variant type in builtin::type\n");
            return Variant("undefined");
    }
}
