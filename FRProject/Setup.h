#pragma once
#include "afxwin.h"


// CSetup dialog

class CSetup : public CDialog
{
	DECLARE_DYNAMIC(CSetup)

public:
	CSetup(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSetup();

// Dialog Data
	enum { IDD = IDD_SETUP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	CString m_strDatabasePath;
	CComboBox m_cboComPort;
	void InitComboBox(void);

	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButtonLoadDatabase();
	int m_portnum;
};

