//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Physics/CVolumeChunk.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CVolumeChunk.h"
#include "../Universe/CChunkNode.h"
#include "../Universe/CChunkData.h"

namespace Physics
{
	CVolumeChunk::CVolumeChunk(const CVObject* pObject) :
		CVolume(pObject),
		m_blockOffset(0)
	{
	}

	CVolumeChunk::~CVolumeChunk() { }
	
	void CVolumeChunk::UpdateBounds()
	{
		m_minExtents = m_data.pChunkNode->GetMinExtents();
		m_maxExtents = m_data.pChunkNode->GetMaxExtents();
	}

	bool CVolumeChunk::MotionSolver(CVolume* pOther)
	{
		bool bAdjusted = false;

		Math::Vector3 center = *(Math::Vector3*)GetPosition().ToFloat();

		Math::Vector3 origin = *reinterpret_cast<const Math::Vector3*>(pOther->GetSolverPosition().ToFloat());
		Math::Vector3 mn = m_data.pChunkNode->GetMinExtents();
		Math::Vector3 mx = m_data.pChunkNode->GetMaxExtents();
		Math::Vector3 dir = *reinterpret_cast<const Math::Vector3*>(pOther->GetSolverVelocity().ToFloat());
		RaycastInfo info { };

		struct HitInfo
		{
			RaycastInfo info;
			Math::Vector3 pt;
		};

		std::vector<HitInfo> infoList;
		float dialation = m_blockSizeHalfPadded;
		
		info.distance = dir.Length();
		dir /= info.distance;
		info.distance += m_data.skinDepth;
		if(OctreeRayTest(origin, dir, mn, mx, -pOther->GetMaxExtents() - dialation, -pOther->GetMinExtents() + dialation, info.distance, info, false,
			[](float a, float b){ return true; },
			[&infoList](const RaycastInfo& i, const Math::Vector3& pt){ 
				infoList.push_back({ i, pt });
			}))
		{
			for(size_t i = 0; i < infoList.size(); ++i)
			{
				m_blockOffset = infoList[i].pt * m_blockSize;
				bAdjusted |= CVolume::MotionSolver(pOther);
			}
		}

		return bAdjusted;

		return false;
	}

	bool CVolumeChunk::IdleSolver(CVolume* pOther)
	{
		bool bAdjusted = false;

		Math::Vector3 origin = *reinterpret_cast<const Math::Vector3*>(pOther->GetSolverPosition().ToFloat());
		Math::Vector3 mn = m_data.pChunkNode->GetMinExtents();
		Math::Vector3 mx = m_data.pChunkNode->GetMaxExtents();

		struct HitInfo
		{
			std::pair<Math::VectorInt3, u32> indices;
			Math::Vector3 pt;
		};

		std::vector<HitInfo> infoList;
		float dialation = m_blockSizeHalfPadded;//GetSkinDepth() + pOther->GetSkinDepth();

		if(OctreeIntersectionTest(origin, mn, mx, -pOther->GetMaxExtents() - dialation, -pOther->GetMinExtents() + dialation,
			[&infoList](std::pair<Math::VectorInt3, u32> indices, const Math::Vector3& pt) {
				infoList.push_back({ indices, pt });
			}))
		{
			for(size_t i = 0; i < infoList.size(); ++i)
			{
				m_blockOffset = infoList[i].pt * m_blockSize;
				bAdjusted |= CVolume::IdleSolver(pOther);
			}
		}

		return bAdjusted;
	}

