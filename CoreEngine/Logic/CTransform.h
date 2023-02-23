//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Engine
//
// File: Logic/CTransform.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CTRANSFORM_H
#define CTRANSFORM_H

#include "../Objects/CVComponent.h"
#include "../Objects/CCompQueue.h"
#include "../Utilities/CMemAlign.h"
#include "../Math/CSIMDMatrix.h"
#include <mutex>
#include <vector>

namespace Logic
{
	class CTransform;

	typedef CCompQueue<CTransform> CTransformQueue;

	// This is legacy to CNodeTransform, and will eventually be replaced by it.
	class CTransform : public CMemAlign<16>, public CVComponent {
	public:
		CTransform(const CVObject* pObject);
		virtual ~CTransform();
		CTransform(const CTransform&) = delete;
		CTransform(CTransform&&) = delete;
		CTransform& operator = (const CTransform&) = delete;
		CTransform& operator = (CTransform&&) = delete;
		
		void Reset() final;
		void Update() final;

	protected:
		virtual void Calculate();

		virtual void SetAsDirty(bool bForced);
		void RemoveChild(CTransform* pChild);

	public:
		inline void AddOnUpdateComponent(CVComponent* pComp) { m_compUpdateList.push_back(pComp); }

		inline bool IsValid() const { return !m_bForced; }
		inline u32 GetLayer() const { return m_layer; }

		//---------------------------------------------------------------------------------------------
		// Family methods.
		//---------------------------------------------------------------------------------------------
		
		virtual void SetParent(CTransform* pParent);
		const CTransform* GetParent() const { return m_pParent; }

		const std::vector<CTransform*>& GetChildren() const { return m_children; }

		//---------------------------------------------------------------------------------------------
		// Directional vector inline methods.
		//---------------------------------------------------------------------------------------------
		
		inline Math::SIMDVector GetRight() const { return m_right; }
		inline Math::SIMDVector GetLeft() const { return -m_right; }
		
		inline Math::SIMDVector GetUp() const { return m_up; }
		inline Math::SIMDVector GetDown() const { return -m_up; }
		
		inline Math::SIMDVector GetForward() const { return m_forward; }
		inline Math::SIMDVector GetBackward() const { return -m_forward; }
		
		//---------------------------------------------------------------------------------------------
		// World space transformation methods.
		//---------------------------------------------------------------------------------------------

		// Position.
		inline const Math::SIMDVector& GetPosition() const { return m_position; }

		inline void SetPosition(const Math::SIMDVector& position)
		{
			m_position = position;

			if(m_pParent == nullptr)
			{
				m_localPosition = m_position;
			}
			else
			{
				m_localPosition = m_position - m_pParent->GetPosition();
			}

			SetAsDirty(true);
		}
		
		// Euler.
		inline const Math::SIMDVector& GetEuler() const { return m_euler; }

		inline void SetEuler(const Math::SIMDVector& euler)
		{
			m_euler = euler;
			m_rotation = m_rotation.RotateEuler(m_euler);

			if(m_pParent == nullptr)
			{
				m_localEuler = m_euler;
				m_localRotation = m_rotation;
			}
			else
			{
				SetLocalRotation(Math::SIMDQuaternion::Concatenate(m_rotation, Math::SIMDQuaternion::Conjugate(m_pParent->GetRotation())));
			}
			
			SetAsDirty(true);
		}
		
		// Rotation.
		inline const Math::SIMDQuaternion& GetRotation() const { return m_rotation; }

		inline void SetRotation(const Math::SIMDQuaternion& rotation)
		{
			m_rotation = rotation;
			m_euler = m_rotation.ToEuler();
			
			if(m_pParent == nullptr) {
				m_localRotation = m_rotation;
				m_localEuler = m_euler;
			}
			else
			{
				SetLocalRotation(Math::SIMDQuaternion::Concatenate(m_rotation, Math::SIMDQuaternion::Conjugate(m_pParent->GetRotation())));
			}
			
			SetAsDirty(true);
		}

		// Scale.
		inline const Math::SIMDVector& GetScale() const { return m_scale; }

		inline void SetScale(const Math::SIMDVector& scale)
		{
			m_scale = scale;

			if(m_pParent == nullptr)
			{
				m_localScale = m_scale;
			}
			else
			{
				m_localScale = Math::SIMDVector::PointwiseQuotient(m_scale, m_pParent->GetScale());
			}
			
			SetAsDirty(true);
		}
		
