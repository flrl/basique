/*
 *  symboltable.cpp
 *  Untitled
 *
 *  Created by Ellie on 27/08/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "symboltable.h"

// FIXME need a destructor that clears out all the frames and the stuff in them

void Basic::SymbolTable::startScope(void) {
    m_frames.push_back(Frame());
}

void Basic::SymbolTable::endScope(void) {
    Frame &frame = m_frames.back();
    for (Frame::iterator symbol = frame.begin(); symbol != frame.end(); symbol++) {
        Entry &entry = symbol->second;
        switch (entry.type) {
                // FIXME what type of cleanup does each of these require?
            case BUILTIN_FUNCTION:  break;
            case FUNCTION:          break;
            case SUBROUTINE:        break;
            case VARIANT:           delete static_cast<Variant *>(entry.binding);   break;
            case ARRAY:             delete static_cast<Array *>(entry.binding);     break;
            default:
                fprintf(stderr, "warning: unrecognised symbol table entry type in SymbolTable::endScope()\n");
        }
    }
    
    m_frames.pop_back();
}

const Basic::SymbolTable::Entry *Basic::SymbolTable::find(const String &identifier) const {
    for (std::vector<Frame>::const_reverse_iterator frame = m_frames.rbegin(); frame != m_frames.rend(); frame++) {
        Frame::const_iterator result = frame->find(identifier);
        if (result != frame->end())  return &result->second;  // iterator of map derefs to pair<key_t, value_t>
    }

    return NULL;
}

bool Basic::SymbolTable::defined(const String &identifier) const {
    for (std::vector<Frame>::const_reverse_iterator frame = m_frames.rbegin(); frame != m_frames.rend(); frame++) {
        if (frame->count(identifier) > 0)  return true;
    }
    return false;
}

void Basic::SymbolTable::defineBuiltinFunction(const String &identifier, BuiltinFunction *binding) {
    Frame &frame = m_frames.back();
    frame.insert(std::make_pair(identifier, Entry(BUILTIN_FUNCTION, binding)));
}

void Basic::SymbolTable::defineFunction(const String &identifier, FunctionDefinition *binding) {
    Frame &frame = m_frames.back();
    frame.insert(std::make_pair(identifier, Entry(FUNCTION, binding)));
}

void Basic::SymbolTable::defineSubroutine(const String &identifier, SubDefinition *binding) {
    Frame &frame = m_frames.back();
    frame.insert(std::make_pair(identifier, Entry(SUBROUTINE, binding)));
}

void Basic::SymbolTable::defineVariant(const String &identifier, Variant *binding) {
    Frame &frame = m_frames.back();
    frame.insert(std::make_pair(identifier, Entry(VARIANT, binding)));
}

void Basic::SymbolTable::defineArray(const String &identifier, Array *binding) {
    Frame &frame = m_frames.back();
    frame.insert(std::make_pair(identifier, Entry(ARRAY, binding)));
}
