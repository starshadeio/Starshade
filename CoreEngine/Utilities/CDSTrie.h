//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Engine
//
// File: Utilities/CDSTrie.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CDSTRIE_H
#define CDSTRIE_H

#include <vector>
#include <string>
#include <assert.h>
#include <initializer_list>
#include <functional>

namespace Util
{
	// Trie using nodes comprised of ASCII characters.
	template<typename T, typename STR, typename CH>
	class CDSTrie
	{
	public:
		struct Node
		{
			CH prefix = 0x0;
			bool bTerminal = false;
			T value = T();
			std::vector<Node*> children;
		};

		struct Elem
		{
			STR str;
			T value;
		};

	public:
		CDSTrie() : m_pHead(nullptr) { }
		CDSTrie(std::initializer_list<Elem> lst)
		{
			for(const auto& elem : lst)
			{
				AddSequence(elem.str, elem.value);
			}
		}

		~CDSTrie() { Clear(); }
		CDSTrie(const CDSTrie&) = delete;
		CDSTrie(CDSTrie&&) = delete;
		CDSTrie& operator = (const CDSTrie&) = delete;
		CDSTrie& operator = (CDSTrie&&) = delete;

		void AddSequence(const STR& str, const T& value)
		{
			assert(str.length() > 0);

			if(m_pHead == nullptr) { m_pHead = new Node(); }
			Node* pNode = m_pHead;

			for(int i = 0; i < str.length(); ++i)
			{
				const bool bEnd = i == str.length() - 1;
				bool bFound = false;

				for(Node* pChild : pNode->children)
				{
					if(pChild->prefix == str[i])
					{
						if(bEnd)
						{
							pChild->value = value;
							pChild->bTerminal = true;
							return;
						}
						else
						{
							pNode = pChild;
							bFound = true;
							break;
						}
					}
				}

				if(!bFound)
				{
					Node* pNewNode = new Node();
					pNewNode->prefix = str[i];
					pNewNode->bTerminal = bEnd;

					if(bEnd)
					{
						pNewNode->value = value;
						pNode->children.push_back(pNewNode);
						return;
					}
					else
					{
						pNode->children.push_back(pNewNode);
						pNode = pNewNode;
					}
				}
			}
		}

		Node* Step(CH ch, Node* pNode) const
		{
			if(pNode == nullptr) pNode = m_pHead;
			for(Node* pChild : pNode->children)
			{
				if(pChild->prefix == ch)
				{
					return pChild;
				}
			}

			return nullptr;
		}

		Node* Insert(CH ch, Node* pNode)
		{
			Node* pNewNode = new Node();
			pNewNode->prefix = ch;
			pNewNode->bTerminal = false;
			pNode->children.push_back(pNewNode);
			return pNewNode;
		}

		bool Find(const STR& str, T& output) const
		{
			Node* pNode = m_pHead;
			for(int i = 0; i < str.length(); ++i)
			{
				const bool bEnd = i == str.length() - 1;
				bool bFound = false;

				for(Node* pChild : pNode->children)
				{
					if(pChild->prefix == str[i])
					{
						if(bEnd)
						{
							output = pChild->value;
							return pChild->bTerminal;
						}
						else
						{
							pNode = pChild;
							bFound = true;
							break;
						}
					}
				}

				if(!bFound)
				{
					return false;
				}
			}

			return false;
		}

		void Clear() { Clear(m_pHead); m_pHead = nullptr; }

		inline Node* GetHead() const { return m_pHead; }
		inline Node* GetOrCreateHead() { if(m_pHead == nullptr) { m_pHead = new Node(); } return m_pHead; }
		inline void SetOnDelete(const std::function<void(T*)>& onDelete) { m_onDelete = onDelete; }

	private:
		void Clear(Node* pNode)
		{
			if(pNode == nullptr) return;

			for(Node* pChild : pNode->children)
			{
				Clear(pChild);
			}

			if(pNode->bTerminal && m_onDelete) { m_onDelete(&pNode->value); }
			delete pNode;
		}

