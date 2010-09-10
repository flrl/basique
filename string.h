/*
 *  string.h
 *  Untitled
 *
 *  Created by Daniel on 5/09/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _IMMUTABLESTRING_H
#define _IMMUTABLESTRING_H

class String {
public:
    String(const char *);
    String(const String &);
    String();
    ~String();
    
    operator const char * () const { return _cstring; }
    size_t length(void) const { return _length; }
    
    bool operator==(const String &) const;
    bool operator<(const String &) const;
    bool operator>(const String &) const;

    
    String &operator=(const String &);
    String &operator+=(const String &);
    
private:
    size_t _length;
    size_t _size;
    char *_cstring;
};

String operator+(const String &, const String &);

#endif
