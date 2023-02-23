//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Engine
//
// File: Logic/CTransform.h
//
//-------------------------------------------------------------------------------------------------

#include "CTransform.h"
#include "../Physics/CPhysics.h"
#include "../Objects/CVObject.h"
#include "../Application/CCoreManager.h"

namespace Logic
{
	CTransform::CTransform(const CVObject* pObject) : 
		CVComponent(nullptr),
		m_pParent(nullptr)
	{
		m_pObject = pObject;
		if(m_pObject) AddComponent();
		Reset();
	}

	CTransform::~CTransform()
	{
		if(m_pObject) RemoveComponent();
		m_pObject = nullptr;
	}

	void CTransform::Reset()
	{
		if(m_pParent)
		{
			m_pParent->RemoveChild(this);
		}

		m_bDirty = false;
		m_bForced = false;
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
		m_compUpdateList.clear();
		m_pParent = nullptr;
	}

	void CTransform::Update()
	{
		if(!m_bDirty) { return; }

		Calculate();

		// Create directional vectors from this transform's world matrix.
		m_right = m_worldMatrix.TransformNormal(Math::SIMD_VEC_RIGHT).Normalized();
		m_up = m_worldMatrix.TransformNormal(Math::SIMD_VEC_UP).Normalized();
		m_forward = m_worldMatrix.TransformNormal(Math::SIMD_VEC_FORWARD).Normalized();
		
		// Check if physics should be updated as well.
		if(m_bForced && m_pObject && m_pObject->HasPhysics())
		{
			Physics::CPhysics::Instance().MarkObjectAsDirty(m_pObject, m_worldMatrix);
		}

		for(CVComponent* pComp : m_compUpdateList)
		{
			(*pComp)();
		}

		m_bForced = false;
		m_bDirty = false;
	}
	
	//-----------------------------------------------------------------------------------------------
	// Internal methods.
	//-----------------------------------------------------------------------------------------------

	void CTransform::Calculate()
	{
		if(m_pParent)
		{ // Has parent.
			m_localMatrix = Math::SIMDMatrix::Scale(m_localScale);

			const float* localEuler = m_localEuler.ToFloat();
			m_localMatrix *= Math::SIMDMatrix::RotateZ(localEuler[2]);
			m_localMatrix *= Math::SIMDMatrix::RotateX(localEuler[0]);
			m_localMatrix *= Math::SIMDMatrix::RotateY(localEuler[1]);

			m_localMatrix *= Math::SIMDMatrix::Translate(m_localPosition);

			// Update world transforms.
			m_position = m_pParent->GetPosition() + m_localPosition;

			m_euler = m_pParent->GetEuler() + m_localEuler;
			m_rotation.RotateEuler(m_euler);

			m_scale = Math::SIMDVector::PointwiseProduct(m_pParent->GetScale(), m_localScale);

			m_worldMatrix = m_localMatrix * m_pParent->m_worldMatrix;
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

	void CTransform::SetAsDirty(bool bForced)
	{
		m_bForced |= bForced;
		if(m_bDirty) { return; }

		m_bDirty = true;

		App::CCoreManager::Instance().QueueTransformUpdate(this);
	}

	//-----------------------------------------------------------------------------------------------
	// Family methods.
	//-----------------------------------------------------------------------------------------------

	void CTransform::SetParent(CTransform* pParent)
	{
		if(m_pParent == pParent)
		{
			return;
		}

		if(m_pParent)
		{
			m_pParent->RemoveChild(this);
		}

		m_pParent = pParent;
		if(m_pParent)
		{
			m_pParent->m_children.push_back(this);
			m_layer = m_pParent->m_layer + 1;

			// Update local values.
			m_localPosition = m_position - m_pParent->GetPosition();
			m_localRotation = Math::SIMDQuaternion::Concatenate(m_rotation, Math::SIMDQuaternion::Conjugate(m_pParent->GetRotation()));
			m_localEuler = m_localRotation.ToEuler();
			m_localScale = Math::SIMDVector::PointwiseQuotient(m_scale, m_pParent->GetScale());
			
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

	void CTransform::RemoveChild(CTransform* pChild)
	{
		size_t i, j;
		for(i = 0, j = 0; i < m_children.size(); ++i)
		{
			if(m_children[i] != pChild)
			{
				m_children[j++] = m_children[i];
			}
		}

		m_children.resize(j);
	}
};
