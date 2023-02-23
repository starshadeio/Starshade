//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: UI/CUIText.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CUIText.h"
#include "CUIFrame.h"
#include "CFont.h"
#include "../Graphics/CGraphicsAPI.h"
#include "../Resources/CResourceManager.h"
#include "../Factory/CFactory.h"
#include <Math/CMathFNV.h>
#include <Utilities/CConvertUtil.h>

namespace UI
{
	CUIText::CUIText(const CVObject* pObject, class CUICanvas* pCanvas, const CUIScript::Section* pSection) : 
		CUIRenderer(pObject, pCanvas, pSection),
		m_bUpdateText(true),
		m_pFont(nullptr),
		m_pGraphics(nullptr)
	{
	}

	CUIText::~CUIText()
	{
	}
	
	void CUIText::Initialize()
	{
		CUIRenderer::Initialize();

		m_pFont = reinterpret_cast<CFont*>(Resources::CManager::Instance().GetResource(Resources::RESOURCE_TYPE_FONT, m_data.fontHash));
		m_pGraphics = CFactory::Instance().GetGraphicsAPI();
	}

	void CUIText::DrawSetup()
	{
		CUIRenderer::DrawSetup();

		if(!IsActive())
		{
			return;
		}
		
		UpdateMetrics();
		CalculateStart();
		BuildString();
	}

	void CUIText::BuildString()
	{
		u32 instanceIndex = 0;
		m_offset = 0.0f;

		for(size_t i = 0; i < m_data.text.size(); ++i)
		{
			const auto& glyph = m_pFont->GetGlyph(m_data.text[i]);
			if(!IsValidGlyph(glyph))
			{
				continue;
			}
		
			if(glyph.uv[2] == 0.0f || glyph.uv[3] == 0.0f)
			{
				m_offset.x += float(glyph.hAdvance) * m_metricScale * m_data.spacingScale;
				continue;
			}
			else
			{
				CUIQuad::Instance& instance = *PullInstance(instanceIndex++);

				instance.color = m_data.color;
				instance.texCoord = Math::Vector4(
					glyph.uv[0], 1.0f - (glyph.uv[1] + glyph.uv[3]), glyph.uv[2], glyph.uv[3]
				);
		
				m_offset.x += float(glyph.hBearingX) * m_metricScale * m_data.spacingScale;
				Math::Vector2 vertical = Math::VEC2_DOWN * float(glyph.height - glyph.hBearingY) * m_metricScale;

				Math::Matrix3x2 mtx = Math::MTX3X2_IDENTITY;
				mtx = Math::Matrix3x2::Scale(Math::Vector2(float(glyph.width), float(glyph.height)) * m_metricScale + m_padding) *
					Math::Matrix3x2::Translate(m_position + m_offset + vertical);
		
				m_offset.x += float(glyph.hAdvance - glyph.hBearingX) * m_metricScale * m_data.spacingScale;

				instance.world = mtx * GetFrame()->GetInternalMatrix();
			}
		}
	}
	
	//-----------------------------------------------------------------------------------------------
	// CUICommand methods.
	//-----------------------------------------------------------------------------------------------
	
	static const std::unordered_map<std::wstring, CUIText::HAlign> HALIGN_MAP {
		{ L"Left", CUIText::HAlign::Left },
		{ L"Center", CUIText::HAlign::Center },
		{ L"Right", CUIText::HAlign::Right },
	};
		
	static const std::unordered_map<std::wstring, CUIText::VAlign> VALIGN_MAP {
		{ L"Top", CUIText::VAlign::Top },
		{ L"Center", CUIText::VAlign::Center },
		{ L"Bottom", CUIText::VAlign::Bottom },
	};
		
	bool CUIText::CommandElement(const CUICommandList::StateElem& stateElem)
	{
		if(CUIRenderer::CommandElement(stateElem))
		{
			return true;
		}

		struct MapData
		{
			const CUICommandList::StateElem& stateElem;
			CUIText* pText;
			Data& data;
		};

		typedef std::function<void(const MapData&)> SECTION_FUNC;
		static const std::unordered_map<std::wstring, SECTION_FUNC> TEXT_MAP {
			{ L"hAlign", [](const MapData& mapData) { mapData.data.hAlign = HALIGN_MAP.find(mapData.stateElem.valStr)->second; } },
			{ L"vAlign", [](const MapData& mapData) { mapData.data.vAlign = VALIGN_MAP.find(mapData.stateElem.valStr)->second; } },

			{ L"fontSize", [](const MapData& mapData) { mapData.data.fontSize = mapData.stateElem.val.f; } },
			{ L"spacingScale", [](const MapData& mapData) { mapData.data.spacingScale = mapData.stateElem.val.f; } },

			{ L"text", [](const MapData& mapData) { mapData.data.text = mapData.stateElem.valStr; mapData.pText->m_bUpdateText = true; } },
		};
		
		const auto& elem = TEXT_MAP.find(stateElem.GetSubElemAt(0));
		if(elem == TEXT_MAP.end()) return false;

		elem->second({ stateElem, this, m_data });
		MarkAsDirty();

		return true;
	}

	//-----------------------------------------------------------------------------------------------
	// Section methods.
	//-----------------------------------------------------------------------------------------------
	
