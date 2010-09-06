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

String::String(const char *s) {
    _length = strlen(s);
    _cstring = new char[1 + _length];
    strcpy(_cstring, s);
}

String::String(const String &s) {
    _length = s.length();
    _cstring = new char[1 + _length];
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
    if (_cstring)  delete[] _cstring;
    _length = other.length();
    _cstring = new char[1 + _length];
    strcpy(_cstring, other.cstring());
}
