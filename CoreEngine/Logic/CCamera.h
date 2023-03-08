//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Engine
//
// File: Logic/CCamera.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CCAMERA_H
#define CCAMERA_H

#include "CTransform.h"
#include "../Objects/CVComponent.h"
#include "../Math/CSIMDMatrix.h"
#include "../Math/CSIMDPlane.h"
#include "../Math/CMathFloat.h"
#include "../Math/CMathRect.h"

namespace Logic
{
	enum CAMERA_PROJ_MODE
	{
		CAMERA_PROJ_MODE_ORTHOGRAPHIC,
		CAMERA_PROJ_MODE_PERSPECTIVE,
	};
	
	class CCamera : public CMemAlign<16>, public CVComponent
	{
	public:
		struct Data
		{
			CAMERA_PROJ_MODE projMode;
			float fov;
			float nearView;
			float farView;
			float aspectRatio;
			float size;
			Math::Rect screenRect;
		};

	public:
		CCamera(const CVObject* pObject, CTransform* pTransform);
		CCamera(const CCamera&) = delete;
		CCamera(CCamera&&) = default;
		CCamera& operator = (const CCamera&) = delete;
		CCamera& operator = (CCamera&&) = default;

		void operator ()() final { Update(); }
		
		void SetupProjection(const Data& data);
		void SetupProjection(const Math::Rect& screenRect);
		void SetupProjection();
		void Update() final;

		bool CheckPoint(const Math::SIMDVector& pt) const;
		
		// Inline methods.
		inline void SetProjectionMode(CAMERA_PROJ_MODE projMode) { m_data.projMode = projMode; }
		inline CAMERA_PROJ_MODE GetProjectionMode() { return m_data.projMode; }

		inline void SetFOVDegrees(float degrees) { m_data.fov = degrees * Math::g_PiOver180; }
		inline float GetFOVDegrees() const { return m_data.fov * Math::g_180OverPi; }
		inline void SetFOVRadians(float radians) { m_data.fov = radians; }
		inline float GetFOVRadians() const { return m_data.fov; }
		
		inline void SetNearView(float nearView) { m_data.nearView = nearView; }
		inline float GetNearView() const { return m_data.nearView; }
		inline void SetFarView(float farView) { m_data.farView = farView; }
		inline float GetFarView() const { return m_data.farView; }
		
		inline void SetSize(float size) { m_data.size = size; }
		inline float GetSize() const { return m_data.size; }

		inline const Math::Rect& GetSreenRect() const { return m_data.screenRect; }
		inline float GetAspectRatio() const { return m_data.aspectRatio; }
		
		inline const Math::SIMDMatrix& GetProjectionMatrix() const { return m_projMatrix; }
		inline const Math::SIMDMatrix& GetProjectionMatrixInv() const { return m_projMatrixInv; }
		inline const Math::SIMDMatrix& GetViewMatrix() const { return m_viewMatrix; }
		inline const Math::SIMDMatrix& GetViewMatrixInv() const { return m_viewMatrixInv; }

		inline const CTransform* GetTransform() const { return m_pTransform; }
		inline const Math::SIMDPlane& GetFrustumPlane(size_t index) const { return m_frustumPlanes[index]; }

	private:
		void CalculateFrustumPlanes();

	private:
		Data m_data;

		Math::SIMDMatrix m_projMatrix;
		Math::SIMDMatrix m_projMatrixInv;
		Math::SIMDMatrix m_viewMatrix;
		Math::SIMDMatrix m_viewMatrixInv;

		Math::SIMDPlane m_frustumPlanes[6];

		CTransform* m_pTransform;
	};
};

#endif
