//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Engine
//
// File: Math/CMathColor.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CMathColor.h"

namespace Math
{
	//
	// Conversion member methods.
	//

	const Color HCYwts = Color(0.299f, 0.587f, 0.114f, 0.0f);

	const float HCLgamma = 3.0f;
	const float HCLy0 = 100.0f;
	const float HCLmaxL = 0.530454533953517f; // == exp(HCLgamma / HCLy0) - 0.5f

	void Color::CreateFromHUE(float hue)
	{
		*this = ColorFromHUE(hue);
	}

	void Color::CreateFromHSV(const Color& hsv)
	{
		*this = ColorFromHSV(hsv);
	}

	void Color::CreateFromHSL(const Color& hsl)
	{
		*this = ColorFromHSL(hsl);
	}

	void Color::CreateFromHCY(const Color& hcy)
	{
		*this = ColorFromHCY(hcy);
	}

	void Color::CreateFromHCL(const Color& hcl)
	{
		*this = ColorFromHCL(hcl);
	}

	Color Color::ToHCV()
	{
		// Based on work by Sam Hocevar and Emil Persson
		Color P = (g < b) ? Color(b, g, -1.0f, Math::g_1Over3 * 2.0f) : Color(g, b, 0.0f, -Math::g_1Over3);
		Color Q = (r < P.x) ? Color(P.x, P.y, P.w, r) : Color(r, P.y, P.z, P.x);
		float C = Q.x - std::min(Q.w, Q.y);
		float H = fabsf((Q.w - Q.y) / (6.0f * C + Math::g_Epsilon) + Q.z);

		return Color(H, C, Q.x, a);
	}

	Color Color::ToHSV()
	{
		Color hcv = ToHCV();
		float S = hcv.y / (hcv.z + Math::g_Epsilon);

		return Color(hcv.x, S, hcv.z, a);
	}

	Color Color::ToHSL()
	{
		Color hcv = ToHCV();
		float L = hcv.z - hcv.y * 0.5f;
		float S = hcv.y / (1.0f - fabsf(L * 2.0f - 1.0f) + Math::g_Epsilon);

		return Color(hcv.x, S, L, a);
	}

	Color Color::ToHCY()
	{
		Color hcv = ToHCV();
		float Y = r * HCYwts.x + g * HCYwts.y + b * HCYwts.z;
		float Z = Dot(ColorFromHUE(hcv.x), HCYwts);

		if(Y < Z)
		{
			hcv.y *= Z / (Math::g_Epsilon + Y);
		}
		else
		{
			hcv.y *= (1.0f - Z) / (Math::g_Epsilon + 1.0f - Y);
		}
		
		return Color(hcv.x, hcv.y, Y, a);
	}

	Color Color::ToHCL()
	{
		Color HCL;
		float H = 0.0f;
		float U = std::min(r, std::min(g, b));
		float V = std::max(r, std::max(g, b));
		float Q = HCLgamma / HCLy0;
		HCL.y = V - U;

		if(HCL.y != 0.0f)
		{
			H = atan2f(g - b, r - g) / Math::g_Pi;
			Q *= U / V;
		}
		
		Q = expf(Q);
		HCL.x = Math::Frac(H * 0.5f - std::min(Math::Frac(H), Math::Frac(-H)) * Math::g_1Over6);
		HCL.y *= Q;
		HCL.z = Math::Lerp(-U, V, Q) / (HCLmaxL * 2.0f);
		HCL.a = a;

		return HCL;
	}

	//
	// Conversion static methods.
	//

	Color Color::ColorFromHUE(float hue)
	{
		Color c;
		c.r = Math::Saturate(fabsf(hue * 6.0f - 3.0f) - 1.0f);
		c.g = Math::Saturate(2.0f - fabsf(hue * 6.0f - 2.0f));
		c.b = Math::Saturate(2.0f - fabsf(hue * 6.0f - 4.0f));
		c.a = 1.0f;

		return c;
	}

	Color Color::ColorFromHSV(const Color& hsv)
	{
		Color rgb = ColorFromHUE(hsv.x);
		rgb = ((rgb - 1.0f) * hsv.y + 1.0f) * hsv.z;

		return Color(rgb.r, rgb.g, rgb.b, hsv.a);
	}

	Color Color::ColorFromHSL(const Color& hsl)
	{
		Color rgb = ColorFromHUE(hsl.x);
		float C = (1.0f - fabsf(2.0f * hsl.z - 1.0f)) * hsl.y;
		rgb = (rgb - 0.5f) * C + hsl.z;

		return Color(rgb.r, rgb.g, rgb.b, hsl.a);
	}

	Color Color::ColorFromHCY(const Color& hcy)
	{
		Color rgb = ColorFromHUE(hcy.x);
		float Z = Color::Dot(rgb, HCYwts);
		float Y = hcy.y;

		if(hcy.z < Z)
		{
			Y = hcy.y * (hcy.z / Z);
		}
		else if(Z < 1.0f)
		{
			Y = (1.0f - hcy.z) / (1.0f - Z);
		}
		
		rgb = (rgb - Z) * Y + hcy.z;
		return Color(rgb.r, rgb.g, rgb.b, hcy.a);
	}

