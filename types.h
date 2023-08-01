#ifndef TYPES_H_
#define TYPES_H_
#if defined FAST
using cint = int_fast32_t;
inline constexpr cint CINT_MIN = INT_FAST32_MIN;
inline constexpr cint CINT_MAX = INT_FAST32_MAX;
#endif
#if defined SMALL
using cint = int_least32_t;
inline constexpr cint CINT_MIN = INT_LEAST32_MIN;
inline constexpr cint CINT_MAX = INT_LEAST32_MAX;
#endif
#endif
