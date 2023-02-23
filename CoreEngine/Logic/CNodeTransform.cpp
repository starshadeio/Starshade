//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Engine
//
// File: Logic/CNodeTransform.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CNodeTransform.h"
#include "../Application/CCoreManager.h"

namespace Logic
{
	//-----------------------------------------------------------------------------------------------
	// CNodeTransform::Data
	//-----------------------------------------------------------------------------------------------

	void CNodeTransform::Data::Reset()
	{
		if(m_parent)
		{
			GetTransform(m_parent).RemoveChild(m_this);
		}

		m_bDirty = false;
		m_bForced = false;
		
		m_positionConstraints = AXIS_CONSTRAINT_FLAG_NONE;
		m_eulerConstraints = AXIS_CONSTRAINT_FLAG_NONE;
		m_scaleConstraints = AXIS_CONSTRAINT_FLAG_NONE;

		m_layer = 0;

		m_right = Math::SIMD_VEC_RIGHT;
		m_up = Math::SIMD_VEC_UP;
		m_forward = Math::SIMD_VEC_FORWARD;

		m_position = Math::SIMD_VEC_ZERO;
		m_localPosition = Math::SIMD_VEC_ZERO;
		m_scale = Math::SIMD_VEC_ONE;
		m_localScale = Math::SIMD_VEC_ONE;
		m_euler = Math::SIMD_VEC_ZERO;
		m_localEuler = Math::SIMD_VEC_ZERO;
		m_rotation = Math::SIMD_QUAT_IDENTITY;
		m_localRotation = Math::SIMD_QUAT_IDENTITY;

		m_localMatrix = Math::SIMD_MTX4X4_IDENTITY;
		m_worldMatrix = Math::SIMD_MTX4X4_IDENTITY;

		m_children.clear();
		m_parent = 0;
	}
	
	//----------------------------------------------
	// Update methods.
	//----------------------------------------------
	
	void CNodeTransform::Data::Update()
	{
		if(!m_bDirty) { return; }

		Calculate();

		// Create directional vectors from this transform's world matrix.
		m_right = m_worldMatrix.TransformNormal(Math::SIMD_VEC_RIGHT).Normalized();
		m_up = m_worldMatrix.TransformNormal(Math::SIMD_VEC_UP).Normalized();
		m_forward = m_worldMatrix.TransformNormal(Math::SIMD_VEC_FORWARD).Normalized();
		
		// Check if physics should be updated as well.
		/*if(m_bForced && m_pObject && m_pObject->HasPhysics())
		{
			Physics::CPhysics::Instance().MarkObjectAsDirty(m_pObject, m_worldMatrix);
		}

		for(CVComponent* pComp : m_compUpdateList)
		{
			(*pComp)();
		}*/

		m_bForced = false;
		m_bDirty = false;
	}
	
	void CNodeTransform::Data::Calculate()
	{
		if(m_parent)
		{ // Has parent.
			m_localMatrix = Math::SIMDMatrix::Scale(m_localScale);

			const float* localEuler = m_localEuler.ToFloat();
			m_localMatrix *= Math::SIMDMatrix::RotateZ(localEuler[2]);
			m_localMatrix *= Math::SIMDMatrix::RotateX(localEuler[0]);
			m_localMatrix *= Math::SIMDMatrix::RotateY(localEuler[1]);

			m_localMatrix *= Math::SIMDMatrix::Translate(m_localPosition);

			// Update world transforms.
			m_position = GetTransform(m_parent).GetPosition() + m_localPosition;

			m_euler = GetTransform(m_parent).GetEuler() + m_localEuler;
			m_rotation.RotateEuler(m_euler);

			m_scale = Math::SIMDVector::PointwiseProduct(GetTransform(m_parent).GetScale(), m_localScale);

			m_worldMatrix = m_localMatrix * GetTransform(m_parent).m_worldMatrix;
		}
		else
		{ // At root.
			m_worldMatrix = Math::SIMDMatrix::Scale(m_scale);

			const float* euler = m_euler.ToFloat();
			m_worldMatrix *= Math::SIMDMatrix::RotateZ(euler[2]);
			m_worldMatrix *= Math::SIMDMatrix::RotateX(euler[0]);
			m_worldMatrix *= Math::SIMDMatrix::RotateY(euler[1]);

			m_worldMatrix *= Math::SIMDMatrix::Translate(m_position);
			m_localMatrix = m_worldMatrix;
		}
	}

