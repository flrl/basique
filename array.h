/*
 *  array.h
 *  Untitled
 *
 *  Created by Ellie on 16/01/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _ARRAY_H

#include <vector>

#include "variant.h"

namespace Basic {
    
class Array {
public:
    Array(size_t size_x, size_t size_y, VariantType type) { initialiseContents(size_x, size_y, type); }
    Array(size_t size_x, VariantType type) { initialiseContents(size_x, 1, type); }
    ~Array(void);
    
    Variant &itemAt(unsigned, unsigned); 
    Variant &itemAt(unsigned i) { return itemAt(i, 0); }
    
    Variant getItemAt(unsigned, unsigned) const;
    Variant getItemAt(unsigned i) const { return getItemAt(i, 0); }
    
    void setItemAt(unsigned, unsigned, Variant);
    void setItemAt(unsigned i, Variant v) { setItemAt(i, 0, v); }
    
private:
    size_t size_x, size_y;
    size_t allocated_size;
    VariantType content_type;
    std::vector<Variant> contents;
    
    void initialiseContents(size_t, size_t, VariantType);
    
    int getContentsIndexForPosition(unsigned x, unsigned y) const {
        if (x >= size_x || y >= size_y)  return -1;
        return y * size_x + x;
    }
};

}
#endif
