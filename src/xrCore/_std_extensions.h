#pragma once
#ifndef _STD_EXT_internal
#define _STD_EXT_internal

#include <math.h>
#include <float.h>
#include <stdio.h>

#include <string_view>

#include "xrCommon/math_funcs_inline.h"
//#include "xr_token.h"

#ifdef abs
#undef abs
#endif

#ifdef _MIN
#undef _MIN
#endif

#ifdef _MAX
#undef _MAX
#endif

#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

// generic
template <class T>
IC T _min(T a, T b)
{
    return a < b ? a : b;
}
template <class T>
IC T _max(T a, T b)
{
    return a > b ? a : b;
}
template <class T>
IC T _sqr(T a)
{
    return a * a;
}

IC bool _valid(const float x) noexcept
{
    // check for: Signaling NaN, Quiet NaN, Negative infinity ( ???INF), Positive infinity (+INF), Negative denormalized,
    // Positive denormalized
#if defined(XR_PLATFORM_WINDOWS) && defined(XR_COMPILER_MSVC)
#if defined(XR_ARCHITECTURE_X64)
    const int cls = _fpclassf(x);
#else
    const int cls = _fpclass(static_cast<double>(x));
#endif // XR_ARCHITECTURE_X64
    if (cls & (_FPCLASS_SNAN + _FPCLASS_QNAN + _FPCLASS_NINF + _FPCLASS_PINF + _FPCLASS_ND + _FPCLASS_PD))
        return false;
#else
    const int cls = std::fpclassify(x);
    switch (cls)
    {
    case FP_NAN:
    case FP_INFINITE:
    case FP_SUBNORMAL:
        return false;
    default:
        break;
    }

    /* *****other cases are*****
    _FPCLASS_NN Negative normalized non-zero
    _FPCLASS_NZ Negative zero ( ??? 0)
    _FPCLASS_PZ Positive 0 (+0)
    _FPCLASS_PN Positive normalized non-zero
    */
#endif
    return true;
}

// double
IC bool _valid(const double x)
{
    // check for: Signaling NaN, Quiet NaN, Negative infinity ( ???INF), Positive infinity (+INF), Negative denormalized,
    // Positive denormalized
#if defined(XR_PLATFORM_WINDOWS) && defined(XR_COMPILER_MSVC)
    const int cls = _fpclass(x);
    if (cls & (_FPCLASS_SNAN + _FPCLASS_QNAN + _FPCLASS_NINF + _FPCLASS_PINF + _FPCLASS_ND + _FPCLASS_PD))
        return false;
#else
    const int cls = std::fpclassify(x);
    switch (cls)
    {
    case FP_NAN:
    case FP_INFINITE:
    case FP_SUBNORMAL:
        return false;
    default:
        break;
    }
    /* *****other cases are*****
    _FPCLASS_NN Negative normalized non-zero
    _FPCLASS_NZ Negative zero ( ??? 0)
    _FPCLASS_PZ Positive 0 (+0)
    _FPCLASS_PN Positive normalized non-zero
    */
#endif
    return true;
}

// XXX: "magic" specializations, that really require profiling to see if they are worth this effort.
// int8
IC s8 _abs(s8 x) { return (x >= 0) ? x : s8(-x); }
IC s8 _min(s8 x, s8 y) { return y + ((x - y) & ((x - y) >> (sizeof(s8) * 8 - 1))); }
IC s8 _max(s8 x, s8 y) { return x - ((x - y) & ((x - y) >> (sizeof(s8) * 8 - 1))); }
// unsigned int8
IC u8 _abs(u8 x) { return x; }
// int16
IC s16 _abs(s16 x) { return (x >= 0) ? x : s16(-x); }
IC s16 _min(s16 x, s16 y) { return y + ((x - y) & ((x - y) >> (sizeof(s16) * 8 - 1))); }
IC s16 _max(s16 x, s16 y) { return x - ((x - y) & ((x - y) >> (sizeof(s16) * 8 - 1))); }
// unsigned int16
IC u16 _abs(u16 x) { return x; }
// int32
IC s32 _abs(s32 x) { return (x >= 0) ? x : s32(-x); }
IC s32 _min(s32 x, s32 y) { return y + ((x - y) & ((x - y) >> (sizeof(s32) * 8 - 1))); }
IC s32 _max(s32 x, s32 y) { return x - ((x - y) & ((x - y) >> (sizeof(s32) * 8 - 1))); }
// int64
IC s64 _abs(s64 x) { return (x >= 0) ? x : s64(-x); }
IC s64 _min(s64 x, s64 y) { return y + ((x - y) & ((x - y) >> (sizeof(s64) * 8 - 1))); }
IC s64 _max(s64 x, s64 y) { return x - ((x - y) & ((x - y) >> (sizeof(s64) * 8 - 1))); }

