//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Console Application: App Pack
//
// File: Main.cpp
//
//-------------------------------------------------------------------------------------------------

#include "Main.h"
#include <fstream>
#include <Utilities/CArchive.h>
#include <Utilities/CFileSystem.h>

// Creates an application pack for the editor to be able to generate builds without a need to recompile.
//	In the future there will also be the ability to create source code that can be built manually or automatically,
//		but until the user can add complex logic to projects, this simple solution will suffice.
int main(int argc, char* argv[])
{
	Util::CArchive archive;

	archive.Insert(L".\\Binaries\\Starshade.exe", L"Starshade.exe");
	archive.Insert(L".\\Binaries\\D3DCompiler_47.dll", L"D3DCompiler_47.dll");
	archive.Insert(L".\\Binaries\\msvcp140.dll", L"msvcp140.dll");
	archive.Insert(L".\\Binaries\\vcruntime140.dll", L"vcruntime140.dll");
	archive.Insert(L".\\Binaries\\vcruntime140_1.dll", L"vcruntime140_1.dll");
	archive.Insert(L".\\Binaries\\XAudio2_9.dll", L"XAudio2_9.dll");
	archive.Insert(L".\\Binaries\\XInput1_4.dll", L"XInput1_4.dll");
	archive.Insert(L".\\Binaries\\Licenses\\DirectXTK12.txt", L"Licenses\\DirectXTK12.txt");
	archive.Insert(L".\\Binaries\\Licenses\\libogg.txt", L"Licenses\\libogg.txt");
	archive.Insert(L".\\Binaries\\Licenses\\libvorbis.txt", L"Licenses\\libvorbis.txt");

	Util::CFileSystem::Instance().NewPath(L"..\\Production\\Data\\");

	std::ofstream fileOut(L"..\\Production\\Data\\app.pack", std::ios::binary);
	archive.Save(fileOut);
	fileOut.close();

	return 0;
}
