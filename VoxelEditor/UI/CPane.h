//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: UI/CPane.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CPANE_H
#define CPANE_H

#include <UI/CUIFrame.h>
#include <UI/CUIScript.h>

namespace UI
{
	class CPane
	{
	public:
		struct Data
		{
			std::wstring filename;
		};

	public:
		CPane(const CVObject* pObject, class CUICanvas* pCanvas);
		~CPane();
		CPane(const CPane&) = delete;
		CPane(CPane&&) = delete;
		CPane& operator = (const CPane&) = delete;
		CPane& operator = (CPane&&) = delete;

		void Initialize();
		void DebugUpdate();
		void Release();
		
		// Accessors.
		inline const CUIScript& GetScript() const { return m_script; }

		// Modifiers.
		inline void SetData(const Data& data) { m_data = data; }

	private:
		Data m_data;

		CUIFrame m_frame;
		CUIScript m_script;
	};
};

#endif
