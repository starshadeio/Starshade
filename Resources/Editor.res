//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Application: Voxel Editor
//
// File: ../Resources/Editor.res
//
//-------------------------------------------------------------------------------------------------

#include "Engine.res"

@EDITOR {
	LOCALIZATION {
		( LOC_EN, "Localization/VoxelEditor/localizedText_en.loc" )
	}

	SCRIPTS {
		( SCRIPT_PANE_EDITOR, "UI/Panes/VoxelEditor/Editor.pane" )
		( SCRIPT_PANE_SCENE_VIEW, "UI/Panes/VoxelEditor/SceneView.pane" )
	}

	FONTS {
		( FONT_DEFAULT, "Fonts/VoxelEditor/RedHatDisplay-Regular.fnt" )
	}

	TEXTURES {
		( TEXTURE_UI_FRAME_FILLED_1PT, "Textures/VoxelEditor/UI/ui_frame_filled_1pt.dds" )
		( TEXTURE_UI_FRAME_FILLED_2PT, "Textures/VoxelEditor/UI/ui_frame_filled_2pt.dds" )
		( TEXTURE_UI_FRAME_FILLED_3PT, "Textures/VoxelEditor/UI/ui_frame_filled_3pt.dds" )
		( TEXTURE_UI_FRAME_1PT, "Textures/VoxelEditor/UI/ui_frame_1pt.dds" )
		( TEXTURE_UI_FRAME_2PT, "Textures/VoxelEditor/UI/ui_frame_2pt.dds" )
		( TEXTURE_UI_FRAME_3PT, "Textures/VoxelEditor/UI/ui_frame_3pt.dds" )

		( TEXTURE_UI_BLANK, "Textures/VoxelEditor/UI/ui_blank.dds" )
		( TEXTURE_UI_ICONS, "Textures/VoxelEditor/UI/ui_icons.dds" )

		( TEXTURE_UI_PANEL_ARROW, "Textures/VoxelEditor/UI/ui_panel_arrow.dds" )
	}

	SHADERS {
		( SHADER_POST_OVERLAY, "Shaders/VoxelEditor/PostOverlay.shader" )

		( SHADER_CYBER_GRID, "Shaders/VoxelEditor/CyberGrid.shader" )
		( SHADER_CYBER_NODE, "Shaders/VoxelEditor/CyberNode.shader" )
		( SHADER_NODE_GRID, "Shaders/VoxelEditor/NodeGrid.shader" )
		( SHADER_TEXTURED_QUAD, "Shaders/VoxelEditor/TexturedQuad.shader" )

		( SHADER_GIZMO, "Shaders/VoxelEditor/Gizmo.shader" )
	}

	MATERIALS {
		( MATERIAL_POST_OVERLAY, "Materials/VoxelEditor/PostOverlay.mat" )

		( MATERIAL_DEBUG_GREEN, "Materials/VoxelEditor/DebugGreen.mat" )

		( MATERIAL_CYBER_GRID, "Materials/VoxelEditor/CyberGrid.mat" )
		( MATERIAL_CYBER_NODE, "Materials/VoxelEditor/CyberNode.mat" )
		( MATERIAL_NODE_GRID, "Materials/VoxelEditor/NodeGrid.mat" )
		( MATERIAL_PLACEMENT_QUAD, "Materials/VoxelEditor/PlacementQuad.mat" )
		( MATERIAL_DELETION_QUAD, "Materials/VoxelEditor/DeletionQuad.mat" )

		( MATERIAL_GIZMO_LINE, "Materials/VoxelEditor/GizmoLine.mat" )
		( MATERIAL_GIZMO_TRIANGLE, "Materials/VoxelEditor/GizmoTri.mat" )
		( MATERIAL_GIZMO_TRIANGLE_UNLIT, "Materials/VoxelEditor/GizmoTriUnlit.mat" )
	}
}
