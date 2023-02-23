//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: Application/CGizmoData.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CGIZMODATA_H
#define CGIZMODATA_H

#include <Math/CMathColor.h>
#include <Math/CMathVector2.h>
#include <Math/CSIMDMatrix.h>
#include <Math/CSIMDRay.h>
#include <Globals/CGlobals.h>
#include <functional>

namespace App
{
	//
	// Drawing.
	//

	struct GizmoVertexLine
	{
		Math::Vector3 position;
		Math::Vector3 normal;
	};
	
	struct GizmoVertexGeo
	{
		Math::Vector3 position;
		Math::Vector3 normal;
	};

	struct GizmoInstanceLine
	{
		Math::Color color;
		Math::SIMDMatrix world;
		Math::Vector3 origin;
		float width;
	};

	struct GizmoInstanceGeo
	{
		Math::Color color;
		Math::SIMDMatrix world;
		Math::Vector3 origin;
	};

	struct GizmoDrawLine
	{
		GizmoInstanceLine instance;
	};

	struct GizmoDrawGeo
	{
		GizmoInstanceGeo instance;
	};

	//
	// Interaction.
	//

	enum class GizmoInteractType
	{
		Line,
		Quad,
		Circle,
		Sphere,
		Cube,
	};

	struct GizmoInteractLine
	{
		Math::SIMDVector origin;
		Math::SIMDVector dir;
		float width;
		float length;
	};

	struct GizmoInteractQuad
	{
		Math::SIMDVector origin;
		Math::SIMDVector normal;
		Math::SIMDVector tangent;
		Math::SIMDVector bitangent;
		Math::Vector2 size;
	};

	struct GizmoInteractCircle
	{
		Math::SIMDVector origin;
		Math::SIMDVector normal;
		Math::SIMDVector bitangent;
		float width;
		float radius;
		bool bHalf;
	};
	
	struct GizmoInteractSphere
	{
		Math::SIMDVector origin;
		float radius;
	};
	
	struct GizmoInteractCube
	{
		Math::SIMDVector origin;
		Math::SIMDVector right;
		Math::SIMDVector up;
		Math::SIMDVector forward;
		Math::Vector3 size;
	};

	struct GizmoInteract
	{
		u32 hash = 0;
		u32 priority = 0;
		GizmoInteractType type = GizmoInteractType::Line;

		std::function<void(u32)> onEnter;
		std::function<void(u32)> onExit;
		std::function<void(u32, const Math::CSIMDRay&, float)> onPressed;
		std::function<void(u32)> onReleased;
		std::function<void(u32, const Math::CSIMDRay&)> whileHeld;

		union
		{
			GizmoInteractLine line;
			GizmoInteractQuad quad;
			GizmoInteractCircle circle;
			GizmoInteractSphere sphere;
			GizmoInteractCube cube;
		};
	};
};

#endif
