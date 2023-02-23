//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Engine
//
// File: Utilities/CCompilerUtil.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CCOMPILERUTIL_H
#define CCOMPILERUTIL_H

#include <string>
#include <functional>

namespace Util
{
	// Structure for storing the string for a temporary word before it is moved elsewhere.
	template<typename STR, typename CH>
	struct CompilerWord
	{
	private:
		STR word;

	public:
		inline void AddChar(CH ch) { word.push_back(ch); }
		inline void AddString(const STR& str) { word += str; }
		inline void Pop() { word.pop_back(); }
		inline CH Peek() const { return word.back(); }

		void Move(STR& target)
		{
			target = std::move(word);
		}

		void ForEach(std::function<void(CH)> func)
		{
			for(CH ch : word)
			{
				func(ch);
			}
		}

		inline bool Empty() const { return word.empty(); }
		inline void Clear() { word.clear(); }

		void Trim()
		{
			int offset = 0;
			for(; offset < (int)word.size(); ++offset)
			{
				if(word[offset] > 0x20) { break; }
			}

			int len = (int)word.size() - 1;
			for(; len >= 0; --len)
			{
				if(word[len] > 0x20) { break; }
			}

			if(offset < len)
			{
				word = word.substr(offset, len - offset + 1);
			}
		}
	};

	// Tuple structure for storing a group of related words.
	template<size_t K, typename STR, typename CH>
	struct CompilerTuple
	{
	private:
		STR elem[K];
		size_t target;

	public:
		CompilerTuple()
		{
			Clear();
		}

		bool Continue()
		{
			if(target == K - 1) { return false; }
			target++;
			return true;
		}

		void Clear()
		{
			for(size_t i = 0; i < K; ++i)
			{
				elem[i].clear();
			} target = 0;
		}

		inline void SetTarget(const STR& word) { elem[target] = word; }
		inline void SetTarget(Util::CompilerWord<STR, CH>& word) { word.Move(elem[target]); }

		bool IsComplete() const
		{
			for(size_t i = 0; i < K; ++i)
			{
				if(elem[i].empty()) { return false; }
			}

			return true;
		}

		inline const STR& GetElement(size_t i) const { return elem[i]; }
		inline size_t GetTargetIndex() const { return target; }
	};
};

#endif
