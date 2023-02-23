//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Graphics
//
// File: Graphics/CShaderTrie.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CSHADERTRIE_H
#define CSHADERTRIE_H

#include "CShaderFile.h"
#include <Globals/CGlobals.h>
#include <unordered_map>
#include <vector>
#include <string>
#include <fstream>

namespace Graphics
{
	class CShaderTrie
	{
	public:
		struct Data
		{
			std::wstring filename;
		};

	public:
		struct Node
		{
			std::vector<std::string> macroList;
			class CShader* pShader = nullptr;
			std::unordered_map<std::string, Node*> children;
		};

	public:
		CShaderTrie();
		~CShaderTrie();
		CShaderTrie(const CShaderTrie&) = delete;
		CShaderTrie(CShaderTrie&&) = delete;
		CShaderTrie& operator = (const CShaderTrie&) = delete;
		CShaderTrie& operator = (CShaderTrie&&) = delete;

		void Setup();
		void BuildCSH();
		void Release();

		Node* Insert(const std::string& macro, bool bTerminal, Node* pNode = nullptr);

		// Modifiers.
		inline void SetData(const Data& data) { m_data = data; }

	private:
		void BuildCSHAppend(std::ofstream& file, Node* pNode);
		void LoadCSH();
		void Release(Node* pNode);

	private:
		Data m_data;
		CShaderFile m_shaderFile;
		Node* m_pBase;
	};
};

#endif
