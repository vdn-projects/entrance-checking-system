#pragma once
#include "FrameGrabber.h"
#include "SubFunctions.h"
#include "FaceFunctions.h"

// Register_Dialog dialog

class Register_Dialog : public CDialog
{
	DECLARE_DYNAMIC(Register_Dialog)

public:
	Register_Dialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~Register_Dialog();

	CWinThread	*m_thrdReg;
	BOOL OnInitDialog();
	UINT reg_Display();
	
	IplImage	*notfoundFace;
	IplImage	*m_frame;
	CvMat *m_faceImg8;
	bool m_bRegStart;
	bool m_bFacefound;

	CList<IplImage *> m_Face4Train;
	CList<IplImage *> m_Face4Store;


// Dialog Data
	enum { IDD = IDD_REGISTER_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonCapture();
	afx_msg void OnBnClickedButtonSave();
	afx_msg void OnBnClickedButtonNewone();
	afx_msg void OnBnClickedButton4();
	CString m_strName;
	CString m_strID;
	int m_NumFaces;
	bool m_bSaveImg;
	afx_msg void OnBnClickedButtonDone();
	afx_msg void OnBnClickedButtonCancel();
};

UINT RunRegDialog( LPVOID pParam );
