//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Application/CNodeSelect.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CNodeSelect.h"
#include "CAppData.h"
#include "CEditor.h"
#include "../Resources/CAssetManager.h"
#include <Physics/CPhysics.h>
#include <Application/CCommandManager.h>
#include <Application/CSceneManager.h>
#include <Universe/CChunkNode.h>
#include <Logic/CCameraManager.h>
#include <Application/CInput.h>
#include <Application/CInputDeviceList.h>
#include <Application/CInputMouse.h>
#include <unordered_map>

namespace App
{
	const static std::unordered_map<CNodeSelect::BrushSize, float> BRUSH_SIZE_SCALE_MAP = {
		{ CNodeSelect::BrushSize::_1, 1.0f },
		{ CNodeSelect::BrushSize::_2, 2.0f },
		{ CNodeSelect::BrushSize::_4, 4.0f }
	};

	CNodeSelect::CNodeSelect() :
		m_bActive(false),
		m_selectionType(SelectionType::Fill),
		m_brushSize(BrushSize::_4),
		m_pickedInfo{},
		m_markerErase(L"Chunk Marker (Erase)", VIEW_HASH_MAIN),
		m_markerFill(L"Chunk Marker (Fill)", VIEW_HASH_MAIN),
		m_markerPaint(L"Chunk Marker (Paint)", VIEW_HASH_MAIN),
		m_pPickedCallback(nullptr)
	{
	}

	CNodeSelect::~CNodeSelect()
	{
	}
	
	void CNodeSelect::Initialize()
	{
		{ // Setup quad.
			CBlockMarker::Data data { };

			data.matHash = Math::FNV1a_64("MATERIAL_DELETION_QUAD");
			m_markerErase.SetData(data);
			m_markerErase.Initialize();
			m_markerErase.SetActive(false);

			data.matHash = Math::FNV1a_64("MATERIAL_PLACEMENT_QUAD");
			m_markerFill.SetData(data);
			m_markerFill.Initialize();
			m_markerFill.SetActive(false);

			data.matHash = Math::FNV1a_64("MATERIAL_PLACEMENT_QUAD");
			m_markerPaint.SetData(data);
			m_markerPaint.Initialize();
			m_markerPaint.SetActive(false);
		}
		
		m_pickedInfo.pNodeSelector = this;
		m_ray.SetDistance(m_data.maxDistance);

		{ // Setup commands.
			CCommand::Properties props = { };
			props.bSaveEnabled = true;

			CCommandManager::Instance().RegisterCommand(CMD_KEY_TOGGLE_SELECT_MODE, std::bind(&CNodeSelect::ToggleMode, this));
			CCommandManager::Instance().RegisterCommand(CMD_KEY_TOGGLE_BRUSH_SIZE, std::bind(&CNodeSelect::ToggleSize, this));
			
			CCommandManager::Instance().RegisterCommand(CMD_KEY_SELECT_ERASE, std::bind(&CNodeSelect::EraseMode, this));
			CCommandManager::Instance().RegisterCommand(CMD_KEY_SELECT_FILL, std::bind(&CNodeSelect::FillMode, this));
			CCommandManager::Instance().RegisterCommand(CMD_KEY_SELECT_PAINT, std::bind(&CNodeSelect::PaintMode, this));
			
			CCommandManager::Instance().RegisterCommand(CMD_KEY_SELECT_ERASE_OFF, [](const void* params, bool bInverse){ return true; });
			CCommandManager::Instance().RegisterCommand(CMD_KEY_SELECT_FILL_OFF, [](const void* params, bool bInverse){ return true; });
			CCommandManager::Instance().RegisterCommand(CMD_KEY_SELECT_PAINT_OFF, [](const void* params, bool bInverse){ return true; });
			
			CCommandManager::Instance().RegisterCommand(CMD_KEY_SELECT_BRUSH_1, std::bind(&CNodeSelect::Brush1, this));
			CCommandManager::Instance().RegisterCommand(CMD_KEY_SELECT_BRUSH_2, std::bind(&CNodeSelect::Brush2, this));
			CCommandManager::Instance().RegisterCommand(CMD_KEY_SELECT_BRUSH_4, std::bind(&CNodeSelect::Brush4, this));
			
			CCommandManager::Instance().RegisterCommand(CMD_KEY_SELECT_BRUSH_1_OFF, [](const void* params, bool bInverse){ return true; });
			CCommandManager::Instance().RegisterCommand(CMD_KEY_SELECT_BRUSH_2_OFF, [](const void* params, bool bInverse){ return true; });
			CCommandManager::Instance().RegisterCommand(CMD_KEY_SELECT_BRUSH_4_OFF, [](const void* params, bool bInverse){ return true; });

			CCommandManager::Instance().RegisterCommand(CMD_KEY_SELECT_ACTION, std::bind(&CNodeSelect::Select, this, std::placeholders::_1, std::placeholders::_2), props);
			CCommandManager::Instance().RegisterCommand(CMD_KEY_SELECT_PRESSED, [](const void* params, bool bInverse){ return true; });
			CCommandManager::Instance().RegisterCommand(CMD_KEY_SELECT_RELEASED, [](const void* params, bool bInverse){ return true; });
		}
	}

