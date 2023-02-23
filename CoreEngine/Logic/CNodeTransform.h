//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Engine
//
// File: Logic/CNodeTransform.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CNODETRANSFORM_H
#define CNODETRANSFORM_H

#include "../Objects/CNodeComponent.h"
#include "../Math/CMathFNV.h"
#include "../Math/CSIMDMatrix.h"
#include "../Utilities/CMemAlign.h"
#include "../Utilities/CMacroUtil.h"
#include <vector>
#include <queue>
#include <unordered_map>

class CNodeObject;

namespace Logic
{
	enum AXIS_CONSTRAINT_FLAGS
	{
		AXIS_CONSTRAINT_FLAG_NONE = 0x0,
		AXIS_CONSTRAINT_FLAG_X = 0x1,
		AXIS_CONSTRAINT_FLAG_Y = 0x2,
		AXIS_CONSTRAINT_FLAG_Z = 0x4,
		AXIS_CONSTRAINT_FLAG_ALL = AXIS_CONSTRAINT_FLAG_X | AXIS_CONSTRAINT_FLAG_Y | AXIS_CONSTRAINT_FLAG_Z,
	};
	
	DEFINE_ENUMERATOR_FLAG_OPERATORS(AXIS_CONSTRAINT_FLAGS);

	class CNodeTransform : public CNodeComponent
	{
	public:
		static const u32 HASH = Math::FNV1a_32(L"Data");

	public:
		struct Data : public CMemAlign<16>
		{
		private:
			friend class CNodeTransform;

		public:
			Data(u64 thisHash) : m_this(thisHash) { Reset(); }
			~Data() { }
			Data(const Data&) = default;
			Data(Data&&) = default;
			Data& operator = (const Data&) = default;
			Data& operator = (Data&&) = default;

		private:
			void Reset();

			void Update();
			void Calculate();
			void SetAsDirty(bool bForced);

			void SetParent(u64 parent);
			void RemoveChild(u64 child);

		public:
			inline bool IsValid() const { return !m_bForced; }

			// Family methods.
			inline const Data& GetParent() const { return GetTransform(m_parent); }
			inline size_t GetChildCount() const { return m_children.size(); }
			inline const Data& GetChild(size_t index) const { return GetTransform(m_children[index]); }

			// Constraints.
			inline void SetPositionConstraints(AXIS_CONSTRAINT_FLAGS constraints) { m_positionConstraints = constraints; }
			inline void SetEulerConstraints(AXIS_CONSTRAINT_FLAGS constraints) { m_eulerConstraints = constraints; }
			inline void SetScaleConstraints(AXIS_CONSTRAINT_FLAGS constraints) { m_scaleConstraints = constraints; }
			
			// Directional vector inline methods.
			inline Math::SIMDVector GetRight() const { return m_right; }
			inline Math::SIMDVector GetLeft() const { return -m_right; }
		
			inline Math::SIMDVector GetUp() const { return m_up; }
			inline Math::SIMDVector GetDown() const { return -m_up; }
		
			inline Math::SIMDVector GetForward() const { return m_forward; }
			inline Math::SIMDVector GetBackward() const { return -m_forward; }

			// World space transformation methods.
			inline const Math::SIMDVector& GetPosition() const { return m_position; }
			inline const Math::SIMDVector& GetEuler() const { return m_euler; }
			inline const Math::SIMDQuaternion& GetRotation() const { return m_rotation; }
			inline const Math::SIMDVector& GetScale() const { return m_scale; }

			void SetPosition(const Math::SIMDVector& position);
			void SetEuler(const Math::SIMDVector& euler);
			void SetRotation(const Math::SIMDQuaternion& rotation);
			void SetScale(const Math::SIMDVector& scale);

			inline const Math::SIMDMatrix& GetWorldMatrix() const { return m_worldMatrix; }

			// Local space transformation methods.
			inline const Math::SIMDVector& GetLocalPosition() const { return m_localPosition; }
			inline const Math::SIMDVector& GetLocalEuler() const { return m_localEuler; }
			inline const Math::SIMDQuaternion& GetLocalRotation() const { return m_localRotation; }
			inline const Math::SIMDVector& GetLocalScale() const { return m_localScale; }

			void SetLocalPosition(const Math::SIMDVector& localPosition);
			void SetLocalEuler(const Math::SIMDVector& localEuler);
			void SetLocalRotation(const Math::SIMDQuaternion& localRotation);
			void SetLocalScale(const Math::SIMDVector& localScale);

			inline const Math::SIMDMatrix& GetLocalMatrix() const { return m_localMatrix; }

		private:
			bool m_bDirty;
			bool m_bForced;

			AXIS_CONSTRAINT_FLAGS m_positionConstraints;
			AXIS_CONSTRAINT_FLAGS m_eulerConstraints;
			AXIS_CONSTRAINT_FLAGS m_scaleConstraints;

			u32 m_layer;

			Math::SIMDVector m_right;
			Math::SIMDVector m_up;
			Math::SIMDVector m_forward;

			Math::SIMDVector m_position;
			Math::SIMDVector m_localPosition;
			Math::SIMDVector m_scale;
			Math::SIMDVector m_localScale;
			Math::SIMDVector m_euler;
			Math::SIMDVector m_localEuler;
			Math::SIMDQuaternion m_rotation;
			Math::SIMDQuaternion m_localRotation;

			Math::SIMDMatrix m_localMatrix;
			Math::SIMDMatrix m_worldMatrix;

			std::vector<u64> m_children;
			u64 m_parent = 0;
			u64 m_this = 0;
		};

	public:
		CNodeTransform();
		~CNodeTransform();
		CNodeTransform(const CNodeTransform&) = delete;
		CNodeTransform(CNodeTransform&&) = delete;
		CNodeTransform& operator = (const CNodeTransform&) = delete;
		CNodeTransform& operator = (CNodeTransform&&) = delete;
		
		static CNodeTransform& Get() { static CNodeTransform comp; return comp; }
		void Register();
		void Update() final;
		
	private:
		void QueueTransform(const Data& transform);

		void* AddToObject(u64 objHash) final;
		void* GetFromObject(u64 objHash) final;
		bool TryToGetFromObject(u64 objHash, void** ppData) final;
		void RemoveFromObject(u64 objHash) final;

	private:
		// Accessors.
		static Data& GetTransform(u64 hash) { return Get().m_dataMap.find(hash)->second; }

	private:
		u32 m_maxLayer;

		std::unordered_map<u64, Data> m_dataMap;
		std::vector<std::queue<u64>> m_queueList;
	};
};

#endif
