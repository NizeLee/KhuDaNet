//
//	Dept. Software Convergence, Kyung Hee University
//	Prof. Daeho Lee, nize@khu.ac.kr
//

// KhuDaNetApp.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CKhuDaNetAppApp:
// See KhuDaNetApp.cpp for the implementation of this class
//

class CKhuDaNetAppApp : public CWinApp
{
public:
	CKhuDaNetAppApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CKhuDaNetAppApp theApp;