	Color Color::ColorFromHCL(const Color& hcl)
	{
		Color rgb;
		if(hcl.z != 0.0f)
		{
			float H = hcl.x;
			float C = hcl.y;
			float L = hcl.z * HCLmaxL;
			float Q = expf((1.0f - C / (2.0f * L)) * (HCLgamma / HCLy0));
			float U = (2.0f * L - C) / (2.0f * Q - 1.0f);
			float V = C / Q;
			float T = tanf((H + std::min(Math::Frac(2.0f * H) * 0.25f, Math::Frac(-2.0f * H) * 0.125f)) * Math::g_2Pi);

			H *= 6.0f;
			if(H <= 1.0f)
			{
				rgb.x = 1.0f;
				rgb.y = T / (1.0f + T);
			}
			else if(H <= 2.0f)
			{
				rgb.x = (1.0f + T) / T;
				rgb.y = 1.0f;
			}
			else if(H <= 3.0f)
			{
				rgb.y = 1.0f;
				rgb.z = 1.0f + T;
			}
			else if(H <= 4.0f)
			{
				rgb.y = 1.0f / (1.0f + T);
				rgb.z = 1.0f;
			}
			else if(H <= 5.0f)
			{
				rgb.x = -1.0f / T;
				rgb.z = 1.0f;
			}
			else
			{
				rgb.x = 1.0f;
				rgb.z = -T;
			}

			rgb = rgb * V + U;
		}
		
		return Color(rgb.r, rgb.g, rgb.b, hcl.a);
	}

	Color Color::ToHCV(const Color& c)
	{
		// Based on work by Sam Hocevar and Emil Persson
		Color P = (c.g < c.b) ? Color(c.b, c.g, -1.0f, Math::g_1Over3 * 2.0f) : Color(c.g, c.b, 0.0f, -Math::g_1Over3);
		Color Q = (c.r < P.x) ? Color(P.x, P.y, P.w, c.r) : Color(c.r, P.y, P.z, P.x);
		float C = Q.x - std::min(Q.w, Q.y);
		float H = fabsf((Q.w - Q.y) / (6.0f * C + Math::g_Epsilon) + Q.z);
		return Color(H, C, Q.x, c.a);
	}

	Color Color::ToHSV(const Color& c)
	{
		Color hcv = ToHCV(c);
		float S = hcv.y / (hcv.z + Math::g_Epsilon);
		return Color(hcv.x, S, hcv.z, c.a);
	}

	Color Color::ToHSL(const Color& c)
	{
		Color hcv = ToHCV(c);
		float L = hcv.z - hcv.y * 0.5f;
		float S = hcv.y / (1.0f - fabsf(L * 2.0f - 1.0f) + Math::g_Epsilon);
		return Color(hcv.x, S, L, c.a);
	}

	Color Color::ToHCY(const Color& c)
	{
		Color hcv = ToHCV(c);
		float Y = c.r * HCYwts.x + c.g * HCYwts.y + c.b * HCYwts.z;
		float Z = Color::Dot(ColorFromHUE(hcv.x), HCYwts);

		if(Y < Z)
		{
			hcv.y *= Z / (Math::g_Epsilon + Y);
		}
		else
		{
			hcv.y *= (1.0f - Z) / (Math::g_Epsilon + 1.0f - Y);
		}
		
		return Color(hcv.x, hcv.y, Y, c.a);
	}

	Color Color::ToHCL(const Color& c)
	{
		Color HCL;
		float H = 0.0f;
		float U = std::min(c.r, std::min(c.g, c.b));
		float V = std::max(c.r, std::max(c.g, c.b));
		float Q = HCLgamma / HCLy0;
		HCL.y = V - U;

		if(HCL.y != 0.0f)
		{
			H = atan2f(c.g - c.b, c.r - c.g) / Math::g_Pi;
			Q *= U / V;
		}
		
		Q = expf(Q);
		HCL.x = Math::Frac(H * 0.5f - std::min(Math::Frac(H), Math::Frac(-H)) * Math::g_1Over6);
		HCL.y *= Q;
		HCL.z = Math::Lerp(-U, V, Q) / (HCLmaxL * 2.0f);
		HCL.a = c.a;
		return HCL;
	}

	Color Color::ToGamma(const Color& c)
	{
		static const float inv2_2 = 1.0f / 2.2f;
		return Color(powf(c.r, inv2_2), powf(c.g, inv2_2), powf(c.b, inv2_2), c.a);
	}

	Color Color::ToLinear(const Color& c)
	{
		return Color(powf(c.r, 2.2f), powf(c.g, 2.2f), powf(c.b, 2.2f), c.a);
	}
};
