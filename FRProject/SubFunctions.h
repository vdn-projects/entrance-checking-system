#pragma once
#include "stdafx.h"
#include "GlobalHeader.h"


void EmbedCvWindow(HWND pWnd, CString strWndName, int w, int h);
bool SelDirectory ( HWND hWnd, LPCTSTR strTitle, CString &strDir );