/*
 *  variant.h
 *  Untitled
 *
 *  Created by Daniel on 8/01/10.
 *  Copyright 2010 Daniel Hyde. All rights reserved.
 *
 */
#ifndef BASIQUE_VARIANT_H
#define BASIQUE_VARIANT_H

#include <cstdlib>

#include "string.h"

namespace basic {
    class Variant;
}

class basic::Variant {
public:
    enum Type { VaUNDEF, VaBOOL, VaINT, VaDOUBLE, VaSTRING };
    
    Variant() : m_bool_value(false), m_int_value(0), m_double_value(0.0), m_string_value(""), m_type(VaUNDEF) {}
    Variant(const Variant &);
    Variant(const String &s) { setStringValue(s); }
    Variant(const char *s) { setStringValue(s); }
    Variant(double d) { setDoubleValue(d); }    
    Variant(int i) { setIntValue(i); }    
    Variant(bool b) { setBoolValue(b); }
    Variant(Type t) { setDefaultValueForType(t); }
    
    ~Variant(void) { }
    
    bool    getBoolValue(void) const;
    int     getIntValue(void) const;
    double  getDoubleValue(void) const;
    String  getStringValue(void) const;
    Type    getType(void) const { return m_type; }

    void setBoolValue(bool);
    void setIntValue(int);
    void setDoubleValue(double);
    void setStringValue(const String &);
    void setDefaultValueForType(Type);
    void setUndefined(void) { setDefaultValueForType(VaUNDEF); }
    
    bool isDefined(void) const  { return m_type != VaUNDEF; }
    bool isUndef(void) const    { return m_type == VaUNDEF; }
    bool isBool(void) const     { return m_type == VaBOOL; }
    bool isInt(void) const      { return m_type == VaINT; }
    bool isDouble(void) const   { return m_type == VaDOUBLE; }
    bool isString(void) const   { return m_type == VaSTRING; }
    
//    Variant &operator=(const Variant &);
    Variant &operator+=(const Variant &);
    Variant &operator-=(const Variant &);
    Variant &operator*=(const Variant &);
    Variant &operator/=(const Variant &);
    
    int compare(const Variant &) const;
    
private:
    bool    m_bool_value;
    int     m_int_value;
    double  m_double_value;
    String  m_string_value;
    Type    m_type;
};

inline basic::Variant operator+(const basic::Variant &left, const basic::Variant &right) {
    basic::Variant result(left);
    result += right;
    return result;
}

inline basic::Variant operator-(const basic::Variant &left, const basic::Variant &right) {
    basic::Variant result(left);
    result -= right;
    return result;
}

inline basic::Variant operator*(const basic::Variant &left, const basic::Variant &right) {
    basic::Variant result(left);
    result *= right;
    return result;
}

inline basic::Variant operator/(const basic::Variant &left, const basic::Variant &right) {
    basic::Variant result(left);
    result /= right;
    return result;
}

inline bool operator<(const basic::Variant &left, const basic::Variant &right) {
    return left.compare(right) < 0;
}

inline bool operator>(const basic::Variant &left, const basic::Variant &right) {
    return left.compare(right) > 0;
}

inline bool operator>=(const basic::Variant &left, const basic::Variant &right) {
    return left.compare(right) >= 0;
}

inline bool operator<=(const basic::Variant &left, const basic::Variant &right) {
    return left.compare(right) <= 0;
}

inline bool operator==(const basic::Variant &left, const basic::Variant &right) {
    return left.compare(right) == 0;
}

inline bool operator!=(const basic::Variant &left, const basic::Variant &right) {
    return left.compare(right) != 0;
}

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