	bool CVolumeChunk::RayTest(const QueryRay& query, RaycastInfo& info) const
	{
		Math::Vector3 center = *(Math::Vector3*)GetPosition().ToFloat();

		Math::Vector3 origin = *reinterpret_cast<const Math::Vector3*>(query.ray.GetOrigin().ToFloat());
		Math::Vector3 dir = *reinterpret_cast<const Math::Vector3*>(query.ray.GetDirection().ToFloat());
		Math::Vector3 mn = m_data.pChunkNode->GetMinExtents();
		Math::Vector3 mx = m_data.pChunkNode->GetMaxExtents();
		info.distance = query.ray.GetDistance();
		
		if(!OctreeRayTest(origin, dir, mn, mx, 0.0f, 0.0f, -1.0f, info, true, [](float a, float b){ return a < b; }, nullptr))
		{
			// Test planes
			float tmin = 0.0f;
			float tmax = info.distance;

			// Test for ray intersection with the current AABB.
			for(u32 i = 0; i < 3; ++i)
			{
				float invD = 1.0f / dir[i];
				float t0 = (mn[i] - origin[i]) * invD;
				float t1 = (mx[i] - origin[i]) * invD;

				if(invD < 0.0f)
				{
					std::swap(t0, t1);
				}

				tmin = t0 > tmin ? t0 : tmin;
				tmax = t1 < tmax ? t1 : tmax;

				if(tmax <= tmin) { return false; }
			}

			// Find Block.
			Math::Vector3 hit = origin + dir * tmax - center;
			
			float largestCoord = 0.0f;
			Universe::SIDE side = Universe::SIDE::SIDE_TOP;
			for(u32 i = 0; i < 3; ++i)
			{
				if(-hit[i] > largestCoord) { largestCoord = -hit[i]; side = static_cast<Universe::SIDE>((i << 1) + 1); }
				if( hit[i] > largestCoord) { largestCoord =  hit[i]; side = static_cast<Universe::SIDE>((i << 1)); }
			}

			auto indices = m_data.pChunkNode->GetIndex(hit * m_blockSizeInv + Universe::SIDE_NORMAL[side] * 0.5f);

			info.index.U32.lo[0] = static_cast<u32>(indices.first.x);
			info.index.U32.lo[1] = static_cast<u32>(indices.first.y);
			info.index.U32.hi[0] = static_cast<u32>(indices.first.z);
			info.index.U32.hi[1] = indices.second | (0x1 << 31); // MSB set indicating that this is a point on the boundary.
			info.distance = tmax;
			info.pVolume = this;
			info.normal = Universe::SIDE_NORMAL[side];
			info.point = query.ray.GetOrigin() + query.ray.GetDirection() * info.distance;

			return true;
		}
		else
		{
			return true;
		}
	}

	Math::SIMDVector CVolumeChunk::SupportPoint(const Math::SIMDVector& dir, const CVolume* pVolumeA, float inset) const
	{
		Math::SIMDVector position = GetSolverPosition() + m_blockOffset - pVolumeA->GetSolverPosition();

		Math::SIMDVector localDir = GetSolverRotation().Conjugate() * dir;
		Math::SIMDVector maxCorner(_mm_or_ps(_mm_and_ps(localDir.m_xmm, _mm_set_ps1(-0.0f)),
			_mm_setr_ps(m_blockSizeHalfPadded - inset, m_blockSizeHalfPadded - inset, m_blockSizeHalfPadded - inset, 0.0f)));
		return position + GetSolverRotation() * maxCorner;
	}

	//-----------------------------------------------------------------------------------------------
	// Octree methods.
	//-----------------------------------------------------------------------------------------------
	
