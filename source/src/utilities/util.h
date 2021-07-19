#ifndef UVENT_UTIL_H
#define UVENT_UTIL_H

#include <Arduino.h>
#ifdef __cplusplus

// Undef the Arduino implementation
#undef min

template<typename Tpa, typename Tpb>
constexpr auto min(const Tpa& a, const Tpb& b) -> decltype(a < b ? a : b)
{
    return b < a ? b : a;
}
#endif// __cplusplus

/**
 * @param ptr The pointer to the field/var keeping the time
 * @param n The amount of millis required to elapse
 * @return True if n millis have passed, else false.
 */
bool has_time_elapsed(uint32_t* ptr, uint32_t n);

// Returns the current time in seconds
inline float now_s() { return millis() * 1e-3; }

#endif//UVENT_UTIL_H
