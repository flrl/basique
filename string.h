/*
 *  string.h
 *  Untitled
 *
 *  Created by Ellie on 5/09/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _STRING_H
#define _STRING_H

class String {
public:
    String(const char *);
    String(const String &);
    String();
    ~String();
    
    const char *c_str() const { return m_cstring; }
    size_t length(void) const { return m_length; }
    
    operator const char*(void) const { return m_cstring; }
    bool operator==(const String &) const;
    bool operator<(const String &) const;
    bool operator>(const String &) const;

    String &operator=(const String &);
    String &operator+=(const String &);


private:
    size_t  m_size;
    size_t  m_length;
    char   *m_cstring;
};

String operator+(const String &, const String &);

#endif
