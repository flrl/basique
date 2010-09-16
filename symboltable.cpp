/*
 *  symboltable.cpp
 *  Untitled
 *
 *  Created by Daniel on 27/08/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "symboltable.h"

// FIXME need a destructor that clears out all the frames and the stuff in them

void basic::SymbolTable::startScope(void) {
    m_frames.push_back(Frame());
}

void basic::SymbolTable::endScope(void) {
    Frame &frame = m_frames.back();
    for (Frame::iterator symbol = frame.begin(); symbol != frame.end(); symbol++) {
        Entry &entry = symbol->second;
        switch (entry.type) {
                // FIXME what type of cleanup does each of these require?
            case BUILTIN_FUNCTION:  break;
            case FUNCTION:          break;
            case SUBROUTINE:        break;
            case VARIANT:           delete entry.variant;   break;
            case ARRAY:             delete entry.array;     break;
            default:
                fprintf(stderr, "warning: unrecognised symbol table entry type in SymbolTable::endScope()\n");
        }
    }
    
    m_frames.pop_back();
}

const basic::SymbolTable::Entry *basic::SymbolTable::find(const String &identifier, unsigned int accepted_types) const {
    for (std::vector<Frame>::const_reverse_iterator frame = m_frames.rbegin(); frame != m_frames.rend(); frame++) {
        Frame::const_iterator result = frame->find(identifier);
        // iterator of map derefs to a std::pair<key_T, value_T>
        if (result != frame->end() and (accepted_types == 0 or (accepted_types & result->second.type)))  return &result->second;
    }

    return NULL;
}

bool basic::SymbolTable::defined(const String &identifier) const {
    for (std::vector<Frame>::const_reverse_iterator frame = m_frames.rbegin(); frame != m_frames.rend(); frame++) {
        if (frame->count(identifier) > 0)  return true;
    }
    return false;
}

void basic::SymbolTable::defineBuiltinFunction(const String &identifier, builtin::function *binding) {
    Frame &frame = m_frames.back();
    frame.insert(std::make_pair(identifier, Entry(BUILTIN_FUNCTION, binding)));
}

void basic::SymbolTable::defineFunction(const String &identifier, FunctionDefinition *binding) {
    Frame &frame = m_frames.back();
    frame.insert(std::make_pair(identifier, Entry(FUNCTION, binding)));
}

void basic::SymbolTable::defineSubroutine(const String &identifier, SubDefinition *binding) {
    Frame &frame = m_frames.back();
    frame.insert(std::make_pair(identifier, Entry(SUBROUTINE, binding)));
}

void basic::SymbolTable::defineVariant(const String &identifier, Variant *binding) {
    Frame &frame = m_frames.back();
    frame.insert(std::make_pair(identifier, Entry(VARIANT, binding)));
}

void basic::SymbolTable::defineArray(const String &identifier, Array *binding) {
    Frame &frame = m_frames.back();
    frame.insert(std::make_pair(identifier, Entry(ARRAY, binding)));
}
