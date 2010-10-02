/*
 *  symboltable.cpp
 *  Untitled
 *
 *  Created by Daniel on 27/08/10.
 *  Copyright 2010 Daniel Hyde. All rights reserved.
 *
 */

#include "symboltable.h"

basic::SymbolTable::SymbolTable() : m_frames() {
    startScope();
    installBuiltins();
    startScope();
}

basic::SymbolTable::~SymbolTable() {
    // FIXME clear out all the frames and the stuff in them    
    endScope();
    endScope();
}

void basic::SymbolTable::installBuiltins() {
    defineBuiltinFunction("left", &basic::builtin::left);
    defineBuiltinFunction("right", &basic::builtin::right);
    defineBuiltinFunction("mid", &basic::builtin::mid);
    defineBuiltinFunction("len", &basic::builtin::len);
    defineBuiltinFunction("type", &basic::builtin::type);
}

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
            case FILEHANDLE:        delete entry.file;      break;
            default:
                fprintf(stderr, "warning: unrecognised symbol table entry type in SymbolTable::endScope()\n");
        }
    }
    
    m_frames.pop_back();
}

const basic::SymbolTable::Entry *basic::SymbolTable::find_helper(const String &identifier, unsigned int accepted_types) const {
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

// return NULL on success (SymbolTable has taken ownership of binding)
// return binding on failure (SymbolTable can't take ownership, caller retains responsibility)
basic::builtin::function *basic::SymbolTable::defineBuiltinFunction(const String &identifier, builtin::function *binding) {
    Frame &frame = m_frames.back();
    if (frame.count(identifier) > 0) {
        fprintf(stderr, "error: couldn't define builtin function `%s': identifier already exists in current scope\n", identifier.c_str()); 
        return binding;
    }
    else {
        std::pair<Frame::iterator, bool> r = frame.insert(std::make_pair(identifier, Entry(BUILTIN_FUNCTION, binding)));
        if (r.second) {
            // success
            return NULL;  
        } 
        else {
            fprintf(stderr, "error: couldn't define builtin function `%s': map insertion failed\n", identifier.c_str()); 
            return binding;
        }
    }
}

// return NULL on success (SymbolTable has taken ownership of binding)
// return binding on failure (SymbolTable can't take ownership, caller retains responsibility)
const basic::FunctionDefinition *basic::SymbolTable::defineFunction(const String &identifier, const FunctionDefinition *binding) {
    Frame &frame = m_frames.back();
    if (frame.count(identifier) > 0) {
        fprintf(stderr, "error: couldn't define function `%s': identifier already exists in current scope\n", identifier.c_str()); 
        return binding;
    }
    else {
        std::pair<Frame::iterator, bool> r = frame.insert(std::make_pair(identifier, Entry(FUNCTION, binding)));
        if (r.second) {
            // success
            return NULL;
        }
        else {
            fprintf(stderr, "error: couldn't define builtin function `%s': map insertion failed\n", identifier.c_str()); 
            return binding;
        }
    }
}

// return NULL on success (SymbolTable has taken ownership of binding)
// return binding on failure (SymbolTable can't take ownership, caller retains responsibility)
const basic::SubDefinition *basic::SymbolTable::defineSubroutine(const String &identifier, const SubDefinition *binding) {
    Frame &frame = m_frames.back();
    if (frame.count(identifier) > 0) {
        fprintf(stderr, "error: couldn't define subroutine `%s': identifier already exists in current scope\n", identifier.c_str());
        return binding;
    }
    else {
        std::pair<Frame::iterator, bool> r = frame.insert(std::make_pair(identifier, Entry(SUBROUTINE, binding)));
        if (r.second) {
            // success
            return NULL;
        }
        else {
            fprintf(stderr, "error: couldn't define subroutine `%s': map insertion failed\n", identifier.c_str());
            return binding;
        }
    }
}

// return NULL on success (SymbolTable has taken ownership of binding)
// return binding on failure (SymbolTable can't take ownership, caller retains responsibility)
basic::Variant *basic::SymbolTable::defineVariant(const String &identifier, Variant *binding) {
    Frame &frame = m_frames.back();
    if (frame.count(identifier) > 0) {
        fprintf(stderr, "error: couldn't define variant `%s': identifier already exists in current scope\n", identifier.c_str());
        return binding;
    }
    else {
        std::pair<Frame::iterator, bool> r = frame.insert(std::make_pair(identifier, Entry(VARIANT, binding)));
        if (r.second) {
            // success
            return NULL;
        }
        else {
            fprintf(stderr, "error: couldn't define variant `%s': map insertion failed\n", identifier.c_str());
            return binding;
        }
    }
}

// return NULL on success (SymbolTable has taken ownership of binding)
// return binding on failure (SymbolTable can't take ownership, caller retains responsibility)
basic::Array *basic::SymbolTable::defineArray(const String &identifier, Array *binding) {
    Frame &frame = m_frames.back();
    if (frame.count(identifier) > 0) {
        fprintf(stderr, "error: couldn't define array `%s': identifier already exists in current scope\n", identifier.c_str());
        return binding;
    }
    else {
        std::pair<Frame::iterator, bool> r = frame.insert(std::make_pair(identifier, Entry(ARRAY, binding)));
        if (r.second) {
            // success
            return NULL;
        }
        else {
            fprintf(stderr, "error: couldn't define array `%s': map insertion failed\n", identifier.c_str());
            return binding;
        }
    }
}

// return NULL on success (SymbolTable has taken ownership of binding)
// return binding on failure (SymbolTable can't take ownership, caller retains responsibility)
basic::File *basic::SymbolTable::defineFile(const String &identifier, File *binding) {
    Frame &frame = m_frames.back();
    if (frame.count(identifier) > 0) {
        fprintf(stderr, "error: couldn't define file handle `%s': identifier already exists in current scope\n", identifier.c_str());
        return binding;
    }
    else {
        std::pair<Frame::iterator, bool> r = frame.insert(std::make_pair(identifier, Entry(FILEHANDLE, binding)));
        if (r.second) {
            // success
            return NULL;
        }
        else {
            fprintf(stderr, "error: couldn't define file handle `%s': map insertion failed\n", identifier.c_str());
            return binding;
        }
    }
}

bool basic::SymbolTable::undefine(const String &identifier, unsigned int mask) {
    if (mask == 0)  return false;

    for (std::vector<Frame>::reverse_iterator frame = m_frames.rbegin(); frame != m_frames.rend(); frame++) {
        Frame::iterator object = frame->find(identifier);
        // iterator of map derefs to a std::pair<key_T, value_T>
        if (object != frame->end() and (mask & object->second.type)) {
            Entry &entry = object->second;
            switch (entry.type) {
                case BUILTIN_FUNCTION:  break;
                case FUNCTION:          break;
                case SUBROUTINE:        break;
                case VARIANT:           delete entry.variant;   break;
                case ARRAY:             delete entry.array;     break;
                case FILEHANDLE:        delete entry.file;      break;
                default:
                    fprintf(stderr, "warning: unrecognised symbol table entry type in SymbolTable::endScope()\n");
            }
            frame->erase(object);
            return true;
        }
    }
    
    return false;
}



