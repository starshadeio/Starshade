//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Engine
//
// File: Application/CLocalizedString.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CLocalizedString.h"
#include <cassert>

namespace App
{
	CLocalizedString::CLocalizedString(const std::wstring& str, const std::wstring& split)
	{
		ParseString(str, split);
	}

	CLocalizedString::~CLocalizedString()
	{
	}

	std::wstring CLocalizedString::Format(std::initializer_list<std::wstring> valList) const
	{
		std::wstring res;
		size_t index = 0;
		for(auto& elem : valList)
		{
			res += m_strList[index++] + elem;
		}

		while(index < m_strList.size())
		{
			res += m_strList[index++];
		}

		return res;
	}

	void CLocalizedString::ParseString(const std::wstring& str, const std::wstring& split)
	{
		if(!split.empty())
		{
			assert(split.size() == 1);
			m_strList.clear();

			size_t l, r;
			for(l = 0, r = 0; r <= str.size(); ++r)
			{
				if(r == str.size() || str[r] == split[0])
				{
					if(r < str.size() || (r == str.size() && l < r))
					{
						m_strList.push_back(str.substr(l, r - l));
					}

					l = r + 1;
				}
			}
		}
		else
		{
			m_strList = { str };
		}
	}
};
