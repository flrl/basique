/*
 *  variant.h
 *  Untitled
 *
 *  Created by Daniel on 8/01/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef _VARIANT_H
#define _VARIANT_H

enum VariantType {
    is_undef, is_bool, is_int, is_double, is_string,
};

class Variant {
public:
    Variant(void);
    Variant(const Variant&);
    Variant(int i) { this->setIntValue(i); }    
    Variant(double d) { this->setDoubleValue(d); }    
    Variant(const char *s) { this->setStringValue(s); }
    
    ~Variant(void) { clearStringValue(); }
    
    bool getBoolValue(void) const;
    int getIntValue(void) const;
    double getDoubleValue(void) const;
    const char* getStringValue(void) const;
    VariantType getType(void) const { return this->type; }

    void setBoolValue(bool);
    void setIntValue(int);
    void setDoubleValue(double);
    void setStringValue(const char *);
    
    bool isDefined(void) const { return this->type == is_undef; }
    
private:
    bool bool_value;
    int int_value;
    double double_value;
    char* string_value; //?
    VariantType type;
    
    void clearStringValue(void) { 
        if (string_value != NULL) delete[] string_value; 
        string_value = NULL; 
    }
};

#endif