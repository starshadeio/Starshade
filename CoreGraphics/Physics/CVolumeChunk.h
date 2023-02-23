//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Physics/CVolumeChunk.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CVOLUMECHUNK_H
#define CVOLUMECHUNK_H

#include <Physics/CVolume.h>
#include <Physics/CPhysicsData.h>
#include <Math/CMathVector3.h>
#include <Math/CMathVectorInt3.h>

namespace Universe
{
	class CChunkNode;
};

namespace Physics
{
	class CVolumeChunk : public CVolume
	{
	public:
		struct Data : mData
		{
			Universe::CChunkNode* pChunkNode;
		};
		
	public:
		CVolumeChunk(const CVObject* pObject);
		~CVolumeChunk();

		CVolumeChunk(const CVolumeChunk&) = delete;
		CVolumeChunk(CVolumeChunk&&) = delete;
		CVolumeChunk& operator = (const CVolumeChunk&) = delete;
		CVolumeChunk& operator = (CVolumeChunk&&) = delete;
		
		void UpdateBounds() final;
		bool MotionSolver(CVolume* pOther) final;
		bool IdleSolver(CVolume* pOther) final;
		bool RayTest(const QueryRay& query, RaycastInfo& info) const final;
		Math::SIMDVector SupportPoint(const Math::SIMDVector& dir, const CVolume* pVolumeA, float inset = 0.0f) const final;

		void SetData(const Data& data);
		
	protected:
		bool OctreeRayTest(const Math::Vector3& origin, const Math::Vector3& dir, const Math::Vector3& mn, const Math::Vector3& mx, 
			const Math::Vector3& mnOffset, const Math::Vector3& mxOffset, float distMax, RaycastInfo& info, bool bGenerateNormal, 
			std::function<bool(float, float)> comp, std::function<void(const RaycastInfo&, const Math::Vector3&)> onFound) const;
		
		bool OctreeIntersectionTest(const Math::Vector3& origin, const Math::Vector3& mn, const Math::Vector3& mx, 
			const Math::Vector3& mnOffset, const Math::Vector3& mxOffset, std::function<void(std::pair<Math::VectorInt3, u32>&, const Math::Vector3&)> onFound) const;

		// Accessors.
		virtual inline const mData& GetData() const final { return m_data; }

	private:
		//Math::Vector3 m_halfSize;
		float m_blockSize;
		float m_blockSizeHalf;
		float m_blockSizeHalfPadded;
		float m_blockSizeInv;
		float m_blockSizeEps;
		float m_blockSizeNegEps;

		Math::Vector3 m_blockOffset;

		Data m_data;
	};
};

#endif