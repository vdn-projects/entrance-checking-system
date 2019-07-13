// Setup.cpp : implementation file
//

#include "stdafx.h"
#include "FRProject.h"
#include "Setup.h"
#include "afxdialogex.h"

#define DEFAULT_DATABASE_PATH "D://XML Database//database.xml"

// CSetup dialog

IMPLEMENT_DYNAMIC(CSetup, CDialog)

CSetup::CSetup(CWnd* pParent /*=NULL*/)
	: CDialog(CSetup::IDD, pParent)
	, m_strDatabasePath(_T(DEFAULT_DATABASE_PATH))
{
}

CSetup::~CSetup()
{
}

void CSetup::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_PORT, m_cboComPort);
}


BEGIN_MESSAGE_MAP(CSetup, CDialog)
	ON_BN_CLICKED(IDCANCEL, &CSetup::OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, &CSetup::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_LOAD_DATABASE, &CSetup::OnBnClickedButtonLoadDatabase)
END_MESSAGE_MAP()

BOOL CSetup::OnInitDialog()
{
	CDialog::OnInitDialog();
	InitComboBox();

	return TRUE;
}

// CSetup message handlers

void CSetup::InitComboBox(void)
{
	m_cboComPort.ResetContent();
	m_cboComPort.InsertString(0, "COM1");
	m_cboComPort.InsertString(1, "COM2");
	m_cboComPort.InsertString(2, "COM3");
	m_cboComPort.InsertString(3, "COM4");
	m_cboComPort.InsertString(4, "COM5");
	m_cboComPort.InsertString(5, "COM6");
	m_cboComPort.InsertString(6, "COM7");
	m_cboComPort.InsertString(7, "COM8");
	m_cboComPort.InsertString(8, "COM9");
	m_cboComPort.InsertString(9, "COM10");
	m_cboComPort.InsertString(10, "COM11");
	m_cboComPort.InsertString(11, "COM12");
	m_cboComPort.InsertString(12, "COM13");
	m_cboComPort.InsertString(12, "COM14");
	m_cboComPort.InsertString(12, "COM15");
	m_cboComPort.InsertString(12, "COM16");
	m_cboComPort.SetCurSel(2);
	
}


void CSetup::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	m_portnum = m_cboComPort.GetCurSel() + 1;
	CDialog::OnOK();
}

void CSetup::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	CDialog::OnCancel();
}

void CSetup::OnBnClickedButtonLoadDatabase()
{
	// TODO: Add your control notification handler code here
	CFileDialog database(TRUE, "xml", 0, OFN_NOCHANGEDIR | OFN_HIDEREADONLY, 
									"xml files(*.xml)|*.xml||");

	if (database.DoModal() == IDOK)
	{
		m_strDatabasePath = database.GetPathName();
	}

}


