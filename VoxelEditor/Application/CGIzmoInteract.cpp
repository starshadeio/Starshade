//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Application/CGizmoInteract.cpp
//
//-------------------------------------------------------------------------------------------------

#include "CGIzmoInteract.h"
#include "CNodeSelect.h"
#include "CEditor.h"
#include <Application/CSceneManager.h>
#include <Application/CCommandManager.h>
#include <Application/CInput.h>
#include <Application/CInputDeviceList.h>
#include <Application/CInputMouse.h>
#include <Graphics/CGraphicsAPI.h>
#include <Factory/CFactory.h>

namespace App
{
	CGizmoInteract::CGizmoInteract() : 
		m_bHeld(false),
		m_bLastIntersected(false),
		m_lastT(0.0f),
		m_lastIntersected{}
	{
	}

	CGizmoInteract::~CGizmoInteract()
	{
	}
	
	void CGizmoInteract::Initialize()
	{
		{ // Command setup.
			CCommandManager::Instance().RegisterAction(CNodeSelect::CMD_KEY_SELECT_PRESSED, std::bind(&CGizmoInteract::OnCmdSelectPressed, this));
			CCommandManager::Instance().RegisterAction(CNodeSelect::CMD_KEY_SELECT_RELEASED, std::bind(&CGizmoInteract::OnCmdSelectReleased, this));
		}
	}
	
	void CGizmoInteract::Update()
	{
		Math::CSIMDRay ray = CEditor::Instance().Nodes().GetSelector().GetScreenRay();

		if(!m_bHeld)
		{
			// Test	queued interactable elements for intersection.
			bool bIntersected = false;
			u32 priority = 0;
			float tMax = FLT_MAX;
			GizmoInteract intersected { };
			while(!m_interactQueue.empty())
			{
				const GizmoInteract& interactable = m_interactQueue.top();
				if(priority < interactable.priority)
				{
					priority = interactable.priority;
					tMax = FLT_MAX;
				}

				bool bTest = false;
				switch(interactable.type)
				{
					case GizmoInteractType::Line:
						bTest = TestLine(interactable.line, ray, tMax);
						break;
					case GizmoInteractType::Quad:
						bTest = TestQuad(interactable.quad, ray, tMax);
						break;
					case GizmoInteractType::Circle:
						bTest = TestCircle(interactable.circle, ray, tMax);
						break;
					case GizmoInteractType::Sphere:
						bTest = TestSphere(interactable.sphere, ray, tMax);
						break;
					case GizmoInteractType::Cube:
						bTest = TestCube(interactable.cube, ray, tMax);
						break;
					default:
						break;
				}

				if(bTest)
				{
					intersected = interactable;
					bIntersected = true;
				}

				m_interactQueue.pop();
			}

			// When intersection is happening, call exit/enter events if this is a new element.
			if(bIntersected)
			{
				m_lastRay = ray;
				m_lastT = tMax;
				
				if(!m_bLastIntersected || m_lastIntersected.hash != intersected.hash)
				{
					if(m_bLastIntersected && m_lastIntersected.onExit)
					{
						m_lastIntersected.onExit(m_lastIntersected.hash);
					}

					m_lastIntersected = intersected;
					m_bLastIntersected = true;

					if(m_lastIntersected.onEnter)
					{
						m_lastIntersected.onEnter(m_lastIntersected.hash);
					}
				}
			}
			else
			{
				if(m_bLastIntersected && m_lastIntersected.onExit)
				{
					m_lastIntersected.onExit(m_lastIntersected.hash);
				}

				m_bLastIntersected = false;
			}
		}
		else
		{ // m_bHeld == true
			if(m_lastIntersected.whileHeld)
			{
				m_lastIntersected.whileHeld(m_lastIntersected.hash, ray);
			}
		}
	}

	//-----------------------------------------------------------------------------------------------
	// Queue methods.
	//-----------------------------------------------------------------------------------------------
	
	void CGizmoInteract::AddInteractable(const GizmoInteract& interactData)
	{
		m_interactQueue.push(interactData);
	}

	//-----------------------------------------------------------------------------------------------
	// Command methods.
	//-----------------------------------------------------------------------------------------------

	void CGizmoInteract::OnCmdSelectPressed()
	{
		if(!m_bLastIntersected) return;

		if(m_lastIntersected.onPressed)
		{
			m_lastIntersected.onPressed(m_lastIntersected.hash, m_lastRay, m_lastT);
		}

		m_bHeld = true;
	}

	void CGizmoInteract::OnCmdSelectReleased()
	{
		if(m_lastIntersected.onReleased)
		{
			m_lastIntersected.onReleased(m_lastIntersected.hash);
		}

		m_bHeld = false;
	}
	
	//-----------------------------------------------------------------------------------------------
	// Intersection tests methods.
	//-----------------------------------------------------------------------------------------------

