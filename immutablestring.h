/*
 *  immutablestring.h
 *  Untitled
 *
 *  Created by Ellie on 5/09/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _IMMUTABLESTRING_H
#define _IMMUTABLESTRING_H

class ImmutableString {
public:
    ImmutableString(const char *);
    ImmutableString(const ImmutableString &);
    ~ImmutableString();
    
    const char *cstring(void) const { return _cstring; }
    size_t length(void) const { return _length; }
    
    bool operator==(const ImmutableString &) const;
    bool operator==(const char *) const;
    bool operator<(const ImmutableString &) const;
    bool operator<(const char *) const;
    bool operator>(const ImmutableString &) const;
    bool operator>(const char *) const;
    
private:
    ImmutableString();
    void operator=(const ImmutableString &);
    char *_cstring;
    size_t _length;
};

#endif
