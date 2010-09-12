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
void Basic::BF_left::call(const ParamList *params) {
    m_result.setUndefined();
    if (params->size() != 2)  return;

    const Expression *e_str = params->param(0);
    e_str->execute();
    String str(e_str->getResult().getStringValue());
    
    const Expression *e_len = params->param(1);
    e_len->execute();
    int len = e_len->getResult().getIntValue();
    if (len < 1)  return;

    char *buffer = new char[1 + len];
    strncpy(buffer, str, len);
    m_result.setStringValue(buffer);
    delete[] buffer;
}

// str right(str, len)
void Basic::BF_right::call(const ParamList *params) {
    m_result.setUndefined();
    if (params->size() != 2)  return;
    
    const Expression *e_str = params->param(0);
    e_str->execute();
    String str(e_str->getResult().getStringValue());
    
    const Expression *e_len = params->param(1);
    e_len->execute();
    int len = e_len->getResult().getIntValue();
    if (len < 1)  return;
    
    const char *p = str;
    p += str.length() - len;
    m_result.setStringValue(p);    
}

// str mid(str, start, len)
void Basic::BF_mid::call(const ParamList *params) {
    m_result.setUndefined();
    if (params->size() != 3)  return;
    
    const Expression *e_str = params->param(0);
    e_str->execute();
    String str(e_str->getResult().getStringValue());
    
    const Expression *e_start = params->param(1);
    e_start->execute();
    int start = e_start->getResult().getIntValue();
    if (start < 0)  return;
    
    const Expression *e_len = params->param(2);
    e_len->execute();
    int len = e_len->getResult().getIntValue();
    if (len < 1)  return;
    
    const char *p = str;
    p += start;
    char *buffer = new char[1 + len];
    strncpy(buffer, p, len);
    m_result.setStringValue(buffer);
    delete[] buffer;
}

// int len(str)
void Basic::BF_len::call(const ParamList *params) {
    m_result.setUndefined();
    if (params->size() != 1)  return;
    
    const Expression *e_str = params->param(0);
    e_str->execute();
    String str(e_str->getResult().getStringValue());
    
    m_result.setIntValue(str.length());
}