	bool CGizmoInteract::IntersectRayCylinder(const Math::CSIMDRay& ray, const Math::SIMDVector& p, const Math::SIMDVector& q, float r, float &tMax)
	{
		const Math::SIMDVector m = ray.GetOrigin() - p;
		const Math::SIMDVector n = ray.GetDirection();

		const Math::SIMDVector diff = q - p;
		const float l = _mm_cvtss_f32(diff.Length());
		const Math::SIMDVector d = diff.Normalized();

		const Math::SIMDVector dxn = Math::SIMDVector::Cross(d, n);
		const Math::SIMDVector dxm = Math::SIMDVector::Cross(d, m);

		const float a = _mm_cvtss_f32(Math::SIMDVector::Dot(dxn, dxn));
		const float c = _mm_cvtss_f32(Math::SIMDVector::Dot(dxm, dxm)) - r * r;

		if(fabsf(a) < 1e-5f)
		{
			if(c > 0.0f) return false;
			
			float t;
			const float md = _mm_cvtss_f32(Math::SIMDVector::Dot(m, d));
			if(md < 0.0f) t = -_mm_cvtss_f32(Math::SIMDVector::Dot(m, n));
			else if(md > l) t = _mm_cvtss_f32(Math::SIMDVector::Dot(n, d)) - _mm_cvtss_f32(Math::SIMDVector::Dot(m, n));
			else t = 0.0f;

			if(tMax > t)
			{
				tMax = t;
				return true;
			}
		}
		else
		{
			const float b = _mm_cvtss_f32(Math::SIMDVector::Dot(dxm, dxn));

			const float discriminant = b * b - a * c;
			if(discriminant < 0.0f) return false;

			const float rt = sqrtf(discriminant);

			float t = std::min((-b - rt) / a, (-b + rt) / a);

			const Math::SIMDVector pt = (ray.GetOrigin() + ray.GetDirection() * t);
			const float rd = _mm_cvtss_f32(Math::SIMDVector::Dot(pt - p, d));

			if(rd < 0.0f)
			{
				if(!PlanePoint(p, -d, ray, t))
				{
					return false;
				}

				if(_mm_cvtss_f32(((ray.GetOrigin() + ray.GetDirection() * t) - p).LengthSq()) > r * r)
				{
					return false;
				}
			}
			else if(rd > l)
			{
				if(!PlanePoint(q, d, ray, t))
				{
					return false;
				}

				if(_mm_cvtss_f32(((ray.GetOrigin() + ray.GetDirection() * t) - q).LengthSq()) > r * r)
				{
					return false;
				}
			}

			if(t < tMax)
			{
				tMax = t;
				return true;
			}
		}

		return false;
	}

	// Intersect segment S(t)=sa+t(sb-sa), 0<=t<=1 against cylinder specified by p, q and r
	bool CGizmoInteract::IntersectSegmentCylinder(const Math::SIMDVector& sa, const Math::SIMDVector& sb, const Math::SIMDVector& p, const Math::SIMDVector& q, float r, float &t)
	{
		Math::SIMDVector d = q - p, m = sa - p, n = sb - sa;
		float md = _mm_cvtss_f32(Math::SIMDVector::Dot(m, d));
		float nd = _mm_cvtss_f32(Math::SIMDVector::Dot(n, d));
		float dd = _mm_cvtss_f32(Math::SIMDVector::Dot(d, d));

		// Test if segment fully outside either endcap of cylinder
		if(md < 0.0f && md + nd < 0.0f) return false; // Segment outside ’p’ side of cylinder
		if(md > dd && md + nd > dd) return false; // Segment outside ’q’ side of cylinder

		float nn = _mm_cvtss_f32(Math::SIMDVector::Dot(n, n));
		float mn = _mm_cvtss_f32(Math::SIMDVector::Dot(m, n));
		float a = dd * nn - nd * nd;
		float k = _mm_cvtss_f32(Math::SIMDVector::Dot(m, m)) - r * r;
		float c = dd * k - md * md;

		if(fabsf(a) < 1e-5f)
		{
			// Segment runs parallel to cylinder axis
			if (c > 0.0f) return false; // ’a’ and thus the segment lie outside cylinder

			// Now known that segment intersects cylinder; figure out how it intersects
			if (md < 0.0f) t = -mn / nn; // Intersect segment against ’p’ endcap
			else if (md > dd) t = (nd - mn) / nn; // Intersect segment against ’q’ endcap
			else t = 0.0f; // ’a’ lies inside cylinder

			return true;
		}

		float b = dd * mn - nd * md;
		float discr = b * b - a * c;

		if(discr < 0.0f) return false; // No real roots; no intersection

		t = (-b - sqrtf(discr)) / a;

		if(t < 0.0f || t > 1.0f) return false; // Intersection lies outside segment

		if(md + t * nd < 0.0f)
		{
			// Intersection outside cylinder on ’p’ side
			if(nd <= 0.0f) return false; // Segment pointing away from endcap
			t = -md / nd;

			// Keep intersection if Dot(S(t) - p, S(t) - p) <= r^2
			auto pt = sa + (sb - sa) * t - p;
			return _mm_cvtss_f32(Math::SIMDVector::Dot(pt, pt)) <= r * r;

			//return k + 2 * t * (mn + t * nn) <= 0.0f;
			//return k + 2 * t * (2 * mn + t * nn) <= 0.0f;
		}
		else if(md + t * nd > dd)
		{
			// Intersection outside cylinder on ’q’ side
			if(nd >= 0.0f) return false; // Segment pointing away from endcap

			t = (dd - md) / nd;

			// Keep intersection if Dot(S(t) - q, S(t) - q) <= r^2
			auto pt = sa + (sb - sa) * t - q;
			return _mm_cvtss_f32(Math::SIMDVector::Dot(pt, pt)) <= r * r;
			//return k + dd - 2 * md + t * (2 * (mn - nd) + t * nn) <= 0.0f;
		}

		// Segment intersects cylinder between the endcaps; t is correct
		return true;
	}
	
