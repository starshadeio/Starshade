//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: UI/CUITransform.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CUITRANFORM_H
#define CUITRANFORM_H

#include <Math/CMathMatrix3x2.h>
#include <Math/CMathVector2.h>
#include <Math/CMathBounds2D.h>
#include <vector>

namespace UI
{
	class CUITransform
	{
	public:
		CUITransform(class CUIElement* pElement);
		virtual ~CUITransform();
		CUITransform(const CUITransform&) = delete;
		CUITransform(CUITransform&&) = delete;
		CUITransform& operator = (const CUITransform&) = delete;
		CUITransform& operator = (CUITransform&&) = delete;
		
		void Reset();
		void Update();

	protected:
		void Calculate();
		
		bool MarkAsDirty();
		void RemoveChild(CUITransform* pChild);

	public:
		inline class CUIElement* GetElement() { return m_pElement; }
		inline u32 GetLayer() const { return m_layer; }
		inline const Math::Vector2& GetVertex(u32 index) const { return m_verices[index]; }
		inline const Math::Bounds2D& GetBounds() const { return m_bounds; }
		inline const Math::Bounds2D& GetLocalBounds() const { return m_localBounds; }
		
		inline const Math::Matrix3x2& GetInternalMatrix() const { return m_internalMatrix; }
		inline const Math::Matrix3x2& GetFinalMatrix() const { return m_finalMatrix; }

		//---------------------------------------------------------------------------------------------
		// Family methods.
		//---------------------------------------------------------------------------------------------
		
		bool SetParent(CUITransform* pParent);
		const CUITransform* GetParent() const { return m_pParent; }

		const std::vector<CUITransform*>& GetChildren() const { return m_children; }

		//---------------------------------------------------------------------------------------------
		// Directional vector inline methods.
		//---------------------------------------------------------------------------------------------
		
		inline Math::Vector2 GetRight() const { return m_right; }
		inline Math::Vector2 GetLeft() const { return -m_right; }
		
		inline Math::Vector2 GetUp() const { return m_up; }
		inline Math::Vector2 GetDown() const { return -m_up; }

		//---------------------------------------------------------------------------------------------
		// World space transformation methods.
		//---------------------------------------------------------------------------------------------

		// Position.
		inline const Math::Vector2& GetPosition() const { return m_position; }

		inline bool SetPosition(const Math::Vector2& position)
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

			return MarkAsDirty();
		}

		// Rotation.
		inline float GetRotation() const { return m_rotation; }

		inline bool SetRotation(float rotation)
		{
			m_rotation = Math::WrapRadians(rotation);

			if(m_pParent == nullptr)
			{
				m_localRotation = m_rotation;
			}
			else
			{
				m_localRotation = Math::WrapRadians(m_rotation - m_pParent->GetRotation());
			}

			return MarkAsDirty();
		}

		// Scale.
		inline const Math::Vector2& GetScale() const { return m_scale; }

		inline bool SetScale(const Math::Vector2& scale)
		{
			m_scale = scale;

			if(m_pParent == nullptr)
			{
				m_localScale = m_scale;
			}
			else
			{
				m_localScale = Math::Vector2::PointwiseQuotient(m_scale, m_pParent->GetScale());
			}

			return MarkAsDirty();
		}

		// Size.
		inline const Math::Vector2& GetSize() const { return m_size; }

		inline bool SetSize(const Math::Vector2& size)
		{
			m_size = size;
			return MarkAsDirty();
		}

		// Pivot.
		inline const Math::Vector2& GetPivot() const { return m_pivot; }

		inline bool SetPivot(const Math::Vector2& pivot)
		{
			m_pivot = pivot;
			return MarkAsDirty();
		}

		// World matrix.
		inline const Math::Matrix3x2& GetWorldMatrix() const { return m_worldMatrix; }
		
		//---------------------------------------------------------------------------------------------
		// Local space transformation methods.
		//---------------------------------------------------------------------------------------------
		
		// Local position.
		inline const Math::Vector2& GetLocalPosition() const { return m_localPosition; }

		inline bool SetLocalPosition(const Math::Vector2& localPosition)
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

			return MarkAsDirty();
		}
		
		// Local rotation.
		inline float GetLocalRotation() const { return m_localRotation; }

		inline bool SetLocalRotation(float localRotation)
		{
			localRotation = Math::WrapRadians(localRotation);

			if(m_pParent == nullptr)
			{
				m_rotation = localRotation;
			}
			else
			{
				m_rotation = Math::WrapRadians(m_pParent->GetRotation() + localRotation);
			}
			
			m_localRotation = localRotation;

			return MarkAsDirty();
		}

		// Local scale.
		inline const Math::Vector2& GetLocalScale() const { return m_localScale; }

		inline bool SetLocalScale(const Math::Vector2& localScale)
		{
			if(m_pParent == nullptr)
			{
				m_scale = localScale;
			}
			else
			{
				m_scale = Math::Vector2::PointwiseProduct(m_pParent->GetScale(), localScale);
			}

			m_localScale = localScale;

			return MarkAsDirty();
		}

		// Local matrix
		inline const Math::Matrix3x2& GetLocalMatrix() const { return m_localMatrix; }
		
	private:
		bool m_bDirty;
		u32 m_layer;

		Math::Vector2 m_position;
		Math::Vector2 m_localPosition;
		float m_rotation;
		float m_localRotation;
		Math::Vector2 m_scale;
		Math::Vector2 m_localScale;

		Math::Vector2 m_right;
		Math::Vector2 m_up;

		Math::Vector2 m_pivot;
		Math::Vector2 m_size;

		Math::Vector2 m_verices[4];
		Math::Bounds2D m_bounds;
		Math::Bounds2D m_localBounds;

		Math::Matrix3x2 m_internalMatrix;
		Math::Matrix3x2 m_worldMatrix;
		Math::Matrix3x2 m_localMatrix;
		Math::Matrix3x2 m_finalMatrix;

		std::vector<CUITransform*> m_children;
		CUITransform* m_pParent;

		class CUIElement* m_pElement;
	};
};

#endif
