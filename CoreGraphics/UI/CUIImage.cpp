//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: UI/CUIImage.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CUIImage.h"
#include "CUIFrame.h"
#include "../Graphics/CTexture.h"
#include "../Graphics/CPostProcessor.h"
#include "../Graphics/CPostEffect.h"
#include "../Application/CSceneManager.h"
#include <Application/CCommandManager.h>
#include <Math/CMathFNV.h>
#include <Utilities/CConvertUtil.h>

namespace UI
{
	CUIImage::CUIImage(const CVObject* pObject, class CUICanvas* pCanvas, const CUIScript::Section* pSection) : 
		CUIRenderer(pObject, pCanvas, pSection)
	{
	}

	CUIImage::~CUIImage()
	{
	}

	void CUIImage::Initialize()
	{
		CUIRenderer::Initialize();
		
		m_cellSize = Math::Vector2(1.0f / m_data.cells.x, 1.0f / m_data.cells.y);

		const u32 i = m_data.cellIndex % m_data.cells.x;
		const u32 j = m_data.cellIndex / m_data.cells.x;
		
		m_data.texCoord = Math::Vector4(m_cellSize.x * i, m_cellSize.y * j, m_cellSize.x, m_cellSize.y);
	}

	void CUIImage::DrawSetup()
	{
		CUIRenderer::DrawSetup();
		
		if(!IsActive())
		{
			return;
		}
		
		if(m_data.bSplitImage)
		{
			SetupSplit();
		}
		else
		{
			SetupSingle();
		}
	}
	
	void CUIImage::SetupSingle()
	{
		CUIQuad::Instance& instance = *PullInstance(0);
		
		instance.color = m_data.color;
		instance.texCoord = m_data.texCoord;
		instance.world = GetFrame()->GetWorldMatrix();
	}

	void CUIImage::SetupSplit()
	{
		u32 index = 0;
		CUIQuad::Instance& i0 = *PullInstance(index++);
		CUIQuad::Instance& i1 = *PullInstance(index++);
		CUIQuad::Instance& i2 = *PullInstance(index++);
		CUIQuad::Instance& i3 = *PullInstance(index++);
		CUIQuad::Instance* i4 = m_data.bFillCenter ? PullInstance(index++) : nullptr;
		CUIQuad::Instance& i5 = *PullInstance(index++);
		CUIQuad::Instance& i6 = *PullInstance(index++);
		CUIQuad::Instance& i7 = *PullInstance(index++);
		CUIQuad::Instance& i8 = *PullInstance(index++);

		i0.color = i1.color = i2.color = i3.color = i5.color = i6.color = i7.color = i8.color = m_data.color;
		if(i4) i4->color = m_data.color;

		// Texture coordinates.
		const float texWidth = static_cast<float>(GetQuad()->GetTexture()->GetWidth()) * m_data.texCoord.z;
		const float texHeight = static_cast<float>(GetQuad()->GetTexture()->GetHeight()) * m_data.texCoord.w;

		const float cu0 = m_data.centerPartition.x / texWidth * m_data.texCoord.z;
		const float cu1 = m_data.centerPartition.z / texWidth * m_data.texCoord.z;
		const float cu2 = m_data.texCoord.z - (cu0 + cu1);
		const float cv0 = m_data.centerPartition.y / texHeight * m_data.texCoord.w;
		const float cv1 = m_data.centerPartition.w / texHeight * m_data.texCoord.w;
		const float cv2 = m_data.texCoord.w - (cv0 + cv1);

		const float ou = m_data.texCoord.x;
		const float ov = m_data.texCoord.y;
		
		i0.texCoord         = Math::Vector4(ou,             ov, cu0, cv0);
		i1.texCoord         = Math::Vector4(ou + cu0,       ov, cu1, cv0);
		i2.texCoord         = Math::Vector4(ou + cu0 + cu1, ov, cu2, cv0);

		i3.texCoord         = Math::Vector4(ou,             ov + cv0, cu0, cv1);
		if(i4) i4->texCoord = Math::Vector4(ou + cu0,       ov + cv0, cu1, cv1);
		i5.texCoord         = Math::Vector4(ou + cu0 + cu1, ov + cv0, cu2, cv1);

		i6.texCoord         = Math::Vector4(ou,             ov + cv0 + cv1, cu0, cv2);
		i7.texCoord         = Math::Vector4(ou + cu0,       ov + cv0 + cv1, cu1, cv2);
		i8.texCoord         = Math::Vector4(ou + cu0 + cu1, ov + cv0 + cv1, cu2, cv2);

		// Scale.
		const Math::Vector2 size = GetFrame()->GetBounds().size;

		float slx = m_data.centerPartition.x;
		float scx = m_data.centerPartition.z;
		float srx = texWidth - (slx + scx);
		float sly = m_data.centerPartition.y;
		float scy = m_data.centerPartition.w;
		float sry = texHeight - (sly + scy);

		scx = std::max(size.x - slx - srx, 0.0f);
		scy = std::max(size.y - sly - sry, 0.0f);

		// Position.
		float offsetX = std::max(0.0f, -(srx - slx)) + srx;
		float offsetY = std::max(0.0f,  (sry - sly)) + sry;

		float plx = offsetX - slx - 0.0f;
		float pcx = offsetX;
		float prx = offsetX + scx + 0.0f;

		float ply = offsetY + scy + 0.0f;
		float pcy = offsetY;
		float pry = offsetY - sry - 0.0f;

		// Matrix calculation.
		const Math::Matrix3x2 world = Math::Matrix3x2::Scale(Math::Vector2(1.0f / size.x, 1.0f / size.y)) * GetFrame()->GetWorldMatrix();

		i0.world         = Math::Matrix3x2::Scale(Math::Vector2(slx, sly)) * Math::Matrix3x2::Translate(Math::Vector2(plx, ply)) * world;
		i1.world         = Math::Matrix3x2::Scale(Math::Vector2(scx, sly)) * Math::Matrix3x2::Translate(Math::Vector2(pcx, ply)) * world;
		i2.world         = Math::Matrix3x2::Scale(Math::Vector2(srx, sly)) * Math::Matrix3x2::Translate(Math::Vector2(prx, ply)) * world;

		i3.world         = Math::Matrix3x2::Scale(Math::Vector2(slx, scy)) * Math::Matrix3x2::Translate(Math::Vector2(plx, pcy)) * world;
		if(i4) i4->world = Math::Matrix3x2::Scale(Math::Vector2(scx, scy)) * Math::Matrix3x2::Translate(Math::Vector2(pcx, pcy)) * world;
		i5.world         = Math::Matrix3x2::Scale(Math::Vector2(srx, scy)) * Math::Matrix3x2::Translate(Math::Vector2(prx, pcy)) * world;

		i6.world         = Math::Matrix3x2::Scale(Math::Vector2(slx, sry)) * Math::Matrix3x2::Translate(Math::Vector2(plx, pry)) * world;
		i7.world         = Math::Matrix3x2::Scale(Math::Vector2(scx, sry)) * Math::Matrix3x2::Translate(Math::Vector2(pcx, pry)) * world;
		i8.world         = Math::Matrix3x2::Scale(Math::Vector2(srx, sry)) * Math::Matrix3x2::Translate(Math::Vector2(prx, pry)) * world;
	}
	
