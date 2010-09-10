/*
 *  array.h
 *  Untitled
 *
 *  Created by Ellie on 16/01/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _ARRAY_H

#include <exception>
#include <utility>
#include <vector>

#include "variant.h"

namespace Basic { 
    class Array;
}

class Basic::Array {
public:
    typedef std::pair<int, int> DimensionSpecification;
    typedef std::vector<int> Index;
    class BadDimensionSpecification : public std::exception { };
    class IndexOutOfBounds : public std::exception { };
    
    Array(const std::vector<DimensionSpecification> &);
    ~Array();
    
    bool isValidIndex(const Index &) const;
    const Variant &itemAt(const Index &) const;
    
    const Variant &operator[](const Index &index) const { return itemAt(index); }
    Variant &operator[](const Index &index) { return const_cast<Variant&>(itemAt(index)); }

private:
    Array();
    Array &operator=(const Array &);
    
    std::vector<DimensionSpecification> _dimensions;
    std::vector<size_t> _sizes;
    std::vector<Variant> _data;
};

#endif
