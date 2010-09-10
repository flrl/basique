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

Basic::Variant::Variant(void) : string_value("") {
    this->bool_value = 0;
    this->int_value = 0;
    this->double_value = 0.0;
    this->type = Basic::Variant::is_undef;
}

Basic::Variant::Variant(const Variant &other) {
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

bool Basic::Variant::getBoolValue(void) const {
    switch (this->type) {
        case is_bool:
            return bool_value;
        case is_int:
            return int_value != 0;
        case is_double:
            return double_value != 0.0;
        case is_string:
            return string_value.length() > 0 && strcmp(string_value, "0") != 0;
        case is_undef:
        default:
            return false;
    }
}

int Basic::Variant::getIntValue(void) const {
    switch (this->type) {
        case is_bool:
            return bool_value ? 1 : 0;
        case is_int:
            return int_value;
        case is_double:
            return static_cast<int>(double_value);
        case is_string:
            return strtol(string_value, NULL, 10);
        case is_undef:
        default:
            return 0;
    }
}

double Basic::Variant::getDoubleValue(void) const {
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
String Basic::Variant::getStringValue(void) const {
    char *buffer = NULL;
    
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
            return this->string_value;
        case is_undef:
            buffer = new char[1];
            buffer[0] = '\0';
            break;
    }
    
    String result(buffer);
    delete[] buffer;
    return result;
}

void Basic::Variant::setBoolValue(bool b) {
    this->bool_value = b;
    this->int_value = 0;
    this->double_value = 0.0;
    this->string_value = "";
    this->type = is_bool;
}

void Basic::Variant::setIntValue(int i) {
    this->bool_value = false;
    this->int_value = i;
    this->double_value = 0.0;
    this->string_value = "";
    this->type = is_int;    
}

void Basic::Variant::setDoubleValue(double d) {
    this->bool_value = false;
    this->int_value = 0;
    this->double_value = d;
    this->string_value = "";
    this->type = is_double;
}

void Basic::Variant::setStringValue(const String &s) {
    this->bool_value = false;
    this->int_value = 0;
    this->double_value = 0.0;
    this->string_value = s;
    this->type = is_string;   
}

void Basic::Variant::setDefaultValueForType(Basic::Variant::Type type){
    switch (type) {
        case is_bool:
            setBoolValue(false);
            break;
        case is_int:
            setIntValue(0);
            break;
        case is_double:
            setDoubleValue(0.0);
            break;
        case is_string:
            setStringValue("");
            break;
        case is_undef:
        default:
            type = is_undef;
    }
}

Basic::Variant operator+(const Basic::Variant &left, const Basic::Variant &right) {
    Basic::Variant result;
    
    if (right.isUndef()) {
        fprintf(stderr, "warning: undefined value in addition at line ..., column ...\n");        
    }
    
    if (left.isString()) {
        result.setStringValue(left.getStringValue() + right.getStringValue());
    }
    else if (left.isDouble() or (right.isDouble() and (left.isUndef() or left.isBool() or left.isInt()))) {
        result.setDoubleValue(left.getDoubleValue() + right.getDoubleValue());
    }
    else if (left.isInt() or (right.isInt() and left.isBool())) {
        result.setIntValue(left.getIntValue() + right.getIntValue());
    }
    else if (left.isBool()) {
        result.setIntValue(left.getBoolValue() + right.getBoolValue()); // FIXME
    }
    else if (left.isUndef()) {
        fprintf(stderr, "warning: undefined value in addition at line ..., column ...\n");   
        result = left;
    }
    else {
        fprintf(stderr, "debug: unhandled condition in Variant operator+\n");
        result = left;
    }
    
    return result;
}

Basic::Variant operator-(const Basic::Variant &left, const Basic::Variant &right) {
    Basic::Variant result;
    
    if (right.isUndef()) {
        fprintf(stderr, "warning: undefined value in subtraction at line ..., column ...\n");        
    }
    
    if (left.isString()) {
        fprintf(stderr, "warning: attempt to subtract from a string at line ..., column ...\n");
        result = left;
    }
    else if (left.isDouble() or (right.isDouble() and (left.isUndef() or left.isBool() or left.isInt()))) {
        result.setDoubleValue(left.getDoubleValue() - right.getDoubleValue());
    }
    else if (left.isInt() or (right.isInt() and left.isBool())) {
        result.setIntValue(left.getIntValue() - right.getIntValue());
    }
    else if (left.isBool()) {
        result.setIntValue(left.getBoolValue() - right.getBoolValue()); // FIXME
    }
    else if (left.isUndef()) {
        fprintf(stderr, "warning: undefined value in subtraction at line ..., column ...\n");
        result = left;
    }
    else {
        fprintf(stderr, "debug: unhandled condition in Variant operator-\n");
        result = left;
    }    
    
    return result;
}