		// World matrix.
		inline const Math::SIMDMatrix& GetWorldMatrix() const { return m_worldMatrix; }
		
		//---------------------------------------------------------------------------------------------
		// Local space transformation methods.
		//---------------------------------------------------------------------------------------------

		// Local position.
		inline const Math::SIMDVector& GetLocalPosition() const { return m_localPosition; }

		inline void SetLocalPosition(const Math::SIMDVector& localPosition)
		{
			if(m_pParent == nullptr)
			{
				m_position = localPosition;
			}
			else
			{
				m_position = m_pParent->GetPosition() + localPosition;
			}

			m_localPosition = localPosition;
			
			SetAsDirty(true);
		}
		
		// Local euler.
		inline const Math::SIMDVector& GetLocalEuler() const { return m_localEuler; }

		inline void SetLocalEuler(const Math::SIMDVector& localEuler)
		{
			if(m_pParent == nullptr)
			{
				m_euler = localEuler;
				m_rotation = m_rotation.RotateEuler(m_euler);
			}
			else
			{
				m_euler = m_pParent->GetEuler() + localEuler;
				m_rotation = m_rotation.RotateEuler(m_euler);
			}
			
			m_localEuler = localEuler;
			m_localRotation = m_localRotation.RotateEuler(m_localEuler);
			
			SetAsDirty(true);
		}

		// Local rotation.
		inline const Math::SIMDQuaternion& GetLocalRotation() const { return m_localRotation; }

		inline void SetLocalRotation(const Math::SIMDQuaternion& localRotation)
		{
			if(m_pParent == nullptr)
			{
				m_rotation = localRotation;
				m_euler = m_rotation.ToEuler();
			}
			else
			{
				m_rotation = m_pParent->GetRotation() * localRotation;
				m_euler = m_rotation.ToEuler();
			}

			m_localRotation = localRotation;
			m_localEuler = m_localRotation.ToEuler();
			
			SetAsDirty(true);
		}

		// Local scale.
		inline const Math::SIMDVector& GetLocalScale() const { return m_localScale; }

		inline void SetLocalScale(const Math::SIMDVector& localScale)
		{
			if(m_pParent == nullptr)
			{
				m_scale = localScale;
			}
			else
			{
				m_scale = Math::SIMDVector::PointwiseProduct(m_pParent->GetScale(), localScale);
			}

			m_localScale = localScale;
			
			SetAsDirty(true);
		}

		// Local matrix
		inline const Math::SIMDMatrix& GetLocalMatrix() const { return m_localMatrix; }
		
		//---------------------------------------------------------------------------------------------
		// Interpolation methods (These should only be used internally by the rigidbody component).
		//---------------------------------------------------------------------------------------------

		inline void SetRigidbodyPosition(const Math::SIMDVector& position)
		{
			m_position = position;

			if(m_pParent == nullptr)
			{
				m_localPosition = m_position;
			}
			else
			{
				m_localPosition = m_position - m_pParent->GetPosition();
			}

			SetAsDirty(false);
		}
		
		inline void InterpolateRigidbodyPosition(const Math::SIMDVector& from, const Math::SIMDVector& to, float t)
		{
			SetRigidbodyPosition(Math::SIMDVector::Lerp(from, to, t));
		}

		inline void SetRigidbodyRotation(const Math::SIMDQuaternion& rotation)
		{
			m_rotation = rotation;
			m_euler = m_rotation.ToEuler();
			
			if(m_pParent == nullptr) {
				m_localRotation = m_rotation;
				m_localEuler = m_euler;
			} else {
				SetLocalRotation(Math::SIMDQuaternion::Concatenate(m_rotation, Math::SIMDQuaternion::Conjugate(m_pParent->GetRotation())));
			}

			SetAsDirty(false);
		}
		
		inline void InterpolateRigidbodyRotation(const Math::SIMDQuaternion& from, const Math::SIMDQuaternion& to, float t)
		{
			SetRigidbodyRotation(Math::SIMDQuaternion::Slerp(from, to, t));
		}

	protected:
		bool m_bDirty;
		bool m_bForced;
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

		std::vector<CTransform*> m_children;
		std::vector<CVComponent*> m_compUpdateList;
		CTransform* m_pParent;
	};
};

#endif
