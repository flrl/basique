/*
 *  variant.cpp
 *  Untitled
 *
 *  Created by Daniel on 8/01/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "variant.h"

Variant::Variant(void) {
    this->bool_value = 0;
    this->int_value = 0;
    this->double_value = 0.0;
    this->clearStringValue();
    this->type = is_undef;
}

Variant::Variant(const Variant &other) {
    switch(other.type) {
        case is_bool:
            this->setBoolValue(other.bool_value);
            break;
        case is_int:
            this->setIntValue(other.int_value);
            break;
        case is_double:
            this->setDoubleValue(other.double_value);
            break;
        case is_string:
            this->setStringValue(other.string_value);
            break;
        case is_undef:
        default:
            this->type = other.type;
            break;
    }
}

bool Variant::getBoolValue(void) const {
    switch (this->type) {
        case is_bool:
            return bool_value;
        case is_int:
            return int_value != 0;
        case is_double:
            return double_value != 0.0;
        case is_string:
            return string_value != NULL && string_value[0] != '\0' && strcmp(string_value, "0") != 0;
        case is_undef:
        default:
            return false;
    }
}

int Variant::getIntValue(void) const {
    switch (this->type) {
        case is_bool:
            return bool_value ? 1 : 0;
        case is_int:
            return int_value;
        case is_double:
            return static_cast<int>(double_value);
        case is_string:
            return string_value != NULL ? strtol(string_value, NULL, 10) : 0;
        case is_undef:
        default:
            return 0;
    }
}

double Variant::getDoubleValue(void) const {
    switch (this->type) {
        case is_bool:
            return this->bool_value ? 1.0 : 0.0 ;
        case is_int:
            return 1.0 * this->int_value;
        case is_double:
            return this->double_value;
        case is_string:
            return strtod(this->string_value, NULL);
        case is_undef:
        default:
            return 0.0; /* FIXME */

    }
}

/* uses dynamically allocated memory, but manages it itself. 
 memory allocated at last call is freed next time. */
const char * Variant::getStringValue(void) const {
    static char *buffer = NULL;
    
    if (buffer) {
        delete[] buffer;
        buffer = NULL;
    }
    switch (this->type) {
        case is_bool:
            buffer = new char[6];
            strcpy(buffer, this->bool_value ? "true" : "false");
            break;
        case is_int:
            buffer = new char[16]; //?
            sprintf(buffer, "%d", this->int_value);
            break;
        case is_double:
            buffer = new char[32]; //?
            sprintf(buffer, "%f", this->double_value);
            break;
        case is_string:
            buffer = new char[1 + strlen(this->string_value)];
            strcpy(buffer, this->string_value);
            break;
        case is_undef:
            buffer = new char[1];
            buffer[0] = '\0';
            break;
    }
    return buffer;
}

void Variant::setBoolValue(bool b) {
    this->bool_value = b;
    this->int_value = 0;
    this->double_value = 0.0;
    clearStringValue();
    this->type = is_bool;
}

void Variant::setIntValue(int i) {
    this->bool_value = false;
    this->int_value = i;
    this->double_value = 0.0;
    clearStringValue();
    this->type = is_int;    
}

void Variant::setDoubleValue(double d) {
    this->bool_value = false;
    this->int_value = 0;
    this->double_value = d;
    clearStringValue();
    this->type = is_double;
}

void Variant::setStringValue(const char *s) {
    this->bool_value = false;
    this->int_value = 0;
    this->double_value = 0.0;
    clearStringValue();
    if (s != NULL) {
        this->string_value = new char[1 + strlen(s)];
        strcpy(this->string_value, s);
        this->type = is_string;   
    }
    else {
        this->type = is_undef;
    }
}