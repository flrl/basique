/*
 *  array.cpp
 *  Untitled
 *
 *  Created by Ellie on 16/01/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "array.h"

Basic::Array::Array(const std::vector<DimensionSpecification> &dimspecs) {
    size_t total_size = 1;
    _dimensions.reserve(dimspecs.size());
    for (std::vector<DimensionSpecification>::const_iterator ds = dimspecs.begin(); ds != dimspecs.end(); ds++) {
        if (ds->first >= ds->second)  throw BadDimensionSpecification();
        total_size *= (1 + ds->second - ds->first);
        _dimensions.push_back(std::make_pair(ds->first, ds->second));
    }
    _data.resize(total_size);
}

Basic::Array::~Array() {
    ;
}

bool Basic::Array::isValidIndex(const Index &index) const {
    if (index.size() != _dimensions.size())  return false;

    for (int d = 0; d < _dimensions.size(); d++) {
        if (_dimensions[d].first > index[d])  return false;
        if (_dimensions[d].second < index[d])  return false;
    }
    
    return true;
}

//address(mat[i][j]) = address(mat[0][0]) +
//                     i * sizeof(row)    +
//                     j * sizeof(T)

//address(mat[i][j][k]) =   address(mat[0][0][0])   +
//                          i * sizeof(j0..jn)      +
//                          j * sizeof(k0..kn)      +
//                          k * sizeof(T)

const Variant& Basic::Array::itemAt(const Index &index) const {
    if (index.size() != _dimensions.size())  throw IndexOutOfBounds();
    size_t actual_index = 0;
    size_t size_of_last_dimension = 1;
    for (int d = index.size() - 1; d >= 0; d--) {
        if (_dimensions[d].first > index[d])  throw IndexOutOfBounds();
        if (_dimensions[d].second < index[d])  throw IndexOutOfBounds();
        actual_index += (_dimensions[d].first + index[d]) * size_of_last_dimension;
        size_of_last_dimension *= 1 + _dimensions[d].second - _dimensions[d].first;
    }
    if (actual_index < 0 || actual_index >= _data.size())  throw IndexOutOfBounds();
    return _data[actual_index];    
}
