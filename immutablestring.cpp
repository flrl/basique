/*
 *  immutablestring.cpp
 *  Untitled
 *
 *  Created by Ellie on 5/09/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include <cstring>

#include "immutablestring.h"

ImmutableString::ImmutableString(const char *s) {
    value = new char[1 + strlen(s)];
    strcpy(value, s);
}

ImmutableString::ImmutableString(const ImmutableString &s) {
    value = new char[1 + strlen(s.value)];
    strcpy(value, s.value);
}

ImmutableString::~ImmutableString() {
    delete[] value;
}

const char *ImmutableString::cstring(void) const {
    return value;
}

bool ImmutableString::operator==(const ImmutableString &other) const {
    return strcmp(value, other.value) == 0;
}

bool ImmutableString::operator==(const char *other) const {
    return strcmp(value, other) == 0;
}

bool ImmutableString::operator<(const ImmutableString &other) const {
    return strcmp(value, other.value) < 0;
}

bool ImmutableString::operator<(const char *other) const {
    return strcmp(value, other) < 0;
}

bool ImmutableString::operator>(const ImmutableString &other) const {
    return strcmp(value, other.value) > 0;
}

bool ImmutableString::operator>(const char *other) const {
    return strcmp(value, other) > 0;
}