	bool CUIText::SetFullDataFromKeyValue(const CUIScript* pScript, const Util::CScriptObject::PropBlock& prop, const std::wstring& key, const std::wstring& value, Data& data)
	{
		if(CUIElement::SetDataFromKeyValue(pScript, prop, key, value, data)) return true;
		if(CUIRenderer::SetDataFromKeyValue(pScript, key, value, data)) return true;
		if(CUIText::SetDataFromKeyValue(pScript, key, value, data)) return true;
		return false;
	}

	bool CUIText::SetDataFromKeyValue(const CUIScript* pScript, const std::wstring& key, const std::wstring& value, Data& data)
	{
		struct MapData
		{
			const CUIScript* pScript;
			const std::wstring& prop;
			Data& data;
		};

		typedef std::function<void(const MapData&)> SECTION_FUNC;
		static const std::unordered_map<std::wstring, SECTION_FUNC> TEXT_MAP {
			{ L"hAlign", [](const MapData& mapData) { mapData.data.hAlign = HALIGN_MAP.find(mapData.prop)->second; } },
			{ L"vAlign", [](const MapData& mapData) { mapData.data.vAlign = VALIGN_MAP.find(mapData.prop)->second; } },

			{ L"fontSize", [](const MapData& mapData) { mapData.data.fontSize = Util::StringToFloat(mapData.prop); } },
			{ L"spacingScale", [](const MapData& mapData) { mapData.data.spacingScale = Util::StringToFloat(mapData.prop); } },

			{ L"maxText", [](const MapData& mapData) { mapData.data.maxText = Util::StringToLong(mapData.prop); } },
			{ L"text", [](const MapData& mapData) { mapData.data.text = mapData.prop; } },

			{ L"material", [](const MapData& mapData) { mapData.data.matHash = Math::FNV1a_64(mapData.prop.c_str()); } },
			{ L"font", [](const MapData& mapData) { mapData.data.fontHash = Math::FNV1a_64(mapData.prop.c_str()); } },
		};

		const auto& elem = TEXT_MAP.find(key);
		if(elem == TEXT_MAP.end()) return false;

		elem->second({ pScript, value, data });
		return true;
	}
		
	void CUIText::BuildFromSection(const CUIScript::Section* pSection)
	{
		CUIRenderer::BuildFromSection(pSection);
		
		if(pSection->cls.size())
		{
			CUIScript::ProcessBlock(*pSection->pScript->GetClass(pSection->cls).typeProp, [&](const std::vector<std::wstring>& str, const Util::CScriptObject::PropBlock& prop, const Util::CScriptObject::PropBlock* pParent){
				SetDataFromKeyValue(pSection->pScript, str[0], prop.propList[1], m_data);
			});
		}

		CUIScript::ProcessBlock(*pSection->typeProp, [&](const std::vector<std::wstring>& str, const Util::CScriptObject::PropBlock& prop, const Util::CScriptObject::PropBlock* pParent){
			SetDataFromKeyValue(pSection->pScript, str[0], prop.propList[1], m_data);
		});
	}

	//-----------------------------------------------------------------------------------------------
	// Utility methods.
	//-----------------------------------------------------------------------------------------------
	
	void CUIText::UpdateMetrics()
	{
		m_metricScale = (1.0f / m_pFont->GetHeader().fontSize) * m_data.fontSize * 0.5f;//m_td.fontSize * m_td.fontSizeMul;
		m_padding = static_cast<float>(m_pFont->GetHeader().padding) / 64.0f * m_metricScale;
	}

	void CUIText::CalculateStart()
	{
		m_position = 0.0f;//GetFrame()->GetWorldMatrix().TransformPoint(0.5f);

		if(m_data.text.size())
		{
			const auto& glyph = m_pFont->GetGlyph(m_data.text[0]);
			assert(IsValidGlyph(glyph));
			m_position.x -= (glyph.hBearingX * m_metricScale * m_data.spacingScale);
		}

		if(m_bUpdateText)
		{
			m_size = 0.0f;
			m_size.y = m_pFont->GetMaxGlyphHeight()[m_data.vAlign == VAlign::Bottom] * m_metricScale;

			for(size_t i = 0; i < m_data.text.size(); ++i)
			{
				const auto& glyph = m_pFont->GetGlyph(m_data.text[i]);
				if(!IsValidGlyph(glyph)) continue;

				if(i == m_data.text.size() - 1)
				{
					m_size.x += float(glyph.width) * m_metricScale + m_padding;
				}
				else
				{
					m_size.x += float(glyph.hAdvance) * m_metricScale * m_data.spacingScale;
				}
			}

			m_bUpdateText = false;
		}
		
		// Get the bounding from of the parent transform.
		Math::Bounds2D bounds = GetFrame()->GetBounds();

		switch(m_data.hAlign)
		{
			case HAlign::Left:
				//m_position.x -= (bounds.size.x * 0.0f);
				break;
			case HAlign::Center:
				m_position.x += (bounds.size.x * 0.5f) - (m_size.x * 0.5f);
				break;
			case HAlign::Right:
				m_position.x += (bounds.size.x * 1.0f - m_size.x);
				break;
			default:
				break;
		}

		switch(m_data.vAlign)
		{
			case VAlign::Top:
				m_position.y += (bounds.size.y * 1.0f - m_size.y);
				break;
			case VAlign::Center:
				m_position.y += (bounds.size.y * 0.5f) - (m_size.y * 0.5f);
				break;
			case VAlign::Bottom:
				//m_position.y -= (bounds.size.y * 0.0f - m_size.y);
				m_position.y += m_size.y;
				break;
			default:
				break;
		}
	}
};
