//-------------------------------------------------------------------------------------------------
//
// Copyright (c) Ryan Alasandro
//
// Static Library: Core Engine
//
// File: Utilities/CMemoryFree.h
//
//-------------------------------------------------------------------------------------------------

#ifndef CMEMORYFREE_H
#define CMEMORYFREE_H

// Memory free macros.
#define SAFE_RELEASE(x) if(x) { (x)->Release(); (x) = nullptr; }
#define SAFE_RELEASE_DELETE(x) if(x) { (x)->Release(); delete (x); (x) = nullptr; }
#define SAFE_DELETE(x) { delete (x); (x) = nullptr; }
#define SAFE_DELETE_ARRAY(x) { if(x) { delete [] (x); (x) = nullptr; } }

#endif