	void CNodeTransform::Data::SetAsDirty(bool bForced)
	{
		m_bForced |= bForced;
		if(m_bDirty) { return; }

		m_bDirty = true;

		Get().QueueTransform(*this);
	}

	//----------------------------------------------
	// Family methods.
	//----------------------------------------------
	
	void CNodeTransform::Data::SetParent(u64 parent)
	{
		if(parent == m_parent) return;

		if(m_parent)
		{
			GetTransform(m_parent).RemoveChild(m_this);
		}

		m_parent = parent;
		if(m_parent)
		{
			auto& parentRef = GetTransform(m_parent);
			
			parentRef.m_children.push_back(m_this);
			m_layer = parentRef.m_layer + 1;

			// Update local values.
			m_localPosition = m_position - parentRef.GetPosition();
			m_localRotation = Math::SIMDQuaternion::Concatenate(m_rotation, Math::SIMDQuaternion::Conjugate(parentRef.GetRotation()));
			m_localEuler = m_localRotation.ToEuler();
			m_localScale = Math::SIMDVector::PointwiseQuotient(m_scale, parentRef.GetScale());
			
			// Calculate new local matrix.
			m_localMatrix = Math::SIMDMatrix::Scale(m_localScale);

			const float* localEuler = m_localEuler.ToFloat();
			m_localMatrix *= Math::SIMDMatrix::RotateZ(localEuler[2]);
			m_localMatrix *= Math::SIMDMatrix::RotateX(localEuler[0]);
			m_localMatrix *= Math::SIMDMatrix::RotateY(localEuler[1]);

			m_localMatrix *= Math::SIMDMatrix::Translate(m_localPosition);
		}
		else
		{
			m_layer = 0;
			
			m_localPosition = m_position;
			m_localRotation = m_rotation;
			m_localEuler = m_euler;
			m_localScale = m_scale;
			m_localMatrix = m_worldMatrix;
		}
	}

	void CNodeTransform::Data::RemoveChild(u64 child)
	{
		size_t i, j;
		for(i = 0, j = 0; i < m_children.size(); ++i)
		{
			if(m_children[i] != child)
			{
				m_children[j++] = m_children[i];
			}
		}

		m_children.resize(j);
	}
	
	//----------------------------------------------
	// World transform methods.
	//----------------------------------------------
	
	void CNodeTransform::Data::SetPosition(const Math::SIMDVector& position)
	{
		m_position = Math::SIMDVector::Lerp(m_position, position, Math::SIMDVector(
			(m_positionConstraints & AXIS_CONSTRAINT_FLAG_X) ? 0.0f : 1.0f,
			(m_positionConstraints & AXIS_CONSTRAINT_FLAG_Y) ? 0.0f : 1.0f,
			(m_positionConstraints & AXIS_CONSTRAINT_FLAG_Z) ? 0.0f : 1.0f
		));

		if(m_parent == 0)
		{
			m_localPosition = m_position;
		}
		else
		{
			m_localPosition = m_position - GetTransform(m_parent).GetPosition();
		}

		SetAsDirty(true);
	}

	void CNodeTransform::Data::SetEuler(const Math::SIMDVector& euler)
	{
		m_euler = Math::SIMDVector::Lerp(m_euler, euler, Math::SIMDVector(
			(m_eulerConstraints & AXIS_CONSTRAINT_FLAG_X) ? 0.0f : 1.0f,
			(m_eulerConstraints & AXIS_CONSTRAINT_FLAG_Y) ? 0.0f : 1.0f,
			(m_eulerConstraints & AXIS_CONSTRAINT_FLAG_Z) ? 0.0f : 1.0f
		));

		m_rotation = m_rotation.RotateEuler(m_euler);
		
		if(m_parent == 0)
		{
			m_localEuler = m_euler;
			m_localRotation = m_rotation;
		}
		else
		{
			SetLocalRotation(Math::SIMDQuaternion::Concatenate(m_rotation, Math::SIMDQuaternion::Conjugate(GetTransform(m_parent).GetRotation())));
		}
			
		SetAsDirty(true);
	}

