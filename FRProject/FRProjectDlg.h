
// FRProjectDlg.h : header file
//

#pragma once
#include "SubFunctions.h"
#include "FrameGrabber.h"
#include "FaceFunctions.h"
#include "Register_Dialog.h"
#include "FaceTraining.h"
#include "Setup.h"
#include "cmscomm.h"

// CFRProjectDlg dialog
class CFRProjectDlg : public CDialogEx
{
// Construction
public:
	CFRProjectDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_FRPROJECT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

public:
//variables
	IplImage	*m_frame, *m_matchImg, *m_unknownPerson, *m_noface, *m_notify;

	CWinThread	*m_thrdFR;
	bool m_bthrdStart;
	bool m_faceDectected;
	bool m_bPortConnected;
	bool m_flagID;
	UINT Display();

	CvMat *faceImg8;
	SMatch	matchInfo;

	Register_Dialog m_register;
	CFaceTraining m_train;
	CSetup m_setup;
	void PortSettings(void);
	void SetDefaultItems(void);
	void NotifyInfo(CString Status, CString icon);
// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButtonTrain();
	afx_msg void OnBnClickedButtonRegister();
	afx_msg void OnStnClickedStatus();
	afx_msg void OnBnClickedFrprojExit();
	DECLARE_EVENTSINK_MAP()
	void OnCommMscomm();
	CMSComm m_mscomm;
	afx_msg void OnBnClickedButtonSetup();
};
UINT RunDialog( LPVOID pParam );