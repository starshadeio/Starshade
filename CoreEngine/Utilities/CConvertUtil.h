//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Engine
//
// File: Utilities/CConvertUtil.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CCONVERTUTIL_H
#define CCONVERTUTIL_H

#include "../Globals/CGlobals.h"
#include "../Math/CMathVectorInt2.h"
#include "../Math/CMathVectorInt3.h"
#include "../Math/CMathVectorInt4.h"
#include "../Math/CMathVector2.h"
#include "../Math/CMathVector3.h"
#include "../Math/CMathVector4.h"
#include "../Math/CMathColor.h"
#include <string>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <cwctype>
#include <cctype>
#include <algorithm>

namespace Util
{
	//-----------------------------------------------------------------------------------------------
	// String Tests
	//-----------------------------------------------------------------------------------------------
	
	inline bool IsLong(const std::string& str, long* pLong = nullptr)
	{
		std::istringstream iss(str);
		long l;
		iss >> std::noskipws >> l;

		if(iss.eof() && !iss.fail())
		{
			if(pLong) *pLong = l;
			return true;
		}
		else
		{
			return false;
		}
	}
	
	inline bool IsFloat(const std::string& str, float* pFloat = nullptr)
	{
		std::istringstream iss(str);
		float f;
		iss >> std::noskipws >> f;
		
		if(iss.eof() && !iss.fail())
		{
			if(pFloat) *pFloat = f;
			return true;
		}
		else
		{
			if(!iss.eof())
			{
				char c;
				iss >> std::noskipws >> c;
				if(std::tolower(c) == L'f' && iss.tellg() == str.size())
				{
					if(pFloat) *pFloat = f;
					return true;
				}
			}

			return false;
		}
	}
	
	inline bool IsBool(std::string str, bool* pBool = nullptr)
	{
		static const std::unordered_map<std::string, bool> MAP = {
			{ "true", true },
			{ "false", false },
		};

		std::transform(str.begin(), str.end(), str.begin(), std::tolower);

		const auto& elem = MAP.find(str);
		if(elem != MAP.end())
		{
			if(pBool) *pBool = elem->second;
			return true;
		}
		else
		{
			return false;
		}
	}

	//-----------------------------------------------------------------------------------------------
	// WString Tests
	//-----------------------------------------------------------------------------------------------
	
	inline bool IsLong(const std::wstring& str, long* pLong = nullptr)
	{
		std::wistringstream iss(str);
		long l;
		iss >> std::noskipws >> l;

		if(iss.eof() && !iss.fail())
		{
			if(pLong) *pLong = l;
			return true;
		}
		else
		{
			return false;
		}
	}
	
	inline bool IsFloat(const std::wstring& str, float* pFloat = nullptr)
	{
		std::wistringstream iss(str);
		float f;
		iss >> std::noskipws >> f;

		if(iss.eof() && !iss.fail())
		{
			if(pFloat) *pFloat = f;
			return true;
		}
		else
		{
			if(!iss.eof())
			{
				wchar_t c;
				iss >> std::noskipws >> c;
				if(std::tolower(c) == L'f' && iss.tellg() == str.size())
				{
					if(pFloat) *pFloat = f;
					return true;
				}
			}

			return false;
		}
	}
	
	inline bool IsBool(std::wstring str, bool* pBool = nullptr)
	{
		static const std::unordered_map<std::wstring, bool> MAP = {
			{ L"true", true },
			{ L"false", false },
		};

		std::transform(str.begin(), str.end(), str.begin(), std::tolower);

		const auto& elem = MAP.find(str);
		if(elem != MAP.end())
		{
			if(pBool) *pBool = elem->second;
			return true;
		}
		else
		{
			return false;
		}
	}

	//-----------------------------------------------------------------------------------------------
	// String
	//-----------------------------------------------------------------------------------------------

	inline u32 SplitString(const std::string& str, char splitter, std::function<void(u32, const std::string&)> func)
	{
		size_t start = 0;
		while(start < str.length() && str[start] <= 0x20)
		{
			++start;
		}

		u32 index = 0;
		for(size_t r = start, l = start; r <= str.length(); ++r)
		{
			if(r == str.length() || str[r] == splitter)
			{
				size_t len = r - l; // Trim right.
				while(len && str[l + len - 1] <= 0x20)
				{
					--len;
				}

				if(len)
				{ // Call.
					func(index, str.substr(l, len));
				}

				l = ++r; // Trim left.
				while(l < str.length() && str[l] <= 0x20)
				{
					l = r = l + 1;
				}

				++index;
			}
		}

		return index;
	}

