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

    String str(params->paramResult(0).getStringValue());
    
    int len = params->paramResult(1).getIntValue();
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
    
    String str(params->paramResult(0).getStringValue());
    
    int len = params->paramResult(1).getIntValue();
    if (len < 1)  return;
    
    const char *p = str;
    p += str.length() - len;
    m_result.setStringValue(p);    
}

// str mid(str, start, len)
void Basic::BF_mid::call(const ParamList *params) {
    m_result.setUndefined();
    if (params->size() != 3)  return;
    
    String str(params->paramResult(0).getStringValue());
    
    int start = params->paramResult(1).getIntValue();
    if (start < 0)  return;
    
    int len = params->paramResult(2).getIntValue();
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
    
    String str(params->paramResult(0).getStringValue());
    
    m_result.setIntValue(str.length());
}
