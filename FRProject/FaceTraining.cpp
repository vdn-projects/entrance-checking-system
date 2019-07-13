// FaceTraining.cpp : implementation file
//

#include "stdafx.h"
#include "FRProject.h"
#include "FaceTraining.h"
#include "afxdialogex.h"

#define DEFAULT_DIR_FOR_TRAIN "D:\\FaceDatabase"


// CFaceTraining dialog

IMPLEMENT_DYNAMIC(CFaceTraining, CDialog)

CFaceTraining::CFaceTraining(CWnd* pParent /*=NULL*/)
	: CDialog(CFaceTraining::IDD, pParent)
	, m_SaveFace(TRUE)
	, m_strFaceDir(_T(DEFAULT_DIR_FOR_TRAIN))
	, m_SaveXml(TRUE)
{

}

CFaceTraining::~CFaceTraining()
{
}

void CFaceTraining::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_SAVE_FACES, m_SaveFace);
	DDX_Text(pDX, IDC_EDIT1, m_strFaceDir);
	DDX_Check(pDX, IDC_CHECK_SAVE_XML, m_SaveXml);
}


BEGIN_MESSAGE_MAP(CFaceTraining, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE, &CFaceTraining::OnBnClickedButtonBrowse)
	ON_BN_CLICKED(IDOK, &CFaceTraining::OnBnClickedOk)
	ON_BN_CLICKED(ID_BUTTON_CANCEL, &CFaceTraining::OnBnClickedButtonCancel)
END_MESSAGE_MAP()


// CFaceTraining message handlers


void CFaceTraining::OnBnClickedButtonBrowse()
{
	// TODO: Add your control notification handler code here
	bool ret = SelDirectory(NULL, "Select face images directory for training", m_strFaceDir);
	UpdateData(FALSE);
}



void CFaceTraining::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	CDialog::OnOK();
}


void CFaceTraining::OnBnClickedButtonCancel()
{
	// TODO: Add your control notification handler code here
	CDialog::OnCancel();
}
