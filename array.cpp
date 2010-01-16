/*
 *  array.cpp
 *  Untitled
 *
 *  Created by Daniel on 16/01/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "array.h"

using Basic::Array;

Array::~Array(void) {
    contents.clear();
}

Variant &Array::itemAt(unsigned x, unsigned y) {
    // FIXME think about how to handle out-of-bounds
    return contents.at(getContentsIndexForPosition(x, y));
}

Variant Array::getItemAt(unsigned x, unsigned y) const {
    // FIXME think about how to handle out-of-bounds
    return contents.at(getContentsIndexForPosition(x, y));
}

void Array::setItemAt(unsigned x, unsigned y, Variant v) {
    // FIXME think about how to handle out-of-bounds
    contents.at(getContentsIndexForPosition(x, y)) = v;
}

void Array::initialiseContents(size_t x, size_t y, VariantType t) {
    this->size_x = x;
    this->size_y = y;
    allocated_size = x * y;
    contents.clear();
    contents.resize(allocated_size, Variant(t));
}