	void CNodeSelect::Update()
	{
		{ // Setup the ray.
			auto pCamera = App::CSceneManager::Instance().CameraManager().GetDefaultCamera();
				
			// Calculate picking ray.
			Math::Vector2 mousePos = App::CInput::Instance().GetDeviceList()->GetMouse()->GetMousePosition();

			mousePos.x -= pCamera->GetSreenRect().x;
			mousePos.y -= pCamera->GetSreenRect().y;
					
			Math::SIMDVector coord(
				(mousePos.x / pCamera->GetSreenRect().w - 0.5f) * 2.0f / pCamera->GetProjectionMatrix().rows[0].m128_f32[0],
				-(mousePos.y / pCamera->GetSreenRect().h - 0.5f) * 2.0f  / pCamera->GetProjectionMatrix().rows[1].m128_f32[1],
				1.0f
			);

			coord = pCamera->GetViewMatrixInv().TransformNormal(coord);
			if(_mm_cvtss_f32(coord.LengthSq()) > 1e-5f)
			{
				coord.Normalize();
			}

			std::lock_guard<std::shared_mutex> lk(m_rayMutex);
			m_ray.SetOrigin(pCamera->GetTransform()->GetPosition());
			m_ray.SetDirection(coord);
		}

		bool bValidState = m_bActive && !App::CSceneManager::Instance().UIEventSystem().GetHovered() && 
			CEditor::Instance().Nodes().Gizmo().Pivot().GetActiveType() == CGizmoPivot::GizmoType::Select;

		if(bValidState)
		{
			Physics::RaycastInfo rayInfo;

			{
				std::lock_guard<std::mutex> lk(m_pickingMutex);
				rayInfo = m_pickedInfo.info;
			}

			if(rayInfo.pVolume)
			{
				const Universe::CChunkNode* pChunkNode = dynamic_cast<const Universe::CChunkNode*>(rayInfo.pVolume->GetVObject());
		
				if(pChunkNode)
				{
					bool bBoundary = rayInfo.index.U32.hi[1] & (0x1 << 31); // Block index.

					Math::SIMDVector coord = pChunkNode->GetPosition({ Math::VectorInt3(
						static_cast<int>(rayInfo.index.U32.lo[0]),
						static_cast<int>(rayInfo.index.U32.lo[1]),
						static_cast<int>(rayInfo.index.U32.hi[0])),
						rayInfo.index.U32.hi[1] ^ (bBoundary << 31)
					});

					if(bBoundary)
					{
						coord -= rayInfo.normal * pChunkNode->GetBlockSize() * 0.5f;
					}
					else
					{
						coord += rayInfo.normal * pChunkNode->GetBlockSize() * 0.5f;
					}

					Math::SIMDVector euler = Math::SIMDVector(asinf(rayInfo.normal[1]), atan2f(-rayInfo.normal[0], -rayInfo.normal[2]), 0.0f);
					const float scale = pChunkNode->GetBlockSize() * BRUSH_SIZE_SCALE_MAP.find(m_brushSize)->second;

					Math::SIMDVector diff;
					
					if(m_brushSize == BrushSize::_1)
					{
						diff = Math::SIMD_VEC_ZERO;
					}
					else
					{
						diff = rayInfo.point - coord;
						diff -= diff * _mm_cvtss_f32(Math::SIMDVector::Dot(diff, rayInfo.normal));

						const float diffLenSq = _mm_cvtss_f32(diff.LengthSq());
						if(diffLenSq > 1e-5f)
						{
							diff /= sqrtf(diffLenSq);

							diff = _mm_and_ps(_mm_cmpgt_ps(_mm_and_ps(diff.m_xmm, _mm_set1_ps(Math::g_Nan)), _mm_set1_ps(1e-3f)), diff.m_xmm);

							diff = _mm_or_ps(
								_mm_and_ps(diff.m_xmm, _mm_set_ps1(-0.0f)),
								_mm_and_ps(
									_mm_set_ps1(1.0f),
									_mm_cmpgt_ps(_mm_and_ps(diff.m_xmm, _mm_set_ps1(Math::g_Nan)), _mm_set_ps1(1e-5f))
								)
							);
						}
						else
						{ // Point is too close to center, so generate an offset manually.
							if(_mm_cvtss_f32(Math::SIMDVector::Dot(rayInfo.normal, Math::SIMD_VEC_UP)) > 1e-5f)
							{
								diff = Math::SIMD_VEC_RIGHT + Math::SIMD_VEC_FORWARD;
							}
							else if(_mm_cvtss_f32(Math::SIMDVector::Dot(rayInfo.normal, Math::SIMD_VEC_RIGHT)) > 1e-5f)
							{
								diff = Math::SIMD_VEC_UP + Math::SIMD_VEC_FORWARD;
							}
							else
							{
								diff = Math::SIMD_VEC_RIGHT + Math::SIMD_VEC_UP;
							}
						}
					
						diff *= pChunkNode->GetBlockSize() * 0.5f;
					}

					switch(m_selectionType)
					{
						case App::CNodeSelect::SelectionType::Fill:
							m_markerPoint = coord + rayInfo.normal * scale * 0.5f + diff;
							break;
						case App::CNodeSelect::SelectionType::Paint:
						case App::CNodeSelect::SelectionType::Erase:
							m_markerPoint = coord - rayInfo.normal * scale * 0.5f + diff;
							break;
						default:
							break;
					}
					
					m_markerErase.GetTransform()->SetScale(scale);
					m_markerErase.GetTransform()->SetPosition(m_markerPoint);

					m_markerFill.GetTransform()->SetScale(scale);
					m_markerFill.GetTransform()->SetPosition(m_markerPoint);

					m_markerPaint.GetTransform()->SetScale(scale);
					m_markerPaint.GetTransform()->SetPosition(m_markerPoint);
				}
				else
				{
					bValidState = false;
				}
			}
			else
			{
				bValidState = false;
			}
		}
		else
		{
		}

		UpdateSelectionQuadState(bValidState);
	}