	bool CGizmoInteract::TestLine(const GizmoInteractLine& line, const Math::CSIMDRay& ray, float& tMax)
	{
		auto pos = App::CSceneManager::Instance().CameraManager().GetDefaultCamera()->GetTransform()->GetPosition();
		auto f = App::CSceneManager::Instance().CameraManager().GetDefaultCamera()->GetTransform()->GetForward();

		const float s = _mm_cvtss_f32(Math::SIMDVector::Dot(line.origin - pos, f));
		
		const float l = line.length * s;
		const float r = line.width * s;

		float t = tMax;
		if(IntersectRayCylinder(ray, line.origin, line.origin + line.dir * l, r, t))
		{
			tMax = t;
			return true;
		}

		return false;
	}

	bool CGizmoInteract::TestQuad(const GizmoInteractQuad& quad, const Math::CSIMDRay& ray, float& tMax)
	{
		auto pos = App::CSceneManager::Instance().CameraManager().GetDefaultCamera()->GetTransform()->GetPosition();
		auto f = App::CSceneManager::Instance().CameraManager().GetDefaultCamera()->GetTransform()->GetForward();

		const float s = _mm_cvtss_f32(Math::SIMDVector::Dot(quad.origin - pos, f));

		Math::SIMDVector n = quad.normal;
		float den = _mm_cvtss_f32(Math::SIMDVector::Dot(n, ray.GetDirection()));
		if(den < 0.0f)
		{
			den = -den;
			n = -n;
		}

		if(fabsf(den) < 1e-5f)
		{
			return false;
		}

		const float t = _mm_cvtss_f32(Math::SIMDVector::Dot(n, quad.origin - ray.GetOrigin())) / den;

		if(tMax > t)
		{
			Math::SIMDVector diff = ray.GetOrigin() + ray.GetDirection() * t - quad.origin;

			const float u = _mm_cvtss_f32(Math::SIMDVector::Dot(quad.tangent, diff));
			if(u < 0.0f || u > quad.size.x * s) return false;
			const float v = _mm_cvtss_f32(Math::SIMDVector::Dot(quad.bitangent, diff));
			if(v < 0.0f || v > quad.size.y * s) return false;

			tMax = t;
			return true;
		}

		return false;
	}

	bool CGizmoInteract::TestCircle(const GizmoInteractCircle& circle, const Math::CSIMDRay& ray, float& tMax)
	{
		auto pos = App::CSceneManager::Instance().CameraManager().GetDefaultCamera()->GetTransform()->GetPosition();
		auto f = App::CSceneManager::Instance().CameraManager().GetDefaultCamera()->GetTransform()->GetForward();

		const float s = _mm_cvtss_f32(Math::SIMDVector::Dot(circle.origin - pos, f));
		
		const float radius = circle.radius * s;
		const float width = circle.width * s;
		const float hw = width * 0.5f;

		const auto p = circle.origin - circle.normal * hw;
		const auto q = circle.origin + circle.normal * hw;

		float t = tMax;
		if(IntersectRayCylinder(ray, p, q, radius + hw, t))
		{
			const auto cylDir = (q - p).Normalized();
			auto hitPt = (ray.GetOrigin() + ray.GetDirection() * t) - p;
			auto parallel = cylDir * _mm_cvtss_f32(Math::SIMDVector::Dot(hitPt, cylDir));
			auto perpendicular = hitPt - parallel;
			
			const float radiusInnerSq = powf(radius - hw, 2.0f);
			const float dist = _mm_cvtss_f32(perpendicular.LengthSq());
			if(dist < radiusInnerSq) return false;

			if(circle.bHalf)
			{
				if(_mm_cvtss_f32(Math::SIMDVector::Dot(perpendicular, circle.bitangent)) < 0.0f)
				{
					return false;
				}
			}

			tMax = t;
			return true;
		}

		return false;
	}
	