	inline bool StringToBool(const std::string& str)
	{
		static const std::unordered_map<std::string, bool> MAP = {
			{ "true", true },
			{ "True", true },
			{ "TRUE", true },
			{ "false", false },
			{ "False", false },
			{ "FALSE", false },
		};

		return MAP.find(str)->second;
	}
	
	//
	// Scalar.
	//

	inline long StringToLong(const std::string& str)
	{
		return std::strtol(str.c_str(), nullptr, 10);
	}

	inline float StringToFloat(const std::string& str)
	{
		return std::strtof(str.c_str(), nullptr);
	}

	//
	// VectorInt.
	//

	template<typename VecInt>
	inline VecInt StringToVectorInt(const std::string& str)
	{
		VecInt res;
		size_t index = 0;

		const char* p = str.c_str();
		char* end;
		for(long i = std::strtol(p, &end, 10); *end || p != end; i = std::strtol(p, &end, 10))
		{
			if(p == end) { p = ++end; continue; }

			res[index++] = static_cast<int>(i);
			p = end;
		}
		
		return res;
	}
	
	//
	// Vector.
	//

	template<typename Vec>
	inline Vec StringToVector(const std::string& str)
	{
		Vec res;
		size_t index = 0;

		const char* p = str.c_str();
		char* end;
		for(float f = std::strtof(p, &end); *end || p != end; f = std::strtof(p, &end))
		{
			if(p == end) { p = ++end; continue; }

			res[index++] = f;
			p = end;
		}

		return res;
	}

	//-----------------------------------------------------------------------------------------------
	// WString
	//-----------------------------------------------------------------------------------------------

	inline u32 SplitString(const std::wstring& str, wchar_t splitter, std::function<void(u32, const std::wstring&)> func)
	{
		size_t start = 0;
		while(start < str.length() && str[start] <= 0x20)
		{
			++start;
		}

		u32 index = 0;
		for(size_t r = start, l = start; r <= str.length(); ++r)
		{
			if(r == str.length() || str[r] == splitter)
			{
				size_t len = r - l; // Trim right.
				while(len && str[l + len - 1] <= 0x20)
				{
					--len;
				}

				if(len)
				{ // Call.
					func(index, str.substr(l, len));
				}

				l = ++r; // Trim left.
				while(l < str.length() && str[l] <= 0x20)
				{
					l = r = l + 1;
				}

				++index;
			}
		}

		return index;
	}

	inline bool StringToBool(const std::wstring& str)
	{
		static const std::unordered_map<std::wstring, bool> MAP = {
			{ L"true", true },
			{ L"True", true },
			{ L"TRUE", true },
			{ L"false", false },
			{ L"False", false },
			{ L"FALSE", false },
		};

		return MAP.find(str)->second;
	}

	//
	// Scalar.
	//

	inline long StringToLong(const std::wstring& str)
	{
		return std::wcstol(str.c_str(), nullptr, 10);
	}

	inline float StringToFloat(const std::wstring& str)
	{
		return std::wcstof(str.c_str(), nullptr);
	}

	//
	// VectorInt.
	//

	template<typename VecInt>
	inline VecInt StringToVectorInt(const std::wstring& str)
	{
		VecInt res;
		size_t index = 0;

		const wchar_t* p = str.c_str();
		wchar_t* end;
		for(long i = std::wcstol(p, &end, 10); *end || p != end; i = std::wcstol(p, &end, 10))
		{
			if(p == end) { p = ++end; continue; }

			res[index++] = static_cast<int>(i);
			p = end;
		}

		return res;
	}

	//
	// Vector.
	//
	
	template<typename Vec>
	inline Vec StringToVector(const std::wstring& str)
	{
		Vec res;
		size_t index = 0;

		const wchar_t* p = str.c_str();
		wchar_t* end;
		for(float f = std::wcstof(p, &end); *end || p != end; f = std::wcstof(p, &end))
		{
			if(p == end) { p = ++end; continue; }

			res[index++] = f;
			p = end;
		}

		return res;
	}
};

#endif