	bool CVolumeChunk::OctreeRayTest(const Math::Vector3& origin, const Math::Vector3& dir, 
		const Math::Vector3& mn, const Math::Vector3& mx, const Math::Vector3& mnOffset, const Math::Vector3& mxOffset, float distMax,
		RaycastInfo& info, bool bGenerateNormal, std::function<bool(float, float)> comp, std::function<void(const RaycastInfo&, const Math::Vector3&)> onFound) const
	{
		const Math::Vector3 sz = mx - mn;
		const float compMax = distMax < 0.0f ? info.distance : distMax;
		bool bVoxel = (sz.x <= m_blockSizeEps && sz.y <= m_blockSizeEps && sz.z <= m_blockSizeEps);
		float tmin = 0.0f;
		float tmax = compMax;

		// Test for ray intersection with the current AABB.
		for(u32 i = 0; i < 3; ++i)
		{
			float invD = 1.0f / dir[i];
			float t0 = (mn[i] + mnOffset[i] - origin[i]) * invD;
			float t1 = (mx[i] + mxOffset[i] - origin[i]) * invD;

			if(invD < 0.0f)
			{
				std::swap(t0, t1);
			}

			tmin = t0 > tmin ? t0 : tmin;
			tmax = t1 < tmax ? t1 : tmax;

			if(tmax <= tmin) { return false; }
		}
		
		bool bResult = false;

		if(bVoxel)
		{
			bool bFlipped = tmin <= 0.0f;

			if(bFlipped)
			{
				tmin = tmax;
			}
		}
		
		if(!bVoxel || comp(tmin, compMax))
		{
			// Collision found.
			if(bVoxel)
			{
				// Inside voxel.
				std::pair<Math::VectorInt3, u32> indices;
				const Universe::Block block = m_data.pChunkNode->GetBlock(mn * m_blockSizeInv + 0.5f, &indices);
				
				if(!block.bEmpty)
				{
					if(bGenerateNormal)
					{
						Math::Vector3 hit = origin + dir * tmin;
						Math::Vector3 offCenter = ((hit - mn) * m_blockSizeInv - 0.5f) * 2.0f;

						std::pair<float, Universe::SIDE> closestList[3] { };
						u32 closestIndex = 0;

						for(u32 i = 0; i < 3; ++i)
						{
							if(-offCenter[i] > 0.99f) { closestList[closestIndex++] = { offCenter[i], static_cast<Universe::SIDE>((i << 1)) }; }
							if( offCenter[i] > 0.99f) { closestList[closestIndex++] = { offCenter[i], static_cast<Universe::SIDE>((i << 1) + 1) }; }
						}

						if(closestIndex > 1)
						{
							if(fabsf(closestList[0].first) < fabsf(closestList[1].first)) std::swap(closestList[0], closestList[1]);

							if(closestIndex > 2)
							{
								if(fabsf(closestList[0].first) < fabsf(closestList[2].first)) std::swap(closestList[0], closestList[2]);
								if(fabsf(closestList[1].first) < fabsf(closestList[2].first)) std::swap(closestList[1], closestList[2]);
							}
						}

						for(u32 i = 0; i < closestIndex; ++i)
						{
							if(block.sideFlag & (1 << closestList[i].second))
							{
								info.index.U32.lo[0] = static_cast<u32>(indices.first.x);
								info.index.U32.lo[1] = static_cast<u32>(indices.first.y);
								info.index.U32.hi[0] = static_cast<u32>(indices.first.z);
								info.index.U32.hi[1] = indices.second;
								info.distance = tmin;
								info.pVolume = this;
								info.normal = Universe::SIDE_NORMAL[closestList[i].second];
								info.point = origin + dir * info.distance;
								if(onFound) onFound(info, mn * m_blockSizeInv + 0.5f);
								return true;
							}
						}
					}
					else
					{
						info.index.U32.lo[0] = static_cast<u32>(indices.first.x);
						info.index.U32.lo[1] = static_cast<u32>(indices.first.y);
						info.index.U32.hi[0] = static_cast<u32>(indices.first.z);
						info.index.U32.hi[1] = indices.second;
						info.distance = tmin;
						info.pVolume = this;
						info.normal = Math::SIMD_VEC_ZERO;
						info.point = origin + dir * info.distance;
						if(onFound) onFound(info, mn * m_blockSizeInv + 0.5f);
						return true;
					}
				}
			}
			else
			{ // Inside octree cell.
				Math::Vector3 half[2];

				for(u32 i = 0; i < 3; ++i)
				{
					const float szOne = sz[i] * m_blockSizeInv;
					const int szInt = static_cast<int>(roundf(szOne));
					const float szHalf = szOne * 0.5f;

					if(szInt & 0x1)
					{
						half[0][i] = floorf(szHalf) * m_blockSize;
						half[1][i] = ceilf(szHalf) * m_blockSize;
					}
					else
					{
						half[0][i] = half[1][i] = szHalf * m_blockSize;
					}
				}
				
				u32 mnMask = 0xFF;

				if(half[0].x < 1e-5f) mnMask &= ~(2 | 8 | 32 | 128);
				if(half[0].y < 1e-5f) mnMask &= ~(16 | 32 | 64 | 128);
				if(half[0].z < 1e-5f) mnMask &= ~(4 | 8 | 64 | 128);
				
				for(u32 i = 0; i < 2; ++i)
				{
					for(u32 j = 0; j < 3; ++j)
					{
						half[i][j] = half[i][j] < m_blockSize ? m_blockSize : half[i][j];
					}
				}

				// Perhaps look for a union style solution in the future.
				const Math::Vector3 mnList[] = {
					{ mn.x, mn.y, mn.z },
					{ mn.x + half[1].x, mn.y, mn.z },
					{ mn.x, mn.y, mn.z + half[1].z },
					{ mn.x + half[1].x, mn.y, mn.z + half[1].z },

					{ mn.x, mn.y + half[1].y, mn.z },
					{ mn.x + half[1].x, mn.y + half[1].y, mn.z },
					{ mn.x, mn.y + half[1].y, mn.z + half[1].z },
					{ mn.x + half[1].x, mn.y + half[1].y, mn.z + half[1].z },
				}; // FIX

				for(u32 i = 0; i < 8; ++i)
				{
					if(mnMask & (0x1 << i)) { bResult |= OctreeRayTest(origin, dir, mnList[i], mnList[i] + half[0], mnOffset, mxOffset, distMax, info, bGenerateNormal, comp, onFound); }
				}
			}
		}

		return bResult;
	}

