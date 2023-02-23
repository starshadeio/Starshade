//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Application/CNodeManager.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CNODEMANAGER_H
#define CNODEMANAGER_H

#include <Globals/CGlobals.h>
#include <Math/CMathRect.h>
#include <unordered_map>
#include <vector>

namespace App
{
	class CNodeManager
	{
	public:
		CNodeManager();
		~CNodeManager();
		CNodeManager(const CNodeManager&) = delete;
		CNodeManager(CNodeManager&&) = delete;
		CNodeManager& operator = (const CNodeManager&) = delete;
		CNodeManager& operator = (CNodeManager&&) = delete;
		
		void Initialize();
		void PostInitialize();
		void Update();
		void LateUpdate();
		void Release();

		void OnMove(const Math::Rect& rect);
		void OnResize(const Math::Rect& rect);

		void RegisterNode(class CNode* pNode);
		void DeregisterNode(class CNode* pNode);

	private:
		std::unordered_map<u64, class CNode*> m_nodeMap;
		std::vector<class CNode*> m_loadedList;
	};
};

#endif
