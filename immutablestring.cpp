/*
 *  immutablestring.cpp
 *  Untitled
 *
 *  Created by Daniel on 5/09/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include <cstring>

#include "immutablestring.h"

ImmutableString::ImmutableString(const char *s) {
    _length = strlen(s);
    _cstring = new char[1 + _length];
    strcpy(_cstring, s);
}

ImmutableString::ImmutableString(const ImmutableString &s) {
    _length = s.length();
    _cstring = new char[1 + _length];
    strcpy(_cstring, s.cstring());
}

ImmutableString::~ImmutableString() {
    delete[] _cstring;
}

bool ImmutableString::operator==(const ImmutableString &other) const {
    return strcmp(_cstring, other.cstring()) == 0;
}

bool ImmutableString::operator==(const char *s) const {
    return strcmp(_cstring, s) == 0;
}

bool ImmutableString::operator<(const ImmutableString &other) const {
    return strcmp(_cstring, other.cstring()) < 0;
}

bool ImmutableString::operator<(const char *s) const {
    return strcmp(_cstring, s) < 0;
}

bool ImmutableString::operator>(const ImmutableString &other) const {
    return strcmp(_cstring, other.cstring()) > 0;
}

bool ImmutableString::operator>(const char *s) const {
    return strcmp(_cstring, s) > 0;
}
