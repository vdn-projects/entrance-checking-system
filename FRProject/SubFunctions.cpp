#include "stdafx.h"
#include "SubFunctions.h"


void EmbedCvWindow( HWND pWnd, CString strWndName, int w, int h )
{
	cvDestroyWindow(strWndName);
	cvNamedWindow(strWndName, 0);
	HWND hWnd = (HWND) cvGetWindowHandle(strWndName);
	HWND hParent = ::GetParent(hWnd);
	::SetParent(hWnd, pWnd);
	::ShowWindow(hParent, SW_HIDE);
	::SetWindowPos(pWnd, NULL, 0,0, w,h, SWP_NOMOVE | SWP_NOZORDER);
	cvResizeWindow(strWndName, w,h);
}

int CALLBACK BrowserCallbackProc
(
 HWND   hWnd,
 UINT   uMsg,
 LPARAM   lParam,
 LPARAM   lpData
 )
{
	switch(uMsg)  
	{  
	case BFFM_INITIALIZED:  
		::SendMessage ( hWnd, BFFM_SETSELECTION, 1, lpData );  
		break;  
	default:  
		break;  
	}  
	return 0;  
}  

bool SelDirectory ( HWND hWnd, LPCTSTR strTitle, CString &strDir )  
{  
	BROWSEINFO bi;  
	char szDisplayName[MAX_PATH] = {0};  

	bi.hwndOwner = hWnd;  
	bi.pidlRoot = NULL;  
	bi.pszDisplayName = szDisplayName;  
	bi.lpszTitle = strTitle;  
	bi.ulFlags = 0;  
	bi.lpfn = BrowserCallbackProc;  
	bi.lParam = (LPARAM)(LPCTSTR)strDir;  
	bi.iImage = NULL;  

	ITEMIDLIST* piid = ::SHBrowseForFolder ( &bi );  

	if ( piid == NULL )  return false;  

	BOOL bValidPath = ::SHGetPathFromIDList ( piid, szDisplayName );  
	if ( ! bValidPath ) return false;  

	LPMALLOC lpMalloc;  
	HRESULT hr = ::SHGetMalloc ( &lpMalloc );
	assert(hr == NOERROR);  
	lpMalloc->Free ( piid );  
	lpMalloc->Release ();  

	if ( szDisplayName[0] == '\0' ) return false; 

	strDir = szDisplayName;  

	return true;
}
