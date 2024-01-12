#pragma once

#include <util/types.hpp>

namespace pathtrace {

// geometry primitive
// interface
class IPrimitive {
public:
    virtual ScalarType get_area() const = 0;
    virtual Vector3 get_centroid() const = 0;
    // uniform_sample_point

    // C++ interface routine (public virtual destructor, or protected non-virtual destructor)
    virtual ~IPrimitive() {}
};

}