	void CNodeSelect::PhysicsUpdate()
	{
		// Cast ray for picking.
		Physics::QueryRay ray { };
		ray.callback = [this](const std::vector<Physics::RaycastInfo>& contacts){
			const Physics::RaycastInfo* pRaycastInfo = nullptr;

			for(size_t i = 0; i < contacts.size(); ++i)
			{
				if(!pRaycastInfo || contacts[i].distance < pRaycastInfo->distance)
				{
					pRaycastInfo = &contacts[i];
				}
			}

			std::lock_guard<std::mutex> lk(m_pickingMutex);
			if(pRaycastInfo) m_pickedInfo.info = *pRaycastInfo;
			else m_pickedInfo.info = { };
		};

		{
			ray.ray = GetScreenRay();
		}

		Physics::CPhysics::Instance().CastRay(ray);
	}

	void CNodeSelect::Release()
	{
		m_markerPaint.Release();
		m_markerFill.Release();
		m_markerErase.Release();
	}

	//-----------------------------------------------------------------------------------------------
	// Commands methods.
	//-----------------------------------------------------------------------------------------------
	
	bool CNodeSelect::ToggleMode()
	{
		switch(m_selectionType)
		{
			case SelectionType::Erase:
				App::CCommandManager::Instance().Execute(CMD_KEY_SELECT_FILL);
				break;
			case SelectionType::Fill:
				App::CCommandManager::Instance().Execute(CMD_KEY_SELECT_PAINT);
				break;
			case SelectionType::Paint:
				App::CCommandManager::Instance().Execute(CMD_KEY_SELECT_ERASE);
				break;
			default:
				break;
		}

		return true;
	}
	
	bool CNodeSelect::ToggleSize()
	{
		switch(m_brushSize)
		{
			case BrushSize::_1:
				App::CCommandManager::Instance().Execute(CMD_KEY_SELECT_BRUSH_2);
				break;
			case BrushSize::_2:
				App::CCommandManager::Instance().Execute(CMD_KEY_SELECT_BRUSH_4);
				break;
			case BrushSize::_4:
				App::CCommandManager::Instance().Execute(CMD_KEY_SELECT_BRUSH_1);
				break;
			default:
				break;
		}

		return true;
	}

	bool CNodeSelect::EraseMode()
	{
		if(m_selectionType == SelectionType::Erase) return false;
		m_selectionType = SelectionType::Erase;
		App::CCommandManager::Instance().Execute(CMD_KEY_SELECT_FILL_OFF);
		App::CCommandManager::Instance().Execute(CMD_KEY_SELECT_PAINT_OFF);
		return true;
	}

	bool CNodeSelect::FillMode()
	{
		if(m_selectionType == SelectionType::Fill) return false;
		m_selectionType = SelectionType::Fill;
		App::CCommandManager::Instance().Execute(CMD_KEY_SELECT_ERASE_OFF);
		App::CCommandManager::Instance().Execute(CMD_KEY_SELECT_PAINT_OFF);
		return true;
	}
	
