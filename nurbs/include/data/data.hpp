#pragma once

#include <tinynurbs.h>

#include <util/types.hpp>

namespace nurbs {

using namespace tinynurbs;

void assign_data_11(std::vector<RationalCurve<ScalarType>> &curves);
void assign_data_12(std::vector<RationalCurve<ScalarType>> &curves);
void assign_data_21(std::vector<RationalCurve<ScalarType>> &curves);
void assign_data_22(std::vector<RationalCurve<ScalarType>> &curves);

}