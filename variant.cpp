/*
 *  variant.cpp
 *  Untitled
 *
 *  Created by Ellie on 8/01/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "variant.h"

Basic::Variant::Variant(const Variant &v) : m_bool_value(v.m_bool_value), 
                                            m_int_value(v.m_int_value), 
                                            m_double_value(v.m_double_value), 
                                            m_string_value (v.m_string_value), 
                                            m_type (v.m_type) 
{
    ;
}

bool Basic::Variant::getBoolValue(void) const {
    switch (m_type) {
        case VaBOOL:
            return m_bool_value;
        case VaINT:
            return m_int_value != 0;
        case VaDOUBLE:
            return m_double_value != 0.0;
        case VaSTRING:
            return m_string_value.length() > 0 && strcmp(m_string_value, "0") != 0;
        case VaUNDEF:
        default:
            return false;
    }
}

int Basic::Variant::getIntValue(void) const {
    switch (m_type) {
        case VaBOOL:
            return m_bool_value ? 1 : 0;
        case VaINT:
            return m_int_value;
        case VaDOUBLE:
            return static_cast<int>(m_double_value);
        case VaSTRING:
            return strtol(m_string_value, NULL, 10);
        case VaUNDEF:
        default:
            return 0;
    }
}

double Basic::Variant::getDoubleValue(void) const {
    switch (m_type) {
        case VaBOOL:
            return m_bool_value ? 1.0 : 0.0 ;
        case VaINT:
            return 1.0 * m_int_value;
        case VaDOUBLE:
            return m_double_value;
        case VaSTRING:
            return strtod(m_string_value, NULL);
        case VaUNDEF:
        default:
            return 0.0; /* FIXME */

    }
}

String Basic::Variant::getStringValue(void) const {
    char *buffer = NULL;
    
    switch (m_type) {
        case VaBOOL:
            buffer = new char[6];
            strcpy(buffer, m_bool_value ? "true" : "false");
            break;
        case VaINT:
            buffer = new char[16]; //?
            sprintf(buffer, "%d", m_int_value);
            break;
        case VaDOUBLE:
            buffer = new char[32]; //?
            sprintf(buffer, "%f", m_double_value);
            break;
        case VaSTRING:
            return m_string_value;
        case VaUNDEF:
            buffer = new char[1];
            buffer[0] = '\0';
            break;
    }
    
    String result(buffer);
    delete[] buffer;
    return result;
}

void Basic::Variant::setBoolValue(bool b) {
    m_bool_value = b;
    m_int_value = 0;
    m_double_value = 0.0;
    m_string_value = "";
    m_type = VaBOOL;
}

void Basic::Variant::setIntValue(int i) {
    m_bool_value = false;
    m_int_value = i;
    m_double_value = 0.0;
    m_string_value = "";
    m_type = VaINT;    
}

void Basic::Variant::setDoubleValue(double d) {
    m_bool_value = false;
    m_int_value = 0;
    m_double_value = d;
    m_string_value = "";
    m_type = VaDOUBLE;
}

void Basic::Variant::setStringValue(const String &s) {
    m_bool_value = false;
    m_int_value = 0;
    m_double_value = 0.0;
    m_string_value = s;
    m_type = VaSTRING;   
}

void Basic::Variant::setDefaultValueForType(Basic::Variant::Type type){
    switch (type) {
        case VaBOOL:
            setBoolValue(false);
            break;
        case VaINT:
            setIntValue(0);
            break;
        case VaDOUBLE:
            setDoubleValue(0.0);
            break;
        case VaSTRING:
            setStringValue("");
            break;
        case VaUNDEF:
        default:
            m_type = VaUNDEF;
    }
}

Basic::Variant &Basic::Variant::operator+=(const Basic::Variant &other) {
    if (other.isUndef()) {
        fprintf(stderr, "warning: undefined value in addition at line ..., column ...\n");
    }
    
    if (m_type == VaSTRING) {
        m_string_value += other.getStringValue();
    }
    else if (m_type == VaDOUBLE or (other.isDouble() and (m_type == VaUNDEF or m_type == VaBOOL or m_type == VaINT))) {
        setDoubleValue(getDoubleValue() + other.getDoubleValue());
    }
    else if (m_type == VaINT or (other.isInt() and m_type == VaBOOL)) {
        setIntValue(getIntValue() + other.getIntValue());
    }
    else if (m_type == VaBOOL) {
        setIntValue(getBoolValue() + other.getBoolValue()); // FIXME
    }
    else if (m_type == VaUNDEF) {
        fprintf(stderr, "warning: undefined value in addition at line ..., column ...\n");
    }
    else {
        fprintf(stderr, "debug: unhandled condition in Variant operator+=\n");
    }
    
    return *this;
}

Basic::Variant &Basic::Variant::operator-=(const Basic::Variant &other) {
    if (other.isUndef()) {
        fprintf(stderr, "warning: undefined value in subtraction at line ..., column ...\n");        
    }
    
    if (isString()) {
        fprintf(stderr, "warning: attempt to subtract from a string at line ..., column ...\n");
    }
    else if (isDouble() or (other.isDouble() and (isUndef() or isBool() or isInt()))) {
        setDoubleValue(getDoubleValue() - other.getDoubleValue());
    }
    else if (isInt() or (other.isInt() and isBool())) {
        setIntValue(getIntValue() - other.getIntValue());
    }
    else if (isBool()) {
        setIntValue(getBoolValue() - other.getBoolValue()); // FIXME
    }
    else if (isUndef()) {
        fprintf(stderr, "warning: undefined value in subtraction at line ..., column ...\n");
    }
    else {
        fprintf(stderr, "debug: unhandled condition in Variant operator-=\n");
    }    
    
    return *this;
}

Basic::Variant &Basic::Variant::operator*=(const Basic::Variant &other) {
    if (other.isUndef()) {
        fprintf(stderr, "warning: undefined value in multiplication at line ..., column ...\n");
    }
    
    if (isString()) {
        fprintf(stderr, "warning: attempt to multiply a string at line ..., column ...\n");
    }
    else if (isDouble() or (other.isDouble() and (isUndef() or isBool() or isInt()))) {
        setDoubleValue(getDoubleValue() * other.getDoubleValue());
    }
    else if (isInt() or (other.isInt() and isBool())) {
        setIntValue(getIntValue() * other.getIntValue());
    }
    else if (isBool()) {
        setBoolValue(getBoolValue() * other.getBoolValue()); // FIXME
    }
    else if (isUndef()) {
        fprintf(stderr, "warning: undefined value in multiplication at line ..., column ...\n");
    }
    else {
        fprintf(stderr, "debug: unhandled condition in Variant operator*=\n");
    }
    
    return *this;
}

Basic::Variant &Basic::Variant::operator/=(const Basic::Variant &other) {
    if (other.isUndef()) {
        fprintf(stderr, "warning: undefined value in division at line ..., column ...\n");
    }
    
    if (isString()) {
        fprintf(stderr, "warning: attempt to divide a string at line ..., column ...\n");
    }
    else if (isDouble() or isInt() or isBool()) {
        if (other.getIntValue() == 0) {
            fprintf(stderr, "warning: attempt to divide by zero at line ..., column ...\n");
        }
        else {
            setDoubleValue(getDoubleValue() / other.getDoubleValue());            
        }
    }
    else if (isUndef()) {
        fprintf(stderr, "warning: undefined value in division at line ..., column ...\n");
    }
    else {
        fprintf(stderr, "debug: unhandled condition in Variant operator/=\n");
    }
    
    return *this;
}
