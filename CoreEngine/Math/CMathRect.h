//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Engine
//
// File: Math/CMathRect.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CMATHRECT_H
#define CMATHRECT_H

namespace Math
{
	struct Rect
	{
		long x, y, w, h;

		Rect() { }
		Rect(long X, long Y, long W, long H) : x(X), y(Y), w(W), h(H) { }
	};

	struct RectLTRB
	{
		long left, top, right, bottom;

		RectLTRB() { }
		RectLTRB(long Left, long Top, long Right, long Bottom) : left(Left), top(Top), right(Right), bottom(Bottom) { }
	};

	struct RectFlt
	{
		float left, top, right, bottom;

		RectFlt() { }
		RectFlt(float Left, float Top, float Right, float Bottom) : left(Left), top(Top), right(Right), bottom(Bottom) { }
	};
};

#endif
