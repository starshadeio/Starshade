//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: UI/CUIFrame.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CUIFRAME_H
#define CUIFRAME_H

#include <Math/CMathMatrix3x2.h>
#include <Math/CMathBounds2D.h>
#include <Math/CMathRect.h>
#include <Objects/CVComponent.h>
#include <vector>
#include <functional>

namespace UI
{
	class CUIFrame : public CVComponent
	{
	public:
		struct Data
		{
			bool bActive = true;
			bool bAllowOverflow = true;

			Math::Vector2 position = 0.0f;
			Math::Vector2 size = 100.0f;
			Math::Vector2 rect[2] = { { 0.0f, 0.0f } , { 0.0f, 0.0f } };

			bool bUseMinWidth = false;
			bool bUseMinHeight = false;
			bool bUseMaxWidth = false;
			bool bUseMaxHeight = false;

			float minWidth = 0.0f;
			float minHeight = 0.0f;
			float maxWidth = 0.0f;
			float maxHeight = 0.0f;
			
			Math::Vector2 anchorMin = 0.5f;
			Math::Vector2 anchorMax = 0.5f;
			Math::Vector2 pivot = 0.5f;

			Math::Vector2 scale = 1.0f;
			float rotation = 0.0f;
			
			CUIFrame* pParent = nullptr;
		};

	public:
		CUIFrame(const CVObject* pObject, class CUICanvas* pCanvas);
		virtual ~CUIFrame();
		CUIFrame(const CUIFrame&) = delete;
		CUIFrame(CUIFrame&&) = delete;
		CUIFrame& operator = (const CUIFrame&) = delete;
		CUIFrame& operator = (CUIFrame&&) = delete;

		void Initialize() final;
		void Update() final;
		void DebugUpdate();

		bool IsIntersecting(const Math::Vector2& point, Math::Vector2* pRelativePt = nullptr) const;

		// Accessors.
		inline class CUICanvas* GetCanvas() const { return m_pCanvas; }

		inline u32 GetLayer() const { return m_layer; }
		inline const Math::Bounds2D& GetBounds() const { return m_bounds; }

		inline const Math::Vector2& GetRight() const { return m_right; }
		inline Math::Vector2 GetLeft() const { return -m_right; }
		inline const Math::Vector2& GetUp() const { return m_up; }
		inline Math::Vector2 GetDown() const { return -m_up; }

		inline const Math::Matrix3x2& GetScaleMatrix() const { return m_scaleMatrix; }
		inline const Math::Matrix3x2& GetRotationMatrix() const { return m_rotationMatrix; }
		inline const Math::Matrix3x2& GetInternalMatrix() const { return m_internalMatrix; }
		inline const Math::Matrix3x2& GetWorldMatrix() const { return m_worldMatrix; }
		
		inline bool AllowOverlap() const { return m_data.bAllowOverflow; }

		// Modifier.
		inline void SetData(const Data& data) { m_data = data; }
		inline void SetOnUpdate(std::function<void()> onUpdate) { m_onUpdate = onUpdate; }
		inline void SetOnActiveChange(std::function<void(bool)> onActiveChange) { m_onActiveChange = onActiveChange; }

		inline void SetAllowOverlap(bool bAllowOverlap) { m_data.bAllowOverflow = bAllowOverlap; }

		//---------------------------------------------------------------------------------------------
		// (De)activation.
		//---------------------------------------------------------------------------------------------

		inline bool IsActive() const { return m_bActiveSelf && m_bActive && !m_bHidden; }
		bool SetActive(bool bActive);
		
		//---------------------------------------------------------------------------------------------
		// Transform.
		//---------------------------------------------------------------------------------------------

		// Position.
		inline const Math::Vector2& GetPosition() const { return m_data.position; }
		
		inline bool SetPosition(const Math::Vector2& position)
		{
			m_data.position = position;
			return MarkAsDirty();
		}

		// Size.
		inline const Math::Vector2& GetSize() const { return m_data.size; }
		
		inline bool SetSize(const Math::Vector2& size)
		{
			m_data.size = size;
			return MarkAsDirty();
		}
		
