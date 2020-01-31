// DrumEditDlg.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "FITOM.h"
#include "FITOMdefine.h"
#include "SoundDev.h"
#include "MIDI.h"
#include "FITOMApp.h"
#include "DrumEditDlg.h"
#include "DrumMapPicker.h"
#include "DrumNoteEditDlg.h"
#include "afxdialogex.h"
#include "FITOMCfg.h"


// CDrumEditDlg �_�C�A���O

IMPLEMENT_DYNAMIC(CDrumEditDlg, CDialogEx)

CDrumEditDlg::CDrumEditDlg(CWnd* pParent /*=NULL*/)
: CDialogEx(CDrumEditDlg::IDD, pParent), bModified(FALSE),
	theConfig(((CFITOMApp*)AfxGetApp())->GetConfig()), theProg(0)
{
	pCH = new CRhythmCh(0, CFITOM::GetInstance());
}

CDrumEditDlg::~CDrumEditDlg()
{
	delete pCH;
}

void CDrumEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_DRUMNOTE, lstDrumNote);
	DDX_Control(pDX, IDC_EDIT_DRUMMAP, edtDrumMapName);
}


BEGIN_MESSAGE_MAP(CDrumEditDlg, CDialogEx)
	ON_MESSAGE(WM_KICKIDLE, &CDrumEditDlg::OnKickIdle)
	ON_BN_CLICKED(IDC_BTN_PICK, &CDrumEditDlg::OnBnClickedBtnPick)
	ON_COMMAND(IDC_BTN_TEST, &CDrumEditDlg::OnBtnTest)
	ON_UPDATE_COMMAND_UI(IDC_BTN_TEST, &CDrumEditDlg::OnUpdateBtnSelection)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_DRUMNOTE, &CDrumEditDlg::OnNMDblclkListDrumnote)
	ON_COMMAND(IDC_BTN_PASTE, &CDrumEditDlg::OnBtnPaste)
	ON_UPDATE_COMMAND_UI(IDC_BTN_PASTE, &CDrumEditDlg::OnUpdateBtnPaste)
	ON_COMMAND(IDC_BTN_COPY, &CDrumEditDlg::OnBtnCopy)
	ON_UPDATE_COMMAND_UI(IDC_BTN_COPY, &CDrumEditDlg::OnUpdateBtnSelection)
	ON_COMMAND(IDC_BTN_SAVE, &CDrumEditDlg::OnBtnSave)
	ON_UPDATE_COMMAND_UI(IDC_BTN_SAVE, &CDrumEditDlg::OnUpdateBtnSave)
	//ON_UPDATE_COMMAND_UI(IDOK, &CDrumEditDlg::OnUpdateBtnSelection)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CDrumEditDlg ���b�Z�[�W �n���h���[


void CDrumEditDlg::OnOK()
{
	// TODO: �����ɓ���ȃR�[�h��ǉ����邩�A�������͊��N���X���Ăяo���Ă��������B
	if (bModified) {
		OnBtnSave();
	}
	CDialogEx::OnOK();
}

void CDrumEditDlg::OnBtnSave()
{
	// TODO: �����ɃR���g���[���ʒm�n���h���[ �R�[�h��ǉ����܂��B
	((CFITOMApp*)AfxGetApp())->SaveVoice(VOICE_GROUP_RHYTHM, theProg);
	bModified = FALSE;
}

BOOL CDrumEditDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO: �����ɏ�������ǉ����Ă�������
	lstDrumNote.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	lstDrumNote.InsertColumn(0, _T("Note#"), LVCFMT_RIGHT, 40);
	lstDrumNote.InsertColumn(1, _T("Name"), LVCFMT_LEFT, 160);
	lstDrumNote.InsertColumn(2, _T("Device"), LVCFMT_LEFT, 160);
	//lstDrumNote.InsertColumn(3, _T("Type"), LVCFMT_LEFT, 100);

	Refresh();
	SetTimer(IDD, 10, NULL);
	return TRUE;  // return TRUE unless you set the focus to a control
	// ��O : OCX �v���p�e�B �y�[�W�͕K�� FALSE ��Ԃ��܂��B
}

