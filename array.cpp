/*
 *  array.cpp
 *  Untitled
 *
 *  Created by Ellie on 16/01/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include <cassert>

#include "array.h"

basic::Array::Array(const std::vector<DimensionSpecification> &dimspecs) {
    size_t total_size = 1;
    m_dimensions.reserve(dimspecs.size());
    if (dimspecs.size() == 0)  throw BadDimensionSpecification();
    for (std::vector<DimensionSpecification>::const_iterator ds = dimspecs.begin(); ds != dimspecs.end(); ds++) {
        if (ds->first > ds->second)  throw BadDimensionSpecification();
        size_t size = (1 + ds->second - ds->first);
        m_sizes.push_back(size);
        total_size *= size;
        m_dimensions.push_back(std::make_pair(ds->first, ds->second));
    }
    assert(total_size > 0);
    m_data.resize(total_size);
}

basic::Array::~Array() {
    ;
}

bool basic::Array::isValidIndex(const Index &index) const {
    if (index.size() != m_dimensions.size())  return false;

    for (size_t d = 0; d < m_dimensions.size(); d++) {
        if (m_dimensions[d].first > index[d])  return false;
        if (m_dimensions[d].second < index[d])  return false;
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

const basic::Variant& basic::Array::itemAt(const Index &index) const {
    if (index.size() != m_dimensions.size())  throw IndexOutOfBounds();
    size_t actual_index = 0;
    size_t size_of_last_dimension = 1;
    for (int d = index.size() - 1; d >= 0; d--) {
        if (m_dimensions[d].first > index[d])  throw IndexOutOfBounds();
        if (m_dimensions[d].second < index[d])  throw IndexOutOfBounds();
        actual_index += (index[d] - m_dimensions[d].first) * size_of_last_dimension;
        size_of_last_dimension *= m_sizes[d];
    }
    assert(actual_index >= 0);
    assert(actual_index < m_data.size());
    return m_data[actual_index];    
}