	//-----------------------------------------------------------------------------------------------
	// CUICommand methods.
	//-----------------------------------------------------------------------------------------------
	
	bool CUIImage::CommandElement(const CUICommandList::StateElem& stateElem)
	{
		if(CUIRenderer::CommandElement(stateElem))
		{
			return true;
		}

		struct MapData
		{
			const CUICommandList::StateElem& stateElem;
			CUIImage* pImage;
			Data& data;
		};

		typedef std::function<void(const MapData&)> SECTION_FUNC;
		static const std::unordered_map<std::wstring, SECTION_FUNC> IMAGE_MAP {
			{ L"bSplitImage", [](const MapData& mapData) { mapData.data.bSplitImage = mapData.stateElem.val.b; } },
			{ L"bFillCenter", [](const MapData& mapData) { mapData.data.bFillCenter = mapData.stateElem.val.b; } },

			{ L"texCoord", [](const MapData& mapData) { mapData.data.texCoord = CUIElement::CommandElementVector<Math::Vector4>(mapData.stateElem, mapData.data.texCoord); } },
			{ L"index", [](const MapData& mapData) { mapData.pImage->SetTexIndex(mapData.stateElem.val.l); } },
			{ L"centerPartition", [](const MapData& mapData) { mapData.data.centerPartition = CUIElement::CommandElementVector<Math::Vector4>(mapData.stateElem, mapData.data.centerPartition); } },
		};
		
		const auto& elem = IMAGE_MAP.find(stateElem.GetSubElemAt(0));
		if(elem == IMAGE_MAP.end()) return false;

		elem->second({ stateElem, this, m_data });
		MarkAsDirty();

		return true;
	}

	//-----------------------------------------------------------------------------------------------
	// Section methods.
	//-----------------------------------------------------------------------------------------------
	
	bool CUIImage::SetFullDataFromKeyValue(CUIImage* pImage, const CUIScript* pScript, const Util::CScriptObject::PropBlock& prop, const std::wstring& key, const std::wstring& value, Data& data)
	{
		if(CUIElement::SetDataFromKeyValue(pScript, prop, key, value, data)) return true;
		if(CUIRenderer::SetDataFromKeyValue(pScript, key, value, data)) return true;
		if(CUIImage::SetDataFromKeyValue(pImage, pScript, key, value, data)) return true;
		return false;
	}

