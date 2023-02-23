//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Application/CNode.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CNODE_H
#define CNODE_H

#include "../Universe/CChunkNode.h"
#include <Objects/CNodeObject.h>
#include <Globals/CGlobals.h>
#include <Math/CMathRect.h>
#include <functional>
#include <string>
#include <unordered_map>
#include <cassert>
#include <fstream>

class CVObject;

namespace App
{
	class CNode
	{
	public:
		friend class CNodeManager;

		CNode(const wchar_t* pName, u32 viewHash = 0);
		virtual ~CNode();
		CNode(const CNode&) = delete;
		CNode(CNode&&) = delete;
		CNode& operator = (const CNode&) = delete;
		CNode& operator = (CNode&&) = delete;
		
		void Load(std::ifstream& file);
		
		u64 AddObject(const wchar_t* name, bool bForceSelect = true);
		bool SelectObject(u64 hash);
		
		template<typename T, typename D>
		void AddComponent(std::function<void(D*)> action)
		{
			assert(m_pSelectedObject);
			action(m_pSelectedObject->AddComponent<T>());
		}
		
		// Accessors.
		u64 GetHash() const { return m_hash; }

		inline CNodeObject* GetSelectedObject() { return m_pSelectedObject; }
		inline Universe::CChunkNode* GetChunkNode() { return &m_chunkNode; }

	protected:
		virtual void Initialize();
		virtual void PostInitialize() { }

		virtual void Update() { }
		virtual void LateUpdate() { }

		virtual void Release();

		virtual void OnMove(const Math::Rect& rect) { }
		virtual void OnResize(const Math::Rect& rect) { }

		virtual CNodeObject* CreateNodeObject(const wchar_t* name);

	protected:
		u32 m_viewHash;
		u64 m_hash;

		Universe::CChunkNode m_chunkNode;
		
		std::unordered_map<u64, CNodeObject*> m_objMap;
		CNodeObject* m_pSelectedObject;

		const wchar_t* m_pName;
	};
};

#endif
