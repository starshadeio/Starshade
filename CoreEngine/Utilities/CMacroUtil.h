//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Engine
//
// File: Utilities/CMacroUtil.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CMACROUTIL_H
#define CMACROUTIL_H

#include "../Globals/CGlobals.h"

// Enumerator flag operator generation. Based heavily on Microsoft's code for doing this very thing.
namespace Util
{
	template <size_t S>
	struct ENUMERATOR_FLAG_INTEGER_FOR_SIZE;

	template <>
	struct ENUMERATOR_FLAG_INTEGER_FOR_SIZE<1>
	{
		typedef s8 type;
	};

	template <>
	struct ENUMERATOR_FLAG_INTEGER_FOR_SIZE<2>
	{
		typedef s16 type;
	};

	template <>
	struct ENUMERATOR_FLAG_INTEGER_FOR_SIZE<4>
	{
		typedef s32 type;
	};

	template <>
	struct ENUMERATOR_FLAG_INTEGER_FOR_SIZE<8>
	{
		typedef s64 type;
	};

	template <class T>
	struct ENUMERATOR_FLAG_SIZED_INTEGER
	{
		typedef typename ENUMERATOR_FLAG_INTEGER_FOR_SIZE<sizeof(T)>::type type;
	};
};

#define DEFINE_ENUMERATOR_FLAG_OPERATORS(ENUMTYPE) \
inline constexpr ENUMTYPE operator | (ENUMTYPE a, ENUMTYPE b) throw() { return ENUMTYPE(((Util::ENUMERATOR_FLAG_SIZED_INTEGER<ENUMTYPE>::type)a) | ((Util::ENUMERATOR_FLAG_SIZED_INTEGER<ENUMTYPE>::type)b)); } \
inline ENUMTYPE &operator |= (ENUMTYPE &a, ENUMTYPE b) throw() { return (ENUMTYPE &)(((Util::ENUMERATOR_FLAG_SIZED_INTEGER<ENUMTYPE>::type &)a) |= ((Util::ENUMERATOR_FLAG_SIZED_INTEGER<ENUMTYPE>::type)b)); } \
inline constexpr ENUMTYPE operator & (ENUMTYPE a, ENUMTYPE b) throw() { return ENUMTYPE(((Util::ENUMERATOR_FLAG_SIZED_INTEGER<ENUMTYPE>::type)a) & ((Util::ENUMERATOR_FLAG_SIZED_INTEGER<ENUMTYPE>::type)b)); } \
inline ENUMTYPE &operator &= (ENUMTYPE &a, ENUMTYPE b) throw() { return (ENUMTYPE &)(((Util::ENUMERATOR_FLAG_SIZED_INTEGER<ENUMTYPE>::type &)a) &= ((Util::ENUMERATOR_FLAG_SIZED_INTEGER<ENUMTYPE>::type)b)); } \
inline constexpr ENUMTYPE operator ~ (ENUMTYPE a) throw() { return ENUMTYPE(~((Util::ENUMERATOR_FLAG_SIZED_INTEGER<ENUMTYPE>::type)a)); } \
inline constexpr ENUMTYPE operator ^ (ENUMTYPE a, ENUMTYPE b) throw() { return ENUMTYPE(((Util::ENUMERATOR_FLAG_SIZED_INTEGER<ENUMTYPE>::type)a) ^ ((Util::ENUMERATOR_FLAG_SIZED_INTEGER<ENUMTYPE>::type)b)); } \
inline ENUMTYPE &operator ^= (ENUMTYPE &a, ENUMTYPE b) throw() { return (ENUMTYPE &)(((Util::ENUMERATOR_FLAG_SIZED_INTEGER<ENUMTYPE>::type &)a) ^= ((Util::ENUMERATOR_FLAG_SIZED_INTEGER<ENUMTYPE>::type)b)); }


#endif