	bool CVolumeChunk::OctreeIntersectionTest(const Math::Vector3& origin, const Math::Vector3& mn, const Math::Vector3& mx, 
		const Math::Vector3& mnOffset, const Math::Vector3& mxOffset, std::function<void(std::pair<Math::VectorInt3, u32>&, const Math::Vector3&)> onFound) const
	{
		const Math::Vector3 sz = mx - mn;
		bool bVoxel = (sz.x <= m_blockSizeEps && sz.y <= m_blockSizeEps && sz.z <= m_blockSizeEps);

		for(int i = 0; i < 3; ++i)
		{
			if(origin[i] < mn[i] + mnOffset[i]) return false;
			if(origin[i] > mx[i] + mxOffset[i]) return false;
		}

		bool bResult = false;
		
		// Collision found.
		if(bVoxel)
		{
			// Inside voxel.
			std::pair<Math::VectorInt3, u32> indices;
			const Universe::Block block = m_data.pChunkNode->GetBlock(mn * m_blockSizeInv + 0.5f, &indices);
			
			if(!block.bEmpty)
			{
				if(onFound) onFound(indices, mn * m_blockSizeInv + 0.5f);
				return true;
			}
		}
		else
		{
			// Inside octree cell.
			Math::Vector3 half[2];

			for(u32 i = 0; i < 3; ++i)
			{
				const float szOne = sz[i] * m_blockSizeInv;
				const int szInt = static_cast<int>(roundf(szOne));
				const float szHalf = szOne * 0.5f;

				if(szInt & 0x1)
				{
					half[0][i] = floorf(szHalf) * m_blockSize;
					half[1][i] = ceilf(szHalf) * m_blockSize;
				}
				else
				{
					half[0][i] = half[1][i] = szHalf * m_blockSize;
				}
			}
			
			u32 mnMask = 0xFF;

			if(half[0].x < 1e-5f) mnMask &= ~(2 | 8 | 32 | 128);
			if(half[0].y < 1e-5f) mnMask &= ~(16 | 32 | 64 | 128);
			if(half[0].z < 1e-5f) mnMask &= ~(4 | 8 | 64 | 128);
				
			for(u32 i = 0; i < 2; ++i)
			{
				for(u32 j = 0; j < 3; ++j)
				{
					half[i][j] = half[i][j] < m_blockSize ? m_blockSize : half[i][j];
				}
			}

			// Perhaps look for a union style solution in the future.
			const Math::Vector3 mnList[] = {
				{ mn.x, mn.y, mn.z },
				{ mn.x + half[1].x, mn.y, mn.z },
				{ mn.x, mn.y, mn.z + half[1].z },
				{ mn.x + half[1].x, mn.y, mn.z + half[1].z },

				{ mn.x, mn.y + half[1].y, mn.z },
				{ mn.x + half[1].x, mn.y + half[1].y, mn.z },
				{ mn.x, mn.y + half[1].y, mn.z + half[1].z },
				{ mn.x + half[1].x, mn.y + half[1].y, mn.z + half[1].z },
			}; // FIX

			for(u32 i = 0; i < 8; ++i)
			{
				if(mnMask & (0x1 << i)) { bResult |= OctreeIntersectionTest(origin, mnList[i], mnList[i] + half[0], mnOffset, mxOffset, onFound); }
			}
		}

		return true;
	}

	//-----------------------------------------------------------------------------------------------
	// Utility methods.
	//-----------------------------------------------------------------------------------------------
	
	void CVolumeChunk::SetData(const Data& data)
	{
		m_data = data;
		m_blockSize = m_data.pChunkNode->GetBlockSize();
		m_blockSizeHalf = m_data.pChunkNode->GetBlockSize() * 0.5f;
		m_blockSizeHalfPadded = m_blockSizeHalf + 0.02f;
		m_blockSizeInv = 1.0f / m_blockSize;
		m_blockSizeEps = m_blockSize + 1e-5f;
		m_blockSizeNegEps = m_blockSize + 1e-5f;
	}
};