	bool CNodeSelect::PaintMode()
	{
		if(m_selectionType == SelectionType::Paint) return false;
		m_selectionType = SelectionType::Paint;
		App::CCommandManager::Instance().Execute(CMD_KEY_SELECT_ERASE_OFF);
		App::CCommandManager::Instance().Execute(CMD_KEY_SELECT_FILL_OFF);
		return true;
	}

	bool CNodeSelect::Brush1()
	{
		if(m_brushSize == BrushSize::_1) return false;
		m_brushSize = BrushSize::_1;
		App::CCommandManager::Instance().Execute(CMD_KEY_SELECT_BRUSH_2_OFF);
		App::CCommandManager::Instance().Execute(CMD_KEY_SELECT_BRUSH_4_OFF);
		return true;
	}
	
	bool CNodeSelect::Brush2()
	{
		if(m_brushSize == BrushSize::_2) return false;
		m_brushSize = BrushSize::_2;
		App::CCommandManager::Instance().Execute(CMD_KEY_SELECT_BRUSH_1_OFF);
		App::CCommandManager::Instance().Execute(CMD_KEY_SELECT_BRUSH_4_OFF);
		return true;
	}

	bool CNodeSelect::Brush4()
	{
		if(m_brushSize == BrushSize::_4) return false;
		m_brushSize = BrushSize::_4;
		App::CCommandManager::Instance().Execute(CMD_KEY_SELECT_BRUSH_1_OFF);
		App::CCommandManager::Instance().Execute(CMD_KEY_SELECT_BRUSH_2_OFF);
		return true;
	}

	bool CNodeSelect::Select(const void* param, bool bInverse)
	{
		bool bPressed = *reinterpret_cast<const bool*>(param);
		bool bResult = false;

		if(CEditor::Instance().Nodes().Gizmo().Pivot().GetActiveType() == CGizmoPivot::GizmoType::Select)
		{
			std::lock_guard<std::mutex> lk(m_pickingMutex);
			if(m_pickedInfo.info.pVolume)
			{
				Logic::CCallback* pCallback = m_pickedInfo.info.pVolume->GetVObject()->FindComponent<Logic::CCallback>();
				if(pCallback)
				{
					m_pickedInfo.info.alt.U8.lo[0] = bPressed ? 1 : 2;
					m_pickedInfo.info.alt.U8.lo[1] = static_cast<u8>(m_selectionType);
					m_pickedInfo.info.alt.U8.lo[2] = static_cast<u8>(m_brushSize);

					m_pickedInfo.info.alt.U8.hi[0] = Resources::CAssets::Instance().GetPalette().GetPaletteIndex();

					const Universe::CChunkNode* pChunkNode = dynamic_cast<const Universe::CChunkNode*>(m_pickedInfo.info.pVolume->GetVObject());
					if(pChunkNode)
					{
						Math::Vector3 coord = *(Math::Vector3*)m_markerPoint.ToFloat() / pChunkNode->GetBlockSize();
						
						const u64 lo = m_pickedInfo.info.index.U64.lo;
						const u64 hi = m_pickedInfo.info.index.U64.hi;

						union FloatU32 {
							float f;
							u32 i;
							FloatU32(float f) : f(f) { }
						};

						m_pickedInfo.info.index.U32.lo[0] = FloatU32(coord.x).i;
						m_pickedInfo.info.index.U32.lo[1] = FloatU32(coord.y).i;
						m_pickedInfo.info.index.U32.hi[0] = FloatU32(coord.z).i;

						bResult = pCallback->CallCallback(Logic::CALLBACK_INTERACT, &m_pickedInfo.info);

						m_pickedInfo.info.index.U64.lo = lo;
						m_pickedInfo.info.index.U64.hi = hi;
					}
					else
					{
						bResult = pCallback->CallCallback(Logic::CALLBACK_INTERACT, &m_pickedInfo.info);
					}

					m_pPickedCallback = bPressed ? pCallback : nullptr;
				}
			}
			else if(m_pPickedCallback)
			{
				m_pickedInfo.info.alt.U8.lo[0] = 0;
				bResult = m_pPickedCallback->CallCallback(Logic::CALLBACK_INTERACT, &m_pickedInfo);
				m_pPickedCallback = nullptr;
			}
		}

		return bResult;
	}

	//-----------------------------------------------------------------------------------------------
	// State methods.
	//-----------------------------------------------------------------------------------------------

	void CNodeSelect::OnPlay()
	{
		m_bActive = false;
	}

	void CNodeSelect::OnEdit()
	{
		m_bActive = true;
	}

	void CNodeSelect::UpdateSelectionQuadState(bool bEnabled)
	{
		m_markerErase.SetActive(m_selectionType == SelectionType::Erase && bEnabled);
		m_markerFill.SetActive(m_selectionType == SelectionType::Fill && bEnabled);
		m_markerPaint.SetActive(m_selectionType == SelectionType::Paint && bEnabled);
	}
};
