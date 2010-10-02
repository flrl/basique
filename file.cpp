/*
 *  file.cpp
 *  Untitled
 *
 *  Created by Ellie on 25/09/10.
 *  Copyright 2010 Ellie. All rights reserved.
 *
 */

#include <cerrno>
#include <cstring>

#include "file.h"

basic::File::File(const String &filename, Mode mode) : m_filename(filename), m_mode(mode), m_file(NULL) {
    const static char *mode_descriptions[] = {
        "for input", "for output", "to append",
    };
    
    switch(m_mode) {
        case INPUT:
            m_file = fopen(m_filename, "r");
            break;
        case OUTPUT:
            m_file = fopen(m_filename, "w");
            break;
        case APPEND:
            m_file = fopen(m_filename, "a");
            break;
    }

    if (m_file == NULL) {
        fprintf(stderr, "error: couldn't open %s %s: %s\n", m_filename.c_str(), mode_descriptions[m_mode], strerror(errno));
    }
}

basic::File::~File() {
//    fprintf(stderr, "debug: file %s closing...\n", m_filename.c_str());
    if (m_file != NULL) {
        if (fclose(m_file) == EOF) {
            fprintf(stderr, "warning: error closing %s: %s\n", m_filename.c_str(), strerror(errno));
        }
        m_file = NULL;
    }
}
