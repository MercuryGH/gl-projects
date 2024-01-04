#pragma once

#include <util/types.hpp>

namespace pathtrace {

// geometry primitive
// interface
class IPrimitive {
public:
    virtual ScalarType get_area() const = 0;
    virtual ScalarType get_centroid() const = 0;
    // uniform_sample_point
};

}