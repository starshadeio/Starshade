//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: UI/CUIElement.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CUIELEMENT_H
#define CUIELEMENT_H

#include "CUIFrame.h"
#include "CUIScript.h"
#include "CUICommandList.h"
#include <Math/CMathColor.h>
#include <Math/CMathVector2.h>
#include <Math/CMathVector3.h>
#include <Math/CMathVector4.h>
#include <Math/CMathVectorInt2.h>
#include <Math/CMathVectorInt3.h>
#include <Math/CMathVectorInt4.h>
#include <Objects/CVComponent.h>
#include <Utilities/CConvertUtil.h>

namespace UI
{
	class CUIElement : public CVComponent
	{
	protected:
		struct ElementData
		{
			s32 depth = 0;
			CUIFrame::Data frameData = { };
		};

	public:
		CUIElement(const CVObject* pObject, class CUICanvas* pCanvas, const CUIScript::Section* pSection);
		virtual ~CUIElement();
		CUIElement(const CUIElement&) = delete;
		CUIElement(CUIElement&&) = delete;
		CUIElement& operator = (const CUIElement&) = delete;
		CUIElement& operator = (CUIElement&&) = delete;
		
		void Initialize() override;
		virtual void OnRegister(class CUICanvas* pCanvas) { }
		virtual void DebugUpdate();

		virtual bool CommandElement(const CUICommandList::StateElem& stateElem);
		
		static bool SetDataFromKeyValue(const CUIScript* pScript, const Util::CScriptObject::PropBlock& prop, const std::wstring& key, const std::wstring& value, ElementData& data);
		
		// Accessors.
		inline bool IsActive() const { return m_frame.IsActive(); }

		inline s32 GetDepth() const { return GetElementData().depth; }
		inline class CUIFrame* GetFrame() { return &m_frame; }

	protected:
		virtual void OnUpdate() { }
		virtual void OnActiveChange(bool bActive) { }
		virtual void BuildFromSection(const CUIScript::Section* pSection);

		template<typename T>
		static T CommandElementVectorInt(const CUICommandList::StateElem& stateElem, const T& initVec)
		{
			T vec;
			if(stateElem.GetSubElemSize() > 1)
			{
				vec = initVec;
				switch(stateElem.GetSubElemAt(1)[0])
				{
					case 'r':
					case 'x':
						vec[0] = stateElem.val.l;
						break;
					case 'g':
					case 'y':
						vec[1] = stateElem.val.l;
						break;
					case 'b':
					case 'z':
						vec[2] = stateElem.val.l;
						break;
					case 'a':
					case 'w':
						vec[3] = stateElem.val.l;
						break;
					default:
						break;
				}
			}
			else
			{
				vec = Util::StringToVectorInt<T>(stateElem.valStr);
			}

			return vec;
		}
		
		template<typename T>
		static T CommandElementVector(const CUICommandList::StateElem& stateElem, const T& initVec)
		{
			T vec;
			if(stateElem.GetSubElemSize() > 1)
			{
				vec = initVec;
				switch(stateElem.GetSubElemAt(1)[0])
				{
					case 'r':
					case 'x':
						vec[0] = stateElem.val.f;
						break;
					case 'g':
					case 'y':
						vec[1] = stateElem.val.f;
						break;
					case 'b':
					case 'z':
						vec[2] = stateElem.val.f;
						break;
					case 'a':
					case 'w':
						vec[3] = stateElem.val.f;
						break;
					default:
						break;
				}
			}
			else
			{
				vec = Util::StringToVector<T>(stateElem.valStr);
			}

			return vec;
		}
		
		// Accessors.
		virtual inline const ElementData& GetElementData() const = 0;

	private:
		CUIFrame m_frame;
		const CUIScript::Section* m_pSection;
	};
};

#endif
