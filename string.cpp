/*
 *  immutablestring.cpp
 *  Untitled
 *
 *  Created by Ellie on 5/09/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include <cstring>

#include "string.h"

String::String(const char *s=NULL) {
    if (s) {
        _length = strlen(s);
        _size = 1 + _length;
        _cstring = new char[_size];
        strcpy(_cstring, s);   
    }
    else {
        _length = 0;
        _size = 1;
        _cstring = new char[_size];
        _cstring[0] = '\0';
    }
}

String::String(const String &s) {
    _length = s.length();
    _size = 1 + _length;
    _cstring = new char[_size];
    strcpy(_cstring, s.cstring());
}

String::~String() {
    delete[] _cstring;
}

bool String::operator==(const String &other) const {
    return strcmp(_cstring, other.cstring()) == 0;
}

bool String::operator<(const String &other) const {
    return strcmp(_cstring, other.cstring()) < 0;
}

bool String::operator>(const String &other) const {
    return strcmp(_cstring, other.cstring()) > 0;
}

void String::operator=(const String &other) {
    if (other.length() >= _size) {
        delete[] _cstring;
        _length = other.length();
        _size = 1 + _length;
        _cstring = new char[_size];
        strcpy(_cstring, other.cstring());        
    }
    else {
        strcpy(_cstring, other.cstring());
    }
}
