/*
 *  file.h
 *  Untitled
 *
 *  Created by Ellie on 25/09/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef BASIQUE_FILE_H
#define BASIQUE_FILE_H

#include <cstdio>

#include "string.h"

namespace basic {
    class File;
}

class basic::File {
public:
    enum Mode {
        INPUT = 0,
        OUTPUT = 1,
        APPEND = 2,
    };
    
    File(const String &filename, Mode mode);
    ~File();
    
    operator const FILE*() const { return m_file; }
    operator FILE*() { return m_file; }
    const FILE *c_file() const { return m_file; }
    FILE *c_file() { return m_file; }
    
    bool isValid() const { return m_file != NULL; }
    
    const String &getFilename() const { return m_filename; }
    Mode getMode() const { return m_mode; }
    
private:
    const String m_filename;
    Mode m_mode;
    FILE *m_file;
};

#endif
