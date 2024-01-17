#pragma once

#include <util/types.hpp>

namespace pathtrace {

// Make sure the direction MUST be normalized. Constructor must be used.
struct Ray {
    Ray(Vector3 origin, Vector3 dir): origin(origin), dir(glm::normalize(dir)) {}
    Vector3 origin;
    Vector3 dir;
};

}
