//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: ../Resources/Engine.res
//
//-------------------------------------------------------------------------------------------------

@ENGINE {
	SCRIPTS {
		( SCRIPT_WUI_APP_MAIN, "UI/WUI/VoxelApp/Main.wui" )
	}

	SHADERS {
		( SHADER_POST_SKY, "Shaders/PostSky.shader" )
		( SHADER_POST_LIGHTING, "Shaders/PostLighting.shader" )
		( SHADER_POST_FXAA, "Shaders/PostFXAA.shader" )
		( SHADER_POST_SSAO, "Shaders/PostSSAO.shader" )
		( SHADER_POST_EDGE, "Shaders/PostEdge.shader" )
		( SHADER_POST_BLUR, "Shaders/PostBlur.shader" )
		( SHADER_POST_TONE_MAPPING, "Shaders/PostToneMapping.shader" )
	
		( SHADER_POST, "Shaders/Post.shader" )

		( SHADER_PRIMITIVE, "Shaders/Primitive.shader" )
		( SHADER_DEBUG, "Shaders/Debug.shader" )
		( SHADER_DEBUG2D, "Shaders/Debug2D.shader" )
		( SHADER_UI_IMAGE, "Shaders/UIImage.shader" )
		( SHADER_UI_TEXT, "Shaders/UIText.shader" )
	
		( SHADER_VOXEL, "Shaders/Voxel.shader" )
	}

	MATERIALS {
		( MATERIAL_POST, "Materials/Post1.mat" )
	
		( MATERIAL_POST_SKY, "Materials/PostSky.mat" )
		( MATERIAL_POST_LIGHTING, "Materials/PostLighting.mat" )
		( MATERIAL_POST_FXAA, "Materials/PostFXAA.mat" )
		( MATERIAL_POST_SSAO, "Materials/PostSSAO.mat" )
		( MATERIAL_POST_EDGE, "Materials/PostEdge.mat" )
		( MATERIAL_POST_HBLUR, "Materials/PostHBlur.mat" )
		( MATERIAL_POST_VBLUR, "Materials/PostVBlur.mat" )
		( MATERIAL_POST_TONE_MAPPING, "Materials/PostToneMapping.mat" )
	
		( MATERIAL_PRIMITIVE, "Materials/Primitive.mat" )
		( MATERIAL_PRIMITIVE_TEXTURED, "Materials/PrimitiveTex.mat" )
	
		( MATERIAL_DEBUG, "Materials/Debug.mat" )
		( MATERIAL_DEBUG2D, "Materials/Debug2D.mat" )

		( MATERIAL_UI_IMAGE, "Materials/UIImage.mat" )
		( MATERIAL_UI_IMAGE_PT, "Materials/UIImagePt.mat" )
		( MATERIAL_UI_TEXT, "Materials/UIText.mat" )

		( MATERIAL_UI_VIEW, "Materials/UIView.mat" )
		( MATERIAL_UI_VIEW_PT, "Materials/UIViewPt.mat" )
	
		( MATERIAL_VOXEL, "Materials/Voxel.mat" )
		( MATERIAL_VOXELWIRE, "Materials/VoxelWire.mat" )
	}

	AUDIO {
		( AUDIO_FOOTSTEP_STONE_001, "Audio/Effects/Footsteps/FootstepStone001.ogg", "false" )
		( AUDIO_FOOTSTEP_STONE_002, "Audio/Effects/Footsteps/FootstepStone002.ogg", "false" )
		( AUDIO_FOOTSTEP_STONE_003, "Audio/Effects/Footsteps/FootstepStone003.ogg", "false" )
		( AUDIO_WINDY_DAY, "Audio/Ambience/Nature/windy_day.ogg", "true" )
	}
}
