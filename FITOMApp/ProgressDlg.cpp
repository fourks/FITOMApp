// ProgressDlg.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "FITOMApp.h"
#include "ProgressDlg.h"
#include "afxdialogex.h"


// CProgressDlg �_�C�A���O

IMPLEMENT_DYNAMIC(CProgressDlg, CDialogEx)

CProgressDlg::CProgressDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CProgressDlg::IDD, pParent)
{
	Create(IDD, pParent);
}

CProgressDlg::~CProgressDlg()
{
}

void CProgressDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS, progBar);
}


BEGIN_MESSAGE_MAP(CProgressDlg, CDialogEx)
END_MESSAGE_MAP()


// CProgressDlg ���b�Z�[�W �n���h���[


void CProgressDlg::PostNcDestroy()
{
	// TODO: �����ɓ���ȃR�[�h��ǉ����邩�A�������͊��N���X���Ăяo���Ă��������B

	delete this;
}


void CProgressDlg::OnOK()
{
	// TODO: �����ɓ���ȃR�[�h��ǉ����邩�A�������͊��N���X���Ăяo���Ă��������B

	CDialogEx::DestroyWindow();
}


void CProgressDlg::OnCancel()
{
	// TODO: �����ɓ���ȃR�[�h��ǉ����邩�A�������͊��N���X���Ăяo���Ă��������B

	CDialogEx::DestroyWindow();
}


void CProgressDlg::SetRange(int lower, int upper)
{
	progBar.SetRange32(lower, upper);
}


int CProgressDlg::SetPos(int pos)
{
	return progBar.SetPos(pos);
}


void CProgressDlg::SetStatus(LPCTSTR str)
{
	GetDlgItem(IDC_STATUS_MSG)->SetWindowText(str);
}


void CProgressDlg::SetTarget(LPCTSTR str)
{
	GetDlgItem(IDC_STATUS_TGT)->SetWindowText(str);
}