	bool CGizmoInteract::TestSphere(const GizmoInteractSphere& sphere, const Math::CSIMDRay& ray, float& tMax)
	{
		auto pos = App::CSceneManager::Instance().CameraManager().GetDefaultCamera()->GetTransform()->GetPosition();
		auto f = App::CSceneManager::Instance().CameraManager().GetDefaultCamera()->GetTransform()->GetForward();

		const float s = _mm_cvtss_f32(Math::SIMDVector::Dot(sphere.origin - pos, f));
		
		const float radius = sphere.radius * s;

		const auto oc = ray.GetOrigin() - sphere.origin;

		const float b = -_mm_cvtss_f32(Math::SIMDVector::Dot(oc, ray.GetDirection()));
		const float c = _mm_cvtss_f32(Math::SIMDVector::Dot(oc, oc)) - radius * radius;
		const float discriminant = b * b - c;
		if(discriminant > 0.0f)
		{
			const float sqr = sqrtf(discriminant);
			const float t = (b + (sqr > b ? sqr : -sqr));
			if(t < tMax)
			{
				tMax = t;
				return true;
			}
		}

		return false;
	}

	bool CGizmoInteract::TestCube(const GizmoInteractCube& cube, const Math::CSIMDRay& ray, float& tMax)
	{
		auto pos = App::CSceneManager::Instance().CameraManager().GetDefaultCamera()->GetTransform()->GetPosition();
		auto f = App::CSceneManager::Instance().CameraManager().GetDefaultCamera()->GetTransform()->GetForward();
		auto fv = App::CSceneManager::Instance().CameraManager().GetDefaultCamera()->GetFarView();

		const float s = _mm_cvtss_f32(Math::SIMDVector::Dot(cube.origin - pos, f));

		Math::SIMDVector a = ray.GetOrigin() - cube.origin;
		Math::SIMDVector b = ray.GetDirection();

		Math::Vector3 p(
			_mm_cvtss_f32(Math::SIMDVector::Dot(a, cube.right)),
			_mm_cvtss_f32(Math::SIMDVector::Dot(a, cube.up)),
			_mm_cvtss_f32(Math::SIMDVector::Dot(a, cube.forward))
		);

		Math::Vector3 d(
			_mm_cvtss_f32(Math::SIMDVector::Dot(b, cube.right)),
			_mm_cvtss_f32(Math::SIMDVector::Dot(b, cube.up)),
			_mm_cvtss_f32(Math::SIMDVector::Dot(b, cube.forward))
		);

		// Perform slab test in OBB space.
		float tMn = 0.0f;
		float tMx = tMax;
		for(int i = 0; i < 3; ++i)
		{
			const float invD = 1.0f / d[i];
			float t0 = (-cube.size[i] * s - p[i]) * invD;
			float t1 = ( cube.size[i] * s - p[i]) * invD;
			if(invD < 0.0f) std::swap(t0, t1);
			tMn = t0 > tMn ? t0 : tMn;
			tMx = t1 < tMx ? t1 : tMx;
			if(tMx <= tMn) return false;
		}

		if(tMn < tMax)
		{
			tMax = tMn;
			return true;
		}

		return false;
	}

	//-----------------------------------------------------------------------------------------------
	// Static methods.
	//-----------------------------------------------------------------------------------------------
	
	// Calculate the plane point for an axis edge.
	bool CGizmoInteract::EdgePoint(const Math::SIMDVector& pt, const Math::SIMDVector& dir, const Math::CSIMDRay& ray, float& t)
	{
		const auto camDir = App::CSceneManager::Instance().CameraManager().GetDefaultCamera()->GetTransform()->GetForward();

		const auto diff = ray.GetOrigin() - pt;

		const auto right = Math::SIMDVector::Cross(diff, dir);
		const auto forward = Math::SIMDVector::Cross(dir, right).Normalized();

		return PlanePoint(pt, forward, ray, t);
	}

	// Calculate the plane point for an axis plane.
	bool CGizmoInteract::PlanePoint(const Math::SIMDVector& pt, const Math::SIMDVector& normal, const Math::CSIMDRay& ray, float& t)
	{
		float den = _mm_cvtss_f32(Math::SIMDVector::Dot(normal, ray.GetDirection()));
		if(den > 0.0f)
		{
			return false;
		}

		if(fabsf(den) < 1e-5f)
		{
			return false;
		}

		t = _mm_cvtss_f32(Math::SIMDVector::Dot(normal, pt - ray.GetOrigin())) / den;
		return true;
	}

};