	void CNodeTransform::Data::SetRotation(const Math::SIMDQuaternion& rotation)
	{
		if(m_eulerConstraints == AXIS_CONSTRAINT_FLAG_NONE)
		{
			m_rotation = rotation;
			m_euler = m_rotation.ToEuler();
		}
		else
		{
			m_euler = Math::SIMDVector::Lerp(m_euler, rotation.ToEuler(), Math::SIMDVector(
				(m_eulerConstraints & AXIS_CONSTRAINT_FLAG_X) ? 0.0f : 1.0f,
				(m_eulerConstraints & AXIS_CONSTRAINT_FLAG_Y) ? 0.0f : 1.0f,
				(m_eulerConstraints & AXIS_CONSTRAINT_FLAG_Z) ? 0.0f : 1.0f
			));

			m_rotation = m_rotation.RotateEuler(m_euler);
		}
			
		if(m_parent == 0)
		{
			m_localRotation = m_rotation;
			m_localEuler = m_euler;
		}
		else
		{
			SetLocalRotation(Math::SIMDQuaternion::Concatenate(m_rotation, Math::SIMDQuaternion::Conjugate(GetTransform(m_parent).GetRotation())));
		}
			
		SetAsDirty(true);
	}

	void CNodeTransform::Data::SetScale(const Math::SIMDVector& scale)
	{
		m_scale = Math::SIMDVector::Lerp(m_scale, scale, Math::SIMDVector(
			(m_scaleConstraints & AXIS_CONSTRAINT_FLAG_X) ? 0.0f : 1.0f,
			(m_scaleConstraints & AXIS_CONSTRAINT_FLAG_Y) ? 0.0f : 1.0f,
			(m_scaleConstraints & AXIS_CONSTRAINT_FLAG_Z) ? 0.0f : 1.0f
		));

		if(m_parent == 0)
		{
			m_localScale = m_scale;
		}
		else
		{
			m_localScale = Math::SIMDVector::PointwiseQuotient(m_scale, GetTransform(m_parent).GetScale());
		}
			
		SetAsDirty(true);
	}

	//----------------------------------------------
	// Local transform methods.
	//----------------------------------------------

	void CNodeTransform::Data::SetLocalPosition(const Math::SIMDVector& localPosition)
	{
		Math::SIMDVector localP = Math::SIMDVector::Lerp(Math::SIMD_VEC_ZERO, localPosition, Math::SIMDVector(
			(m_positionConstraints & AXIS_CONSTRAINT_FLAG_X) ? 0.0f : 1.0f,
			(m_positionConstraints & AXIS_CONSTRAINT_FLAG_Y) ? 0.0f : 1.0f,
			(m_positionConstraints & AXIS_CONSTRAINT_FLAG_Z) ? 0.0f : 1.0f
		));

		if(m_parent == 0)
		{
			m_position = localP;
		}
		else
		{
			m_position = GetTransform(m_parent).GetPosition() + localP;
		}

		m_localPosition = localP;
			
		SetAsDirty(true);
	}

	void CNodeTransform::Data::SetLocalEuler(const Math::SIMDVector& localEuler)
	{
		Math::SIMDVector localE = Math::SIMDVector::Lerp(Math::SIMD_VEC_ZERO, localEuler, Math::SIMDVector(
			(m_eulerConstraints & AXIS_CONSTRAINT_FLAG_X) ? 0.0f : 1.0f,
			(m_eulerConstraints & AXIS_CONSTRAINT_FLAG_Y) ? 0.0f : 1.0f,
			(m_eulerConstraints & AXIS_CONSTRAINT_FLAG_Z) ? 0.0f : 1.0f
		));

		if(m_parent == 0)
		{
			m_euler = localE;
			m_rotation = m_rotation.RotateEuler(m_euler);
		}
		else
		{
			m_euler = GetTransform(m_parent).GetEuler() + localE;
			m_rotation = m_rotation.RotateEuler(m_euler);
		}
			
		m_localEuler = localE;
		m_localRotation = m_localRotation.RotateEuler(m_localEuler);
			
		SetAsDirty(true);
	}

