//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Engine
//
// File: Utilities/CMemAlign.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CMEMALIGN_H
#define CMEMALIGN_H

#include <memory>

template<size_t ALIGNMENT>
class CMemAlign
{
public:
	void* operator new(size_t i) { return _mm_malloc(i, ALIGNMENT); }
	void* operator new[](size_t i) { return _mm_malloc(i, ALIGNMENT); }
	void operator delete(void* p) { _mm_free(p); }
	void operator delete[](void* p) { _mm_free(p); }
};

#endif
