/*
 *  builtin.cpp
 *  Untitled
 *
 *  Created by Ellie on 11/09/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "builtin.h"

// str left(str, len)
void BF_left::call(Basic::ParamList *params) {
    _result.setStringValue("");
    if (params->size() != 2)  return;

    Basic::Expression *e_str = params->param(0);
    e_str->execute();
    String str(e_str->getResult().getStringValue());
    
    Basic::Expression *e_len = params->param(1);
    e_len->execute();
    int len = e_len->getResult().getIntValue();
    if (len < 1)  return;

    char *buffer = new char[1 + len];
    strncpy(buffer, str, len);
    _result.setStringValue(buffer);
    delete[] buffer;
}

// str right(str, len)
void BF_right::call(Basic::ParamList *params) {
    _result.setStringValue("");
    if (params->size() != 2)  return;
    
    Basic::Expression *e_str = params->param(0);
    e_str->execute();
    String str(e_str->getResult().getStringValue());
    
    Basic::Expression *e_len = params->param(1);
    e_len->execute();
    int len = e_len->getResult().getIntValue();
    if (len < 1)  return;
    
    const char *p = str;
    p += str.length() - len;
    _result.setStringValue(p);    
}

// str mid(str, start, len)
void BF_mid::call(Basic::ParamList *params) {
    _result.setStringValue("");
    if (params->size() != 3)  return;
    
    Basic::Expression *e_str = params->param(0);
    e_str->execute();
    String str(e_str->getResult().getStringValue());
    
    Basic::Expression *e_start = params->param(1);
    e_start->execute();
    int start = e_start->getResult().getIntValue();
    if (start < 0)  return;
    
    Basic::Expression *e_len = params->param(2);
    e_len->execute();
    int len = e_len->getResult().getIntValue();
    if (len < 1)  return;
    
    const char *p = str;
    p += start;
    char *buffer = new char[1 + len];
    strncpy(buffer, p, len);
    _result.setStringValue(buffer);
    delete[] buffer;
}

// int len(str)
void BF_len::call(Basic::ParamList *params) {
    this->_result.setIntValue(0);
    if (params->size() != 1)  return;
    
    Basic::Expression *e_str = params->param(0);
    e_str->execute();
    String str(e_str->getResult().getStringValue());
    
    _result.setIntValue(str.length());
}
