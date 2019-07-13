#pragma once
#include "SubFunctions.h"

// CFaceTraining dialog

class CFaceTraining : public CDialog
{
	DECLARE_DYNAMIC(CFaceTraining)

public:
	CFaceTraining(CWnd* pParent = NULL);   // standard constructor
	virtual ~CFaceTraining();

// Dialog Data
	enum { IDD = IDD_FACETRAINING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	BOOL m_SaveFace;
	afx_msg void OnBnClickedButtonBrowse();
	CString m_strFaceDir;
	afx_msg void OnBnClickedOk();
	BOOL m_SaveXml;
	afx_msg void OnBnClickedButtonCancel();
};