	private:
		std::function<void(T*)> m_onDelete;
		Node* m_pHead;
	};

	// Trie using nodes comprised of ASCII characters.
	template<typename C, typename T>
	class CDSTrieGeneric
	{
	public:
		struct Node
		{
			C prefix = C();
			bool bTerminal = false;
			T value = T();
			std::vector<Node*> children;
		};

		struct Elem
		{
			std::vector<C> seq;
			T value;
		};

	public:
		CDSTrieGeneric() : m_pHead(nullptr) { }
		CDSTrieGeneric(std::initializer_list<Elem> lst)
		{
			for(const auto& elem : lst)
			{
				AddSequence(elem.seq, elem.value);
			}
		}

		~CDSTrieGeneric() { Clear(); }
		CDSTrieGeneric(const CDSTrieGeneric&) = delete;
		CDSTrieGeneric(CDSTrieGeneric&&) = delete;
		CDSTrieGeneric& operator = (const CDSTrieGeneric&) = delete;
		CDSTrieGeneric& operator = (CDSTrieGeneric&&) = delete;

		void AddSequence(const std::vector<C>& seq, const T& value)
		{
			assert(seq.size() > 0);

			if(m_pHead == nullptr) { m_pHead = new Node(); }
			Node* pNode = m_pHead;

			for(int i = 0; i < seq.size(); ++i)
			{
				const bool bEnd = i == seq.size() - 1;
				bool bFound = false;

				for(Node* pChild : pNode->children)
				{
					if(pChild->prefix == seq[i])
					{
						if(bEnd)
						{
							pChild->value = value;
							pChild->bTerminal = true;
							return;
						}
						else
						{
							pNode = pChild;
							bFound = true;
							break;
						}
					}
				}

				if(!bFound)
				{
					Node* pNewNode = new Node();
					pNewNode->prefix = seq[i];
					pNewNode->bTerminal = bEnd;

					if(bEnd)
					{
						pNewNode->value = value;
						pNode->children.push_back(pNewNode);
						return;
					}
					else
					{
						pNode->children.push_back(pNewNode);
						pNode = pNewNode;
					}
				}
			}
		}

		Node* Step(C elem, Node* pNode) const
		{
			if(pNode == nullptr) pNode = m_pHead;

			for(Node* pChild : pNode->children)
			{
				if(pChild->prefix == elem)
				{
					return pChild;
				}
			}

			return nullptr;
		}

		Node* Insert(C elem, Node* pNode)
		{
			Node* pNewNode = new Node();
			pNewNode->prefix = elem;
			pNewNode->bTerminal = false;
			pNode->children.push_back(pNewNode);
			return pNewNode;
		}

		bool Find(const std::vector<C>& seq, T& output) const
		{
			Node* pNode = m_pHead;
			for(int i = 0; i < seq.size(); ++i)
			{
				const bool bEnd = i == seq.size() - 1;
				bool bFound = false;

				for(Node* pChild : pNode->children)
				{
					if(pChild->prefix == seq[i])
					{
						if(bEnd)
						{
							output = pChild->value;
							return pChild->bTerminal;
						}
						else
						{
							pNode = pChild;
							bFound = true;
							break;
						}
					}
				}

				if(!bFound)
				{
					return false;
				}
			}

			return false;
		}

		inline void Clear() { Clear(m_pHead); m_pHead = nullptr; }

		inline Node* GetHead() const { return m_pHead; }
		inline Node* GetOrCreateHead() { if(m_pHead == nullptr) { m_pHead = new Node(); } return m_pHead; }
		inline void SetOnDelete(const std::function<void(T*)>& onDelete) { m_onDelete = onDelete; }

	private:
		void Clear(Node* pNode)
		{
			if(pNode == nullptr) return;

			for(Node* pChild : pNode->children)
			{
				Clear(pChild);
			}

			if(pNode->bTerminal && m_onDelete)
			{
				m_onDelete(&pNode->value);
			}

			delete pNode;
		}

	private:
		std::function<void(T*)> m_onDelete;
		Node* m_pHead;
	};
};

#endif
