//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: UI/CUIScript.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CUISCRIPT_H
#define CUISCRIPT_H

#include "CUIFrame.h"
#include <Math/CMathVectorInt2.h>
#include <Utilities/CScriptObject.h>
#include <vector>
#include <unordered_map>
#include <string>
#include <functional>
#include <limits>

class CVObject;

namespace UI
{
	class CUIScript
	{
	public:
		struct Data
		{
			std::wstring filename;
			CUIFrame* pParent;
		};

		enum class Type
		{
			Frame,
			Image,
			View,
			Grid,
			Text,
			Tooltip,
			Button,
			GridSelect,
		};

		struct Texture
		{
			std::wstring name;
			const Util::CScriptObject::PropBlock* texture = nullptr;
			Math::VectorInt2 cells = { 1, 1 };
		};

		struct Class
		{
			std::wstring name;
			Type type = Type::Frame;
			CUIFrame::Data frameData = { };
			const Util::CScriptObject::PropBlock* typeProp = nullptr;
		};

		struct Section
		{
			std::wstring name;
			std::wstring cls;

			Type type = Type::Frame;
			CUIFrame::Data frameData = { };

			size_t parentIndex = SIZE_MAX;

			class CUIElement* pUIElement = nullptr;
			
			size_t typeSize;
			const Util::CScriptObject::PropBlock* typeProp = nullptr;

			std::vector<const Util::CScriptObject::PropBlock*> children;

			CUIScript* pScript = nullptr;
		};

	public:
		CUIScript(const CVObject* pObject, class CUICanvas* pCanvas);
		~CUIScript();
		CUIScript(const CUIScript&) = delete;
		CUIScript(CUIScript&&) = delete;
		CUIScript& operator = (const CUIScript&) = delete;
		CUIScript& operator = (CUIScript&&) = delete;

		void Build();
		void DebugUpdate();
		void Release();

		static void ProcessBlock(const Util::CScriptObject::PropBlock& propBlock, std::function<void(const std::vector<std::wstring>&, const Util::CScriptObject::PropBlock&, const Util::CScriptObject::PropBlock*)> func);

		// Accessors.
		inline bool TryGetTexture(const std::wstring& key, Texture& texture) const
		{
			auto elem = m_textureMap.find(key);
			if(elem != m_textureMap.end())
			{
				texture = elem->second;
				return true;
			}

			return false;
		}

		inline const Class& GetClass(const std::wstring& key) const { return m_classMap.find(key)->second; }
		inline const Section& GetSection(size_t index) const { return m_sectionList[index]; }
		inline CUIElement* GetElement(u64 hash) const { return m_elementMap.find(hash)->second; }

		// Modifiers.
		inline void SetData(const Data& data) { m_data = data; }

	private:
		void ParseTexture(const Util::CScriptObject::PropBlock& propBlock);
		void ParseClass(const Util::CScriptObject::PropBlock& propBlock);
		
		void ParseFrame(const Util::CScriptObject::PropBlock& propBlock, size_t parentIndex);
		void ProcessFrame(const std::vector<Util::CScriptObject::PropBlock>& propBlockList, size_t sectionIndex);

		void CreateElement(Section& section);

	private:
		Data m_data;

		Util::CScriptObject m_scriptObject;

		std::unordered_map<std::wstring, Texture> m_textureMap;
		std::unordered_map<std::wstring, Class> m_classMap;
		std::unordered_map<u64, class CUIElement*> m_elementMap;

		std::vector<Section> m_sectionList;

		const CVObject* m_pObject;
		class CUICanvas* m_pCanvas;
	};
};

#endif
