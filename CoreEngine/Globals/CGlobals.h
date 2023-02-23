//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Engine
//
// File: Globals/CGlobals.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CGLOBALS_H
#define CGLOBALS_H

#include <cstdint>
#include <mmintrin.h>
#include <xmmintrin.h>
#include <emmintrin.h>
#include <pmmintrin.h>
#include <tmmintrin.h>
#include <smmintrin.h>
#include <atomic>

// Global type defines.
typedef std::int8_t s8;
typedef std::int16_t s16;
typedef std::int32_t s32;
typedef std::int64_t s64;

typedef std::uint8_t u8;
typedef std::uint16_t u16;
typedef std::uint32_t u32;
typedef std::uint64_t u64;

typedef __m128 vf32;

// Global type defines atmoic.
typedef std::atomic_int8_t As8;
typedef std::atomic_int16_t As16;
typedef std::atomic_int32_t As32;
typedef std::atomic_int64_t As64;

typedef std::atomic_uint8_t Au8;
typedef std::atomic_uint16_t Au16;
typedef std::atomic_uint32_t Au32;
typedef std::atomic_uint64_t Au64;

typedef std::atomic_bool Abool;
typedef std::atomic_char Achar;
typedef std::atomic_flag Aflag;

#endif