void CDrumEditDlg::Refresh()
{
	int prevsel = lstDrumNote.GetSelectionMark();
	int prevpos = lstDrumNote.GetTopIndex();
	lstDrumNote.DeleteAllItems();
	int top = prevpos;
	if (theConfig) {
		int rows = 0;
		CDrumBank* pbank = theConfig->GetDrumBank(theProg);
		if (pbank) {
			edtDrumMapName.SetWindowText(pbank->GetBankName());
			for (int i = 0; i < 128; i++) {
				TCHAR tmp[256];
				DRUMMAP drum;
				pbank->GetVoice(i, &drum);
				StringCchPrintf(tmp, 33, _T("%i"), i);
				lstDrumNote.InsertItem(rows, tmp);
				if (drum.device) {
					DWORD did = drum.devID & 0xff;
					DWORD ifid = (drum.devID >> 16) & 0xff;
					DWORD slid = (drum.devID >> 8) & 0xff;
					CString str;
					if (drum.devID & 0x80000000) { //logical device
						str.Format(_T("%02X:%s"), did, CFITOM::GetDeviceNameFromID(drum.devID));
					}
					else if (theConfig->isPcmDevice(drum.devID)) { // PCM device
						theConfig->GetDeviceName(drum.devID, tmp, _countof(tmp));
						str.Format(_T("%02X:%s (%02X:%02X)"), did, tmp, ifid, slid);
					}
					else { //physical device
						str.Format(_T("%02X:%s (%02X:%02X)"), did, CFITOM::GetDeviceNameFromID(did), ifid, slid);
					}
					lstDrumNote.SetItemText(rows, 1, drum.name);
					lstDrumNote.SetItemText(rows, 2, str);
					//lstDrumNote.SetItemText(rows, 3, (drum.num & 0x80) ? _T("Internal") : _T("Tone"));
					if (top < 0) { top = i; }
				}
				else {
					lstDrumNote.SetItemText(rows, 1, _T("EMPTY"));
					lstDrumNote.SetItemText(rows, 2, _T("----"));
					//lstDrumNote.SetItemText(rows, 3, (drum.num & 0x80) ? _T("Internal") : _T("Tone"));
				}
				lstDrumNote.SetItemData(rows, DWORD_PTR(i));
				rows++;
			}
		}
		if (prevsel >= 0) {
			LVITEM lvi;
			lvi.state = lstDrumNote.GetItemState(prevsel, -1) | LVNI_FOCUSED | LVNI_SELECTED;
			lvi.stateMask = -1;
			lstDrumNote.SetItemState(prevsel, &lvi);
			lstDrumNote.SetSelectionMark(prevsel);
		}
		if (top >= 0) {
			CRect rc;
			lstDrumNote.GetItemRect(0, &rc, LVIR_BOUNDS);
			lstDrumNote.Scroll(CSize(0, top * rc.Height()));
		}
	}
}

void CDrumEditDlg::OnBnClickedBtnPick()
{
	// TODO: �����ɃR���g���[���ʒm�n���h���[ �R�[�h��ǉ����܂��B
	CDrumMapPicker dlg(this);
	dlg.SetProg(theProg);
	if (dlg.DoModal() == IDOK) {
		theProg = dlg.GetProg();
		Refresh();
	}
}

void CDrumEditDlg::OnBtnTest()
{
	// TODO: �����ɃR���g���[���ʒm�n���h���[ �R�[�h��ǉ����܂��B
	pCH->AllNoteOff();
	pCH->SetVolume(127);
	pCH->SetExpress(127);
	int note = lstDrumNote.GetItemData(lstDrumNote.GetSelectionMark());
	CDrumBank* pbank = theConfig->GetDrumBank(theProg);
	if (pbank) {
		DRUMMAP drumnote;
		pbank->GetVoice(note, &drumnote);
		pCH->NoteOn(note, 127, &drumnote);
	}
}


void CDrumEditDlg::OnBtnCopy()
{
	// TODO: �����ɃR���g���[���ʒm�n���h���[ �R�[�h��ǉ����܂��B
	CDrumBank* pbank = theConfig->GetDrumBank(theProg);
	DRUMMAP drumnote;
	pbank->GetVoice(lstDrumNote.GetSelectionMark(), &drumnote);
	((CFITOMApp*)AfxGetApp())->DrumCopy(&drumnote);
}


void CDrumEditDlg::OnBtnPaste()
{
	// TODO: �����ɃR���g���[���ʒm�n���h���[ �R�[�h��ǉ����܂��B
	CDrumBank* pbank = theConfig->GetDrumBank(theProg);
	DRUMMAP drumnote;
	((CFITOMApp*)AfxGetApp())->GetDrumFromClipBoard(&drumnote);
	pbank->SetVoice(lstDrumNote.GetSelectionMark(), &drumnote);
	bModified = TRUE;
	Refresh();
}

void CDrumEditDlg::OnUpdateBtnPaste(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(((CFITOMApp*)AfxGetApp())->IsDrumInClipBoard()
		&& (lstDrumNote.GetSelectionMark() >= 0));
}

void CDrumEditDlg::OnUpdateBtnSelection(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(lstDrumNote.GetSelectionMark() >= 0);
}

void CDrumEditDlg::OnUpdateBtnSave(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(bModified);
}

void CDrumEditDlg::OnNMDblclkListDrumnote(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: �����ɃR���g���[���ʒm�n���h���[ �R�[�h��ǉ����܂��B
	*pResult = 0;
	int note = lstDrumNote.GetItemData(lstDrumNote.GetSelectionMark());
	CDrumNoteEditDlg dlg(pCH, this);
	dlg.SetProg(theProg);
	dlg.SetNote(note);
	if (dlg.DoModal() == IDOK) {
		bModified = TRUE;
		CDrumBank* pbank = theConfig->GetDrumBank(theProg);
		DRUMMAP drumnote;
		dlg.GetDrumNote(&drumnote);
		pbank->SetVoice(note, &drumnote);
		Refresh();
	}
}

LRESULT CDrumEditDlg::OnKickIdle(WPARAM wParam, LPARAM lParam)
{
	UpdateDialogControls(this, FALSE);

	return FALSE;
}

void CDrumEditDlg::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class
	if (bModified) {
		int res = AfxMessageBox(IDS_CONFIRM_VOICE_DISCARD, MB_YESNO);
		if (res == IDNO) {
			return;
		}
	}
	pCH->AllNoteOff();
	CDialogEx::OnCancel();
}


void CDrumEditDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �����Ƀ��b�Z�[�W �n���h���[ �R�[�h��ǉ����邩�A����̏������Ăяo���܂��B
	if (pCH) {
		pCH->TimerCallBack(0);
	}
	CDialogEx::OnTimer(nIDEvent);
}
