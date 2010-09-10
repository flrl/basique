/*
 *  variant.h
 *  Untitled
 *
 *  Created by Ellie on 8/01/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef _VARIANT_H
#define _VARIANT_H

#include <cstdlib>

#include "string.h"

namespace Basic {
    class Variant;
}

class Basic::Variant {
public:
    enum Type {
        is_undef, is_bool, is_int, is_double, is_string,
    };
    
    Variant(void);
    Variant(const Variant&);
    Variant(bool b) { this->setBoolValue(b); }
    Variant(int i) { this->setIntValue(i); }    
    Variant(double d) { this->setDoubleValue(d); }    
    Variant(const String &s) { this->setStringValue(s); }
    Variant(Type t) { this->setDefaultValueForType(t); }
    
    ~Variant(void) { }
    
    bool getBoolValue(void) const;
    int getIntValue(void) const;
    double getDoubleValue(void) const;
    String getStringValue(void) const;
    Type getType(void) const { return this->type; }

    void setBoolValue(bool);
    void setIntValue(int);
    void setDoubleValue(double);
    void setStringValue(const String &);
    
    void setDefaultValueForType(Type);
    
    bool isDefined(void) const  { return this->type != is_undef; }
    bool isUndef(void) const    { return this->type == is_undef; }
    bool isBool(void) const     { return this->type == is_bool; }
    bool isInt(void) const      { return this->type == is_int; }
    bool isDouble(void) const   { return this->type == is_double; }
    bool isString(void) const   { return this->type == is_string; }
    
private:
    bool bool_value;
    int int_value;
    double double_value;
    String string_value;
    Type type;
};

Basic::Variant operator+(const Basic::Variant &, const Basic::Variant &);
Basic::Variant operator-(const Basic::Variant &, const Basic::Variant &);


// FIXME operator overloads

/*
 types are bool, int, double, string, undef
 categories are "numeric" (int, double) or "string" (string) or ???
 operators generally keep the type of their left hand side, thus:
    int i + string s == i + atoi(s)
 if both arguments are numeric and either is a double, the result is a double
 if the left operand is numeric and the right a string, the string is parsed to a numeric value
 if the left operand is a string, the right operand is converted to a string (eg sprintf)
 if either operand is undef, the result is undef (?? FIXME)
 
 the usual numeric operators are valid in numeric contexts
 in string context, + performs concatenation, - / * are invalid
 numeric operators first convert bools to ints (?? FIXME)
 
 */


#endif