		// Rectangle.
		inline float GetLeftExtent() const { return m_data.rect[0].x; }
		inline float GetRightExtent() const { return m_data.rect[0].y; }
		inline float GetTopExtent() const { return m_data.rect[0].x; }
		inline float GetBottomExtent() const { return m_data.rect[0].y; }

		inline float GetMinWidth() const { return m_data.minWidth; }
		inline float GetMaxWidth() const { return m_data.maxWidth; }
		inline float GetMinHeight() const { return m_data.minHeight; }
		inline float GetMaxHeight() const { return m_data.maxHeight; }

		inline Math::RectFlt GetRect() const { return Math::RectFlt(m_data.rect[0].x, m_data.rect[1].x, m_data.rect[0].y, m_data.rect[1].y); }
		
		inline bool SetLeftExtent(float left) { m_data.rect[0].x = left; return MarkAsDirty(); }
		inline bool SetRightExtent(float right) { m_data.rect[0].y = right; return MarkAsDirty(); }
		inline bool SetTopExtent(float top) { m_data.rect[0].x = top; return MarkAsDirty(); }
		inline bool SetBottomExtent(float bottom) { m_data.rect[0].y = bottom; return MarkAsDirty(); }

		inline bool SetMinWidth(float minWidth) { m_data.bUseMinWidth = true; m_data.minWidth = minWidth; return MarkAsDirty(); }
		inline bool SetMaxWidth(float maxWidth) { m_data.bUseMaxWidth = true; m_data.maxWidth = maxWidth; return MarkAsDirty(); }
		inline bool SetMinHeight(float minHeight) { m_data.bUseMinHeight = true; m_data.minHeight = minHeight; return MarkAsDirty(); }
		inline bool SetMaxHeight(float maxHeight) { m_data.bUseMaxHeight = true; m_data.maxHeight = maxHeight; return MarkAsDirty(); }

		inline bool SetRect(const Math::RectFlt& size)
		{
			m_data.rect[0].x = size.left;
			m_data.rect[1].x = size.top;
			m_data.rect[0].y = size.right;
			m_data.rect[1].y = size.bottom;
			return MarkAsDirty();
		}
		
		// Scale.
		inline const Math::Vector2& GetScale() const { return m_data.scale; }
		
		inline bool SetScale(const Math::Vector2& scale)
		{
			m_data.scale = scale;
			return MarkAsDirty();
		}
		
		// Rotation.
		inline float GetRotation() const { return m_data.rotation; }
		
		inline bool SetRotation(float rotation)
		{
			m_data.rotation = rotation;
			return MarkAsDirty();
		}
		
		//---------------------------------------------------------------------------------------------
		// Anchor/Pivot.
		//---------------------------------------------------------------------------------------------
		
		// Anchor Min.
		inline const Math::Vector2& GetAnchorMin() const { return m_data.anchorMin; }
		
		inline bool SetAnchorMin(const Math::Vector2& anchorMin)
		{
			m_data.anchorMin = anchorMin;
			return MarkAsDirty();
		}
		
		// Anchor Max.
		inline const Math::Vector2& GetAnchorMax() const { return m_data.anchorMax; }
		
		inline bool SetAnchorMax(const Math::Vector2& anchorMax)
		{
			m_data.anchorMax = anchorMax;
			return MarkAsDirty();
		}

		// Pivot.
		inline const Math::Vector2& GetPivot() const { return m_data.pivot; }
		
		inline bool SetPivot(const Math::Vector2& pivot)
		{
			m_data.pivot = pivot;
			return MarkAsDirty();
		}

		bool MarkAsDirty();

	private:
		bool m_bActive;
		bool m_bActiveSelf;
		bool m_bHidden;

		bool m_bDirty;
		u32 m_layer;

		Data m_data;
		
		Math::Vector2 m_right;
		Math::Vector2 m_up;

		Math::Bounds2D m_bounds;
		Math::Matrix3x2 m_scaleMatrix;
		Math::Matrix3x2 m_rotationMatrix;
		Math::Matrix3x2 m_internalMatrix;
		Math::Matrix3x2 m_internalMatrixInv;
		Math::Matrix3x2 m_worldMatrix;

		std::function<void()> m_onUpdate;
		std::function<void(bool)> m_onActiveChange;

		std::vector<CUIFrame*> m_children;
		class CUICanvas* m_pCanvas;
	};
};

#endif
