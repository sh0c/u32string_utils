#pragma once

#include <cmath>
#include <limits>


template<typename T, typename U>
bool equal(T a, U b)
{
    return fabs(a - b) < std::numeric_limits<T>::epsilon();
}
