//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: UI/CUIEventSystem.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CUIEventSystem.h"
#include "CUICanvas.h"
#include "CUISelect.h"
#include "CUITooltip.h"
#include "../Actors/CMotor.h"
#include "../Application/CInput.h"
#include "../Application/CInputMouse.h"
#include "../Application/CInputDeviceList.h"
#include "../Factory/CFactory.h"
#include "../Graphics/CGraphicsAPI.h"

namespace UI
{
	CUIEventSystem::CUIEventSystem() :
		m_pHoveredElement(nullptr)
	{
	}

	CUIEventSystem::~CUIEventSystem()
	{
	}
	
	void CUIEventSystem::PostInitialize()
	{
		m_commandList.PostInitialize();
	}

	void CUIEventSystem::Update()
	{
		Math::Vector2 cursor = App::CInput::Instance().GetDeviceList()->GetMouse()->GetMousePosition() -
			Math::Vector2(CFactory::Instance().GetGraphicsAPI()->GetWidth(), CFactory::Instance().GetGraphicsAPI()->GetHeight()) * 0.5f;
		cursor.y = -cursor.y;
		
		CUISelect* pSelectLast = m_pHoveredElement;
		m_pHoveredElement = nullptr;
		for(CUICanvas* pCanvas : m_canvasList)
		{
			if(!pCanvas->IsActive()) continue;

			pCanvas->UpdateElements();
			m_pHoveredElement = pCanvas->TestPoint(cursor);
			if(m_pHoveredElement != nullptr) break;
		}

		// Handle new hovered element.
		if(pSelectLast != m_pHoveredElement)
		{
			if(pSelectLast)
			{
				pSelectLast->OnExit();
			}

			if(m_pHoveredElement)
			{
				m_pHoveredElement->OnEnter();
			}

			for(auto elem : m_tooltipList)
			{
				elem->SetTooltip(m_pHoveredElement);
			}
		}
	}
	
	bool CUIEventSystem::ProcessInput(u32 code, const App::InputCallbackData& callback)
	{
		if(m_pHoveredElement == nullptr) return false;

		switch(code)
		{
			case Actor::MOTOR_INPUT_SELECT:
				m_pHoveredElement->OnSelect(callback);
				return true;
			default:
				return false;
		}
	}

	//-----------------------------------------------------------------------------------------------
	// (De)registration tooltip methods.
	//-----------------------------------------------------------------------------------------------
	
	void CUIEventSystem::RegisterTooltip(CUITooltip* pTooltip)
	{
		m_tooltipList.push_back(pTooltip);
	}
	
	void CUIEventSystem::DeregisterTooltip(CUITooltip* pTooltip)
	{
			size_t i, j;
			for(i = 0, j = 0; i < m_tooltipList.size(); ++i)
			{
				if(m_tooltipList[i] != pTooltip)
				{
					m_tooltipList[j++] = m_tooltipList[i];
				}
			}

			m_tooltipList.resize(j);
	}
	
	//-----------------------------------------------------------------------------------------------
	// (De)registration canvas methods.
	//-----------------------------------------------------------------------------------------------
	
	void CUIEventSystem::RegisterCanvas(CUICanvas* pCanvas)
	{
		m_canvasList.push_back(pCanvas);
	}
	
	void CUIEventSystem::DeregisterCanvas(CUICanvas* pCanvas)
	{
			size_t i, j;
			for(i = 0, j = 0; i < m_canvasList.size(); ++i)
			{
				if(m_canvasList[i] != pCanvas)
				{
					m_canvasList[j++] = m_canvasList[i];
				}
			}

			m_canvasList.resize(j);
	}
};
