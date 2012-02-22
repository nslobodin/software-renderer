#ifndef COMMON_MATH_H
#define COMMON_MATH_H

#include "comm_pch.h"

namespace math
{

const double EPSILON_E1 = 1E-1;
const double EPSILON_E3 = 1E-3;
const double EPSILON_E4 = 1E-4;
const double EPSILON_E5 = 1E-5;
const double EPSILON_E6 = 1E-6;

template <class T> inline T Max(T x, T y) { return (x > y) ? x : y; }
template <class T> inline T Min(T x, T y) { return (x < y) ? x : y; }

#ifdef PI
#undef PI
#endif
const double PI = 3.1415926535;

inline double DegToRad(const double ang) { return ang * (PI / 180.0); }
inline double RadToDeg(const double rad) { return rad * (180.0 / PI); }

}

#endif // COMMON_MATH_H