	void CNodeTransform::Data::SetLocalRotation(const Math::SIMDQuaternion& localRotation)
	{
		if(m_eulerConstraints != AXIS_CONSTRAINT_FLAG_NONE)
		{
			SetLocalEuler(localRotation.ToEuler());
			return;
		}

		if(m_parent == 0)
		{
			m_rotation = localRotation;
			m_euler = m_rotation.ToEuler();
		}
		else
		{
			m_rotation = GetTransform(m_parent).GetRotation() * localRotation;
			m_euler = m_rotation.ToEuler();
		}

		m_localRotation = localRotation;
		m_localEuler = m_localRotation.ToEuler();
			
		SetAsDirty(true);
	}

	void CNodeTransform::Data::SetLocalScale(const Math::SIMDVector& localScale)
	{
		Math::SIMDVector localS = Math::SIMDVector::Lerp(Math::SIMD_VEC_ONE, localScale, Math::SIMDVector(
			(m_scaleConstraints & AXIS_CONSTRAINT_FLAG_X) ? 0.0f : 1.0f,
			(m_scaleConstraints & AXIS_CONSTRAINT_FLAG_Y) ? 0.0f : 1.0f,
			(m_scaleConstraints & AXIS_CONSTRAINT_FLAG_Z) ? 0.0f : 1.0f
		));

		if(m_parent == 0)
		{
			m_scale = localS;
		}
		else
		{
			m_scale = Math::SIMDVector::PointwiseProduct(GetTransform(m_parent).GetScale(), localS);
		}

		m_localScale = localS;
			
		SetAsDirty(true);
	}
	
	//-----------------------------------------------------------------------------------------------
	// CNodeTransform
	//-----------------------------------------------------------------------------------------------

	CNodeTransform::CNodeTransform() : 
		CNodeComponent(HASH),
		m_maxLayer(0)
	{
	}

	CNodeTransform::~CNodeTransform()
	{
	}
	
	void CNodeTransform::Register()
	{
		App::CCoreManager::Instance().NodeRegistry().RegisterComponent<CNodeTransform>();
	}
	
	void CNodeTransform::Update()
	{
		for(size_t i = 0; i < m_maxLayer; ++i)
		{
			while(!m_queueList[i].empty())
			{
				GetTransform(m_queueList[i].front()).Update();
				m_queueList[i].pop();
			}
		}

		m_maxLayer = 0;
	}
	
	//---------------------------------------------
	// Utility methods.
	//---------------------------------------------

	void CNodeTransform::QueueTransform(const Data& transform)
	{
		m_maxLayer = std::max(m_maxLayer, transform.m_layer + 1);

		while(m_queueList.size() <= m_maxLayer - 1)
		{
			m_queueList.push_back(std::queue<u64>());
		}

		m_queueList[transform.m_layer].push(transform.m_this);
	}
	
	//---------------------------------------------
	// Object methods.
	//---------------------------------------------
	
	void* CNodeTransform::AddToObject(u64 objHash)
	{
		return &m_dataMap.insert({ objHash, Data(objHash) }).first->second;
	}
	
	void* CNodeTransform::GetFromObject(u64 objHash)
	{
		return &m_dataMap.find(objHash)->second;
	}

	bool CNodeTransform::TryToGetFromObject(u64 objHash, void** ppData)
	{
		auto elem = m_dataMap.find(objHash);
		if(elem == m_dataMap.end()) return false;
		*ppData = &elem->second;
		return true;
	}

	void CNodeTransform::RemoveFromObject(u64 objHash)
	{
		m_dataMap.erase(objHash);
	}
};