	bool CUIImage::SetDataFromKeyValue(CUIImage* pImage, const CUIScript* pScript, const std::wstring& key, const std::wstring& value, Data& data)
	{
		struct MapData
		{
			CUIImage* pImage;
			const CUIScript* pScript;
			const std::wstring& prop;
			Data& data;
		};

		typedef std::function<void(const MapData&)> SECTION_FUNC;
		static const std::unordered_map<std::wstring, SECTION_FUNC> IMAGE_MAP {
			{ L"bSplitImage", [](const MapData& mapData) { mapData.data.bSplitImage = Util::StringToBool(mapData.prop); } },
			{ L"bFillCenter", [](const MapData& mapData) { mapData.data.bFillCenter = Util::StringToBool(mapData.prop); } },
			{ L"bIgnoreCells", [](const MapData& mapData) { mapData.data.bIgnoreCells = Util::StringToBool(mapData.prop); } },

			{ L"texCoord", [](const MapData& mapData) { mapData.data.texCoord = Util::StringToVector<Math::Vector4>(mapData.prop); } },
			{ L"index", [](const MapData& mapData) { mapData.data.cellIndex = Util::StringToLong(mapData.prop); } },
			{ L"centerPartition", [](const MapData& mapData) { mapData.data.centerPartition = Util::StringToVector<Math::Vector4>(mapData.prop); } },

			{ L"material", [](const MapData& mapData) { mapData.data.matHash = Math::FNV1a_64(mapData.prop.c_str()); } },

			{ L"texture", [](const MapData& mapData) {
					mapData.data.texHash = Math::FNV1a_64(mapData.prop.c_str());
					if(!mapData.data.bIgnoreCells)
					{
						CUIScript::Texture texture;
						if(mapData.pScript->TryGetTexture(mapData.prop, texture))
						{
							mapData.data.cells = texture.cells;
						}
						else
						{
							mapData.data.cells = { 1, 1 };
						}
					}
				}
			},

			{ L"key", [](const MapData& mapData) {
					const auto& postProcessor = App::CSceneManager::Instance().GetView(Math::FNV1a_32(mapData.prop.c_str()))->GetPostProcessor();
					mapData.data.texHash = postProcessor.GetPostEffect(postProcessor.GetPostEffectCount() - 1)->GetRenderTextureList()[0]->GetRTVTextureHash(0);
				}
			},
			
			{ L"actionIndexChange", [](const MapData& mapData) { App::CCommandManager::Instance().RegisterAction(Math::FNV1a_64(mapData.prop.c_str(), mapData.prop.size()), 
				std::bind(&CUIImage::OnIndexChange, mapData.pImage, std::placeholders::_1)); } },
		};

		const auto& elem = IMAGE_MAP.find(key);
		if(elem == IMAGE_MAP.end()) return false;

		elem->second({ pImage, pScript, value, data });
		return true;
	}

	void CUIImage::BuildFromSection(const CUIScript::Section* pSection)
	{
		CUIRenderer::BuildFromSection(pSection);
		
		if(pSection->cls.size())
		{
			CUIScript::ProcessBlock(*pSection->pScript->GetClass(pSection->cls).typeProp, [&](const std::vector<std::wstring>& str, const Util::CScriptObject::PropBlock& prop, const Util::CScriptObject::PropBlock* pParent){
				SetDataFromKeyValue(this, pSection->pScript, str[0], prop.propList[1], m_data);
			});
		}

		CUIScript::ProcessBlock(*pSection->typeProp, [&](const std::vector<std::wstring>& str, const Util::CScriptObject::PropBlock& prop, const Util::CScriptObject::PropBlock* pParent){
			SetDataFromKeyValue(this, pSection->pScript, str[0], prop.propList[1], m_data);
		});
	}
	
	//-----------------------------------------------------------------------------------------------
	// Commands methods.
	//-----------------------------------------------------------------------------------------------
	
	void CUIImage::OnIndexChange(const void* params)
	{
		SetTexIndex(*reinterpret_cast<const u32*>(params));
		MarkAsDirty();
	}
	
	//-----------------------------------------------------------------------------------------------
	// Property methods.
	//-----------------------------------------------------------------------------------------------
	
	void CUIImage::SetTexCoord(const Math::Vector4& texCoord)
	{
		m_data.texCoord = texCoord;
		MarkAsDirty();
	}
	
	void CUIImage::SetTexIndex(u32 index)
	{
		m_data.cellIndex = index;
		const u32 i = index % m_data.cells.x;
		const u32 j = index / m_data.cells.x;
		
		m_data.texCoord = Math::Vector4(m_cellSize.x * i, m_cellSize.y * j, m_cellSize.x, m_cellSize.y);
	}
};
