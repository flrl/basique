/*
 *  builtin.cpp
 *  Untitled
 *
 *  Created by Daniel on 11/09/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "builtin.h"

// str left(str, len)
void BF_left::call(Basic::ParamList *params) {
    this->_result.setDefaultValueForType(Basic::Variant::is_undef);
    if (params->size() != 2) return;

    Basic::Expression *e_str = params->param(0);
    e_str->execute();
    String str(e_str->getResult().getStringValue());
    
    Basic::Expression *e_len = params->param(1);
    e_len->execute();
    int len = e_len->getResult().getIntValue();
    if (len < 1) return;

    char *buffer = new char[1 + len];
    strncpy(buffer, str, len);
    _result.setStringValue(buffer);
    delete[] buffer;
}

