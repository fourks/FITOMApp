// DrumMapPicker.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "FITOMApp.h"
#include "DrumMapPicker.h"
#include "afxdialogex.h"
#include "FITOMCfg.h"

// CDrumMapPicker �_�C�A���O

IMPLEMENT_DYNAMIC(CDrumMapPicker, CDialogEx)

CDrumMapPicker::CDrumMapPicker(CWnd* pParent /*=NULL*/)
: CDialogEx(CDrumMapPicker::IDD, pParent), theConfig(((CFITOMApp*)AfxGetApp())->GetConfig()), theProg(0)
{

}

CDrumMapPicker::~CDrumMapPicker()
{
}

void CDrumMapPicker::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DRUMMAP_LIST, lstDrumMap);
}


BEGIN_MESSAGE_MAP(CDrumMapPicker, CDialogEx)
	ON_NOTIFY(NM_DBLCLK, IDC_DRUMMAP_LIST, &CDrumMapPicker::OnDblclkDrummapList)
END_MESSAGE_MAP()


// CDrumMapPicker ���b�Z�[�W �n���h���[


void CDrumMapPicker::OnDblclkDrummapList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: �����ɃR���g���[���ʒm�n���h���[ �R�[�h��ǉ����܂��B
	*pResult = 0;
	theProg = lstDrumMap.GetItemData(lstDrumMap.GetSelectionMark());
	OnOK();
}


BOOL CDrumMapPicker::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO: �����ɏ�������ǉ����Ă�������
	lstDrumMap.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	lstDrumMap.InsertColumn(0, _T("Prog."), LVCFMT_RIGHT, 40);
	lstDrumMap.InsertColumn(1, _T("Name"), LVCFMT_LEFT, 160);
	lstDrumMap.InsertColumn(2, _T("Path"), LVCFMT_LEFT, 200);

	if (theConfig) {
		int rows = 0;
		CString tmp;
		for (int i = 0; i < 128; i++) {
			CDrumBank* pbank = theConfig->GetDrumBank(i);
			if (pbank) {
				tmp.Format(_T("%i"), i);
				lstDrumMap.InsertItem(rows, tmp);
				lstDrumMap.SetItemText(rows, 1, pbank->GetBankName());
				lstDrumMap.SetItemText(rows, 2, pbank->GetFileName());
				lstDrumMap.SetItemData(rows, DWORD_PTR(i));
				if (i == theProg) {
					lstDrumMap.SetSelectionMark(rows);
				}
				rows++;
			}
		}
		if (lstDrumMap.GetSelectionMark() < 0) {
			lstDrumMap.SetSelectionMark(0);
		}
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	// ��O : OCX �v���p�e�B �y�[�W�͕K�� FALSE ��Ԃ��܂��B
}


void CDrumMapPicker::OnOK()
{
	// TODO: �����ɓ���ȃR�[�h��ǉ����邩�A�������͊��N���X���Ăяo���Ă��������B
	theProg = lstDrumMap.GetItemData(lstDrumMap.GetSelectionMark());
	CDialogEx::OnOK();
}
