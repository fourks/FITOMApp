// LineInputDlg.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "FITOMcfg.h"
#include <mmsystem.h>
#include <mmdeviceapi.h>
#include "LineInputDlg.h"
#include "afxdialogex.h"

// CLineInputDlg �_�C�A���O

IMPLEMENT_DYNAMIC(CLineInputDlg, CDialogEx)

CLineInputDlg::CLineInputDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_MVOL_DLG, pParent)
{

}

CLineInputDlg::~CLineInputDlg()
{
}

void CLineInputDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CMB_LINEIN, cmbLineIn);
}


BEGIN_MESSAGE_MAP(CLineInputDlg, CDialogEx)
END_MESSAGE_MAP()


// CLineInputDlg ���b�Z�[�W �n���h���[
