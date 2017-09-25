// SysCtlDlg.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "FITOMApp.h"
#include "SysCtlDlg.h"
#include "afxdialogex.h"
#include "FITOMCfg.h"

// CSysCtlDlg �_�C�A���O

IMPLEMENT_DYNAMIC(CSysCtlDlg, CDialogEx)

CSysCtlDlg::CSysCtlDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_SYSCTL_DLG, pParent)
{

}

CSysCtlDlg::~CSysCtlDlg()
{
}

void CSysCtlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSysCtlDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_DEVINIT, &CSysCtlDlg::OnBnClickedBtnDevinit)
	ON_BN_CLICKED(IDC_BTN_RESTART, &CSysCtlDlg::OnBnClickedBtnRestart)
	ON_BN_CLICKED(IDC_BTN_RELOAD, &CSysCtlDlg::OnBnClickedBtnReload)
END_MESSAGE_MAP()


// CSysCtlDlg ���b�Z�[�W �n���h���[


void CSysCtlDlg::OnBnClickedBtnDevinit()
{
	// TODO: �����ɃR���g���[���ʒm�n���h���[ �R�[�h��ǉ����܂��B
	theApp.ResetInterface();
	::Sleep(1000);
	theApp.InitDevice();
}


void CSysCtlDlg::OnBnClickedBtnRestart()
{
	// TODO: �����ɃR���g���[���ʒm�n���h���[ �R�[�h��ǉ����܂��B
	theApp.FITOMStop();
	::Sleep(1000);
	theApp.FITOMStart();
}


void CSysCtlDlg::OnBnClickedBtnReload()
{
	// TODO: �����ɃR���g���[���ʒm�n���h���[ �R�[�h��ǉ����܂��B
}
