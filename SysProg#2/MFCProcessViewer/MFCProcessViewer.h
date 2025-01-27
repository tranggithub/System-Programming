
// MFCProcessViewer.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CMFCProcessViewerApp:
// See MFCProcessViewer.cpp for the implementation of this class
//

class CMFCProcessViewerApp : public CWinApp
{
public:
	CMFCProcessViewerApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CMFCProcessViewerApp theApp;
