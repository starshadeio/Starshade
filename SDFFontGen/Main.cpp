//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Console Application: SDF Font Texture Generator
//
// File: Main.cpp
//
//-------------------------------------------------------------------------------------------------

#include "Main.h"
#include "CFontTexGen.h"

int main(int argc, char* argv[])
{
	CFontTexGen fontTexGen;
	fontTexGen.SetFilename("RedHatDisplay-Regular.ttf");
	fontTexGen.SetDefaultFilename("EBGaramond-Regular.ttf");
	fontTexGen.Generate();
	return 0;
}
