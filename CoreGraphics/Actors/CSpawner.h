//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Actors/CSpawner.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CSPAWNER_H
#define CSPAWNER_H

#include <Objects/CNodeComponent.h>
#include <Objects/CVObject.h>
#include <Math/CMathFNV.h>
#include <unordered_map>


namespace Actor
{
	class CSpawner : public CNodeComponent
	{
	public:
		static const u32 HASH = Math::FNV1a_32(L"Spawner");

	public:
		struct Data
		{
		public:
			Data(u64 thisHash) : m_this(thisHash) { }
			~Data() { }
			Data(const Data&) = default;
			Data(Data&&) = default;
			Data& operator = (const Data&) = default;
			Data& operator = (Data&&) = default;

			// Accessors.
			inline u64 GetThis() const { return m_this; }

		private:
			u64 m_this;
		};

	private:
		CSpawner();
		~CSpawner();
		CSpawner(const CSpawner&) = delete;
		CSpawner(CSpawner&&) = delete;
		CSpawner& operator = (const CSpawner&) = delete;
		CSpawner& operator = (CSpawner&&) = delete;
		
	public:
		static CSpawner& Get() { static CSpawner comp; return comp; }

		void Register();
		void Spawn(class CPawn* pawn);

	private:
		void* AddToObject(u64 objHash) final;
		void* GetFromObject(u64 objHash) final;
		bool TryToGetFromObject(u64 objHash, void** ppData) final;
		void RemoveFromObject(u64 objHash) final;

	private:
		std::unordered_map<u64, Data> m_dataMap;
	};
};

#endif