// string management

// return pointer to ".ext"
IC char* strext(const char* S) { return (char*)strrchr(S, '.'); }
IC size_t xr_strlen(const char* S) { return strlen(S); }

#ifndef MASTER_GOLD

inline int xr_strcpy(pstr destination, size_t const destination_size, LPCSTR source)
{
    return strcpy_s(destination, destination_size, source);
}

inline int xr_strcat(pstr destination, size_t const buffer_size, LPCSTR source)
{
    return strcat_s(destination, buffer_size, source);
}

inline int __cdecl xr_sprintf(pstr destination, size_t const buffer_size, LPCSTR format_string, ...)
{
    va_list args;
    va_start(args, format_string);
    const int result = vsprintf_s(destination, buffer_size, format_string, args);
    va_end(args);
    return result;
}

template <size_t count>
inline int __cdecl xr_sprintf(char (&destination)[count], LPCSTR format_string, ...)
{
    va_list args;
    va_start(args, format_string);
    const int result = vsprintf_s(destination, count, format_string, args);
    va_end(args);
    return result;
}
#else // #ifndef MASTER_GOLD

inline int xr_strcpy(pstr destination, size_t const destination_size, LPCSTR source)
{
    return strncpy_s(destination, destination_size, source, destination_size);
}

inline int xr_strcat(pstr destination, size_t const buffer_size, LPCSTR source)
{
    size_t const destination_length = xr_strlen(destination);
    pstr i = destination + destination_length;
    pstr const e = destination + buffer_size - 1;
    if (i > e)
        return 0;

    for (LPCSTR j = source; *j && (i != e); ++i, ++j)
        *i = *j;

    *i = 0;
    return 0;
}

inline int __cdecl xr_sprintf(pstr destination, size_t const buffer_size, LPCSTR format_string, ...)
{
    va_list args;
    va_start(args, format_string);
    const int result = vsnprintf_s(destination, buffer_size, buffer_size - 1, format_string, args);
    va_end(args);
    return result;
}

template <int count>
inline int __cdecl xr_sprintf(char (&destination)[count], LPCSTR format_string, ...)
{
    va_list args;
    va_start(args, format_string);
    const int result = vsnprintf_s(destination, count, count - 1, format_string, args);
    va_end(args);
    return result;
}
#endif // #ifndef MASTER_GOLD

template <size_t count>
inline int xr_strcpy(char(&destination)[count], LPCSTR source)
{
    return xr_strcpy(destination, count, source);
}

template <size_t count>
inline int xr_strcat(char(&destination)[count], LPCSTR source)
{
    return xr_strcat(destination, count, source);
}

inline void MemFill32(void* dst, u32 value, size_t dstSize)
{
    u32* ptr = static_cast<u32*>(dst);
    u32* end = ptr + dstSize;
    while (ptr != end)
        *ptr++ = value;
}

// source: https://stackoverflow.com/a/46711735
constexpr u32 strhash(const std::string_view data) noexcept
{
    uint32_t hash = 5385;
    for (const auto& e : data)
        hash = ((hash << 5) + hash) + e;
    return hash;
}

constexpr u32 operator""_hash(char const* p, size_t size) { return strhash({ p, size }); }

XRCORE_API char* timestamp(string64& dest);

extern XRCORE_API u32 crc32(const void* P, u32 len);
extern XRCORE_API u32 crc32(const void* P, u32 len, u32 starting_crc);
extern XRCORE_API u32 path_crc32(const char* path, u32 len); // ignores '/' and '\'

#endif // _STD_EXT_internal
