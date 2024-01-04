#pragma once

#include <util/types.hpp>

namespace pathtrace {

struct Ray {
    Vector3 origin;
    Vector3 dir;
};

}