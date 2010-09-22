/*
 *  string.cpp
 *  Untitled
 *
 *  Created by Ellie on 5/09/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include <cstring>

#include "string.h"

String::String(const char *s) {
    if (s) {
        m_length = strlen(s);
        m_size = 1 + m_length;
        m_cstring = new char[m_size];
        strcpy(m_cstring, s);   
    }
    else {
        m_length = 0;
        m_size = 1;
        m_cstring = new char[m_size];
        m_cstring[0] = '\0';
    }
}

String::String(const String &s) {
    m_length = s.length();
    m_size = 1 + m_length;
    m_cstring = new char[m_size];
    strcpy(m_cstring, s);
}

String::String() {
    m_length = 0;
    m_size = 1;
    m_cstring = new char[m_size];
    m_cstring[0] = '\0';
}

String::~String() {
    delete[] m_cstring;
}

bool String::operator==(const String &other) const {
    return strcmp(m_cstring, other) == 0;
}

bool String::operator<(const String &other) const {
    return strcmp(m_cstring, other) < 0;
}

bool String::operator>(const String &other) const {
    return strcmp(m_cstring, other) > 0;
}

String &String::operator=(const String &other) {
    if (this == &other)  return *this;
    
    if (other.length() >= m_size) {
        delete[] m_cstring;
        m_length = other.length();
        m_size = 1 + m_length;
        m_cstring = new char[m_size];
        strcpy(m_cstring, other);        
    }
    else {
        m_length = other.length();
        strcpy(m_cstring, other);
    }
    
    return *this;
}

String &String::operator+=(const String &other) {
    m_length += other.length();
    if (m_size <= m_length) {
        m_size = 1 + m_length;
        char *tmp = new char[m_size];
        strcpy(tmp, m_cstring);
        strcat(tmp, other);
        delete[] m_cstring;
        m_cstring = tmp;
    }
    else {
        strcat(m_cstring, other);
    }
    
    return *this;
}

String operator+(const String &left, const String &right) {
    char *tmp = new char[1 + left.length() + right.length()];
    strcpy(tmp, left);
    strcat(tmp, right);
    String result(tmp);
    delete[] tmp;
    return result;
}
