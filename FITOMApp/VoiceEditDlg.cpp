// VoiceEditDlg.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "FITOM.h"
#include "FITOMApp.h"
#include "VoiceEditDlg.h"
#include "VoicePicker.h"
#include "afxdialogex.h"
#include "FITOMCfg.h"
#include "TextVoiceEditDlg.h"

// �f�[�^�e�[�u��
CVoiceEditDlg::VoiceItem CVoiceEditDlg::commonItem[] = {
	{ _T("Feedback"), 0, 0, 7, VOICE_GROUP_OPM | VOICE_GROUP_OPNA | VOICE_GROUP_OPL2 | VOICE_GROUP_OPLL, &CVoiceEditDlg::SetFB, &CVoiceEditDlg::GetFB, },
	{ _T("Feedback1"), 0, 0, 7, VOICE_GROUP_OPL3, &CVoiceEditDlg::SetFB1, &CVoiceEditDlg::GetFB1, },
	{ _T("Feedback2"), 0, 0, 7, VOICE_GROUP_OPL3, &CVoiceEditDlg::SetFB2, &CVoiceEditDlg::GetFB2, },
	{ _T("Algorhythm"), 0, 0, 7, VOICE_GROUP_OPM, &CVoiceEditDlg::SetAL3, &CVoiceEditDlg::GetAL3, },
	{ _T("Algorhythm"), 0, 0, 7, VOICE_GROUP_OPNA, &CVoiceEditDlg::SetAL, &CVoiceEditDlg::GetAL3, },
	{ _T("Algorhythm"), 0, 0, 1, VOICE_GROUP_OPL2, &CVoiceEditDlg::SetAL, &CVoiceEditDlg::GetAL1, },
	{ _T("Algorhythm"), 0, 0, 11, VOICE_GROUP_OPL3, &CVoiceEditDlg::SetAL, &CVoiceEditDlg::GetAL4, },
	{ _T("ROM Tone"), 0, 0, 15, VOICE_GROUP_OPLL, &CVoiceEditDlg::SetROMTN, &CVoiceEditDlg::GetAL4, },
	{ _T("Tone/Noise"), 0, 0, 3, VOICE_GROUP_PSG, &CVoiceEditDlg::SetAL, &CVoiceEditDlg::GetAL, },
	{ _T("Noise Enable"), 0, 0, 3, VOICE_GROUP_OPM, &CVoiceEditDlg::SetNE, &CVoiceEditDlg::GetNE, },
	{ _T("Noise Freq"), 0, 0, 63, VOICE_GROUP_OPM | VOICE_GROUP_PSG, &CVoiceEditDlg::SetNFreq, &CVoiceEditDlg::GetNFreq, },
	{ _T("Noise Freq2"), 0, 0, 63, VOICE_GROUP_PSG, &CVoiceEditDlg::SetNFreq2, &CVoiceEditDlg::GetNFreq2, },
	{ _T("AM Secse"), 0, 0, 3, VOICE_GROUP_OPM | VOICE_GROUP_OPNA, &CVoiceEditDlg::SetAMS, &CVoiceEditDlg::GetAMS, },
	{ _T("PM Secse"), 0, 0, 7, VOICE_GROUP_OPM | VOICE_GROUP_OPNA, &CVoiceEditDlg::SetPMS, &CVoiceEditDlg::GetPMS, },
	{ _T("P-LFO Wave"), 0, 0, 14, VOICE_GROUP_ALL, &CVoiceEditDlg::SetLFOWave, &CVoiceEditDlg::GetLFOWave, },
	{ _T("P-LFO Depth"), 0, -8192, 8191, VOICE_GROUP_ALL, &CVoiceEditDlg::SetLFODepth, &CVoiceEditDlg::GetLFODepth, },
	{ _T("P-LFO Freq"), 0, 0, 15, VOICE_GROUP_ALL, &CVoiceEditDlg::SetLFOFreq, &CVoiceEditDlg::GetLFOFreq, },
	{ _T("P-LFO Delay"), 0, 0, 127, VOICE_GROUP_ALL, &CVoiceEditDlg::SetLFODelay, &CVoiceEditDlg::GetLFODelay, },
	{ _T("P-LFO Rate"), 0, 0, 127, VOICE_GROUP_ALL, &CVoiceEditDlg::SetLFORate, &CVoiceEditDlg::GetLFORate, },
	{ 0, 0, 0, 0, VOICE_GROUP_NONE, 0, 0, },
};

CVoiceEditDlg::VoiceItem CVoiceEditDlg::operatorItem[] = {
	{ _T("Attack Rate"), 0, 0, 31, VOICE_GROUP_OPM | VOICE_GROUP_OPNA, &CVoiceEditDlg::SetAR5, &CVoiceEditDlg::GetAR5, },
	{ _T("Decay Rate"), 0, 0, 31, VOICE_GROUP_OPM | VOICE_GROUP_OPNA, &CVoiceEditDlg::SetDR5, &CVoiceEditDlg::GetDR5, },
	{ _T("Attack Rate"), 0, 0, 15, VOICE_GROUP_OPL2 | VOICE_GROUP_OPL3 | VOICE_GROUP_OPLL, &CVoiceEditDlg::SetAR4, &CVoiceEditDlg::GetAR4, },
	{ _T("Decay Rate"), 0, 0, 15, VOICE_GROUP_OPL2 | VOICE_GROUP_OPL3 | VOICE_GROUP_OPLL, &CVoiceEditDlg::SetDR4, &CVoiceEditDlg::GetDR4, },
	{ _T("Attack Rate"), 0, 0, 127, VOICE_GROUP_PSG, &CVoiceEditDlg::SetAR, &CVoiceEditDlg::GetAR, },
	{ _T("Decay Rate"), 0, 0, 127, VOICE_GROUP_PSG, &CVoiceEditDlg::SetDR, &CVoiceEditDlg::GetDR, },
	{ _T("Sustain Level"), 0, 0, 15, VOICE_GROUP_ALL & (~VOICE_GROUP_PSG), &CVoiceEditDlg::SetSL4, &CVoiceEditDlg::GetSL4, },
	{ _T("Sustain Level"), 0, 0, 127, VOICE_GROUP_PSG, &CVoiceEditDlg::SetSL, &CVoiceEditDlg::GetSL, },
	{ _T("Sustain Rate"), 0, 0, 31, VOICE_GROUP_OPM | VOICE_GROUP_OPNA, &CVoiceEditDlg::SetSR5, &CVoiceEditDlg::GetSR5, },
	{ _T("Sustain Rate"), 0, 0, 15, VOICE_GROUP_OPL2 | VOICE_GROUP_OPL3 | VOICE_GROUP_OPLL, &CVoiceEditDlg::SetSR4, &CVoiceEditDlg::GetSR4, },
	{ _T("Sustain Rate"), 0, 0, 127, VOICE_GROUP_PSG, &CVoiceEditDlg::SetSR, &CVoiceEditDlg::GetSR, },
	{ _T("Release Rate"), 0, 0, 15, VOICE_GROUP_ALL & (~VOICE_GROUP_PSG), &CVoiceEditDlg::SetRR4, &CVoiceEditDlg::GetRR4, },
	{ _T("Release Rate"), 0, 0, 127, VOICE_GROUP_PSG, &CVoiceEditDlg::SetRR, &CVoiceEditDlg::GetRR, },
	{ _T("Reverbation"), 0, 0, 15, VOICE_GROUP_ALL & (~VOICE_GROUP_PSG), &CVoiceEditDlg::SetRV4, &CVoiceEditDlg::GetRV4, },
	{ _T("Reverbation"), 0, 0, 127, VOICE_GROUP_PSG, &CVoiceEditDlg::SetRV, &CVoiceEditDlg::GetRV, },
	{ _T("Total Level"), 0, 0, 127, VOICE_GROUP_OPM | VOICE_GROUP_OPNA, &CVoiceEditDlg::SetTL, &CVoiceEditDlg::GetTL, },
	{ _T("Total Level"), 0, 0, 63, VOICE_GROUP_OPL2 | VOICE_GROUP_OPL3 | VOICE_GROUP_OPLL, &CVoiceEditDlg::SetTL6, &CVoiceEditDlg::GetTL6, },
	{ _T("EG-Bias"), 0, 0, 31, VOICE_GROUP_OPM, &CVoiceEditDlg::SetEGS5, &CVoiceEditDlg::GetEGS5, },
	{ _T("EG-Bias"), 0, 0, 127, VOICE_GROUP_PSG, &CVoiceEditDlg::SetEGS, &CVoiceEditDlg::GetEGS, },
	{ _T("KS-Level"), 0, 0, 3, VOICE_GROUP_OPL2 | VOICE_GROUP_OPL3 | VOICE_GROUP_OPLL, &CVoiceEditDlg::SetKSL, &CVoiceEditDlg::GetKSL, },
	{ _T("KS-Rate"), 0, 0, 3, VOICE_GROUP_OPM | VOICE_GROUP_OPNA | VOICE_GROUP_OPL2 | VOICE_GROUP_OPL3 | VOICE_GROUP_OPLL, &CVoiceEditDlg::SetKSR, &CVoiceEditDlg::GetKSR, },
	{ _T("SSG-EG"), 0, 0, 31, VOICE_GROUP_OPNA | VOICE_GROUP_PSG, &CVoiceEditDlg::SetEG, &CVoiceEditDlg::GetEG, },
	{ _T("AM Enable"), 0, 0, 1, VOICE_GROUP_OPM | VOICE_GROUP_OPNA | VOICE_GROUP_OPL2 | VOICE_GROUP_OPL3 | VOICE_GROUP_OPLL, &CVoiceEditDlg::SetAME, &CVoiceEditDlg::GetAME, },
	{ _T("VIB Enable"), 0, 0, 1, VOICE_GROUP_OPL2 | VOICE_GROUP_OPL3 | VOICE_GROUP_OPLL, &CVoiceEditDlg::SetVIB, &CVoiceEditDlg::GetVIB, },
	{ _T("Osc Fix"), 0, 0, 1, VOICE_GROUP_OPM, &CVoiceEditDlg::SetVIB, &CVoiceEditDlg::GetVIB, },
	{ _T("Wave Select"), 0, 0, 127, VOICE_GROUP_OPM | VOICE_GROUP_OPL2 | VOICE_GROUP_OPL3 | VOICE_GROUP_OPLL | VOICE_GROUP_PSG, &CVoiceEditDlg::SetWS, &CVoiceEditDlg::GetWS, },
	{ _T("Multiple"), 0, 0, 15, VOICE_GROUP_OPM | VOICE_GROUP_OPNA | VOICE_GROUP_OPL2 | VOICE_GROUP_OPL3 | VOICE_GROUP_OPLL, &CVoiceEditDlg::SetML, &CVoiceEditDlg::GetML, },
	{ _T("Detune1"), 0, 0, 7, VOICE_GROUP_OPM | VOICE_GROUP_OPNA, &CVoiceEditDlg::SetDT1, &CVoiceEditDlg::GetDT1, },
	{ _T("Detune2"), 0, 0, 15, VOICE_GROUP_OPM, &CVoiceEditDlg::SetDT2, &CVoiceEditDlg::GetDT2, },
	{ _T("Pseudo Detune"), 0, -8192, 8191, VOICE_GROUP_OPL3, &CVoiceEditDlg::SetPDT, &CVoiceEditDlg::GetPDT, },
	{ _T("Fixed Freq"), 0, 0, 32640, VOICE_GROUP_OPM, 0, &CVoiceEditDlg::GetFix, },
	{ _T("TL-LFO Wave"), 0, 0, 6, VOICE_GROUP_ALL, &CVoiceEditDlg::SetOPLFOWave, &CVoiceEditDlg::GetOPLFOWave, },
	{ _T("TL-LFO Depth"), 0, -64, 63, VOICE_GROUP_ALL, &CVoiceEditDlg::SetOPLFODepth, &CVoiceEditDlg::GetOPLFODepth, },
	{ _T("TL-LFO Freq"), 0, 0, 15, VOICE_GROUP_ALL, &CVoiceEditDlg::SetOPLFOFreq, &CVoiceEditDlg::GetOPLFOFreq, },
	{ _T("TL-LFO Delay"), 0, 0, 127, VOICE_GROUP_ALL, &CVoiceEditDlg::SetOPLFODelay, &CVoiceEditDlg::GetOPLFODelay, },
	{ _T("TL-LFO Rate"), 0, 0, 127, VOICE_GROUP_ALL, &CVoiceEditDlg::SetOPLFORate, &CVoiceEditDlg::GetOPLFORate, },
	{ 0, 0, 0, 0, VOICE_GROUP_NONE, 0, 0, },
};



// CVoiceEditDlg �_�C�A���O

IMPLEMENT_DYNAMIC(CVoiceEditDlg, CDialogEx)

CVoiceEditDlg::CVoiceEditDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CVoiceEditDlg::IDD, pParent)
	, theDevice(-1), theBank(-1), theProg(-1)
	, theConfig(((CFITOMApp*)AfxGetApp())->GetConfig())
	, bInit(FALSE), bModified(FALSE), pICh(0)
{
	lstctls[0] = &lstCommon;
	lstctls[1] = &lstOper1;
	lstctls[2] = &lstOper2;
	lstctls[3] = &lstOper3;
	lstctls[4] = &lstOper4;
}

CVoiceEditDlg::~CVoiceEditDlg()
{
	delete pICh;
}

void CVoiceEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_CPARAM, lstCommon);
	DDX_Control(pDX, IDC_LIST_OPPARAM1, lstOper1);
	DDX_Control(pDX, IDC_LIST_OPPARAM2, lstOper2);
	DDX_Control(pDX, IDC_LIST_OPPARAM3, lstOper3);
	DDX_Control(pDX, IDC_LIST_OPPARAM4, lstOper4);
	DDX_Control(pDX, IDC_EDIT_NAME, edtName);
	DDX_Control(pDX, IDC_EDIT_DEVICE, edtDevice);
	DDX_Control(pDX, IDC_EDIT_BANK, edtBank);
	DDX_Control(pDX, IDC_EDIT_PROG_NO, edtProg);
	DDX_Control(pDX, IDC_BTN_TEST, btnTest);
	DDX_Control(pDX, IDC_SPIN_VELO, spnVelocity);
	DDX_Control(pDX, IDC_SPIN_NOTE, spnNote);
	DDX_Control(pDX, IDC_EDIT_INPLACE, edtInplace);
	DDX_Control(pDX, IDC_SPIN_INPLACE, spnInplace);
}


BEGIN_MESSAGE_MAP(CVoiceEditDlg, CDialogEx)
	ON_MESSAGE(WM_KICKIDLE, &CVoiceEditDlg::OnKickIdle)
	ON_BN_CLICKED(IDC_BUTTON_PICK, &CVoiceEditDlg::OnBnClickedButtonPick)
	ON_EN_UPDATE(IDC_EDIT_NAME, &CVoiceEditDlg::OnUpdateEditName)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_CPARAM, &CVoiceEditDlg::OnDblclkListCparam)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_OPPARAM1, &CVoiceEditDlg::OnDblclkListOpparam1)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_OPPARAM2, &CVoiceEditDlg::OnDblclkListOpparam2)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_OPPARAM3, &CVoiceEditDlg::OnDblclkListOpparam3)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_OPPARAM4, &CVoiceEditDlg::OnDblclkListOpparam4)
	ON_EN_CHANGE(IDC_EDIT_INPLACE, &CVoiceEditDlg::OnEnChangeEditInplace)
	ON_EN_KILLFOCUS(IDC_EDIT_INPLACE, &CVoiceEditDlg::OnKillfocusEditInplace)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_INPLACE, &CVoiceEditDlg::OnDeltaposSpinInplace)
	ON_BN_CLICKED(IDCANCEL, &CVoiceEditDlg::OnBnClickedCancel)
	ON_EN_CHANGE(IDC_EDIT_NAME, &CVoiceEditDlg::OnChangeEditName)
	ON_COMMAND(IDC_VOICE_COPY, &CVoiceEditDlg::OnVoiceCopy)
	ON_COMMAND(IDC_VOICE_PASTE, &CVoiceEditDlg::OnVoicePaste)
	ON_UPDATE_COMMAND_UI(IDC_VOICE_PASTE, &CVoiceEditDlg::OnUpdateVoicePaste)
	ON_BN_CLICKED(IDC_VOICE_IMPORT, &CVoiceEditDlg::OnBnClickedVoiceText)
END_MESSAGE_MAP()


void CVoiceEditDlg::SetDevice(UINT32 dev)
{
	theDevice = dev;
	if (bInit && theDevice > 0) {
		CString tmp;
		UINT8 did = theDevice & 0xff;
		TCHAR devname[64];
		theConfig->GetDeviceName(theDevice, devname, _countof(devname));
		tmp.Format(_T("%02X:%s"), did, devname);
		edtDevice.SetWindowText(tmp);
	}
}

void CVoiceEditDlg::SetBank(UINT32 bank)
{
	theBank = bank;
	if (bInit && theDevice > 0) {
		CString tmp;
		tmp.Format(_T("%03i:%s"), theBank, theConfig->GetBankName(theDevice, theBank));
		edtBank.SetWindowText(tmp);
	}
}

void CVoiceEditDlg::SetProg(UINT32 prog)
{
	theProg = prog;

	UINT32 vg = CFITOM::GetDeviceVoiceGroupMask(theDevice);
	if (bInit && theBank >= 0 && theDevice > 0) {
		TCHAR tmp[64];
		theConfig->GetVoiceName(theDevice, theBank, theProg, tmp, _countof(tmp));
		CString str;
		str.Format(_T("%i"), theProg);
		edtProg.SetWindowText(str);
		edtName.SetWindowText(tmp);
	}
}

void CVoiceEditDlg::UpdateListCtrl(int op, BOOL bInit)
{
	if (op <= 4 && op >= 0) {
		int i = 0;
		if (bInit) {
			lstctls[op]->DeleteAllItems();
		}
		for (VoiceItem* items = (op == 0) ? commonItem : operatorItem; items->caption; items++) {
			if (items->mask & CFITOM::GetDeviceVoiceGroupMask(theDevice)) {
				if (bInit) {
					lstctls[op]->InsertItem(i, items->caption);
					lstctls[op]->SetItemData(i, DWORD_PTR(items));
				}
				lstctls[op]->SetItemData(i, DWORD_PTR(items));
				CString tmp;
				if (items->pGetter) {
					tmp.Format(_T("%i"), (this->*(items->pGetter))(op-1));
				}
				else {
					tmp = _T("-");
				}
				lstctls[op]->SetItemText(i, 1, tmp);
				i++;
			}
		}
	}
}

// CVoiceEditDlg ���b�Z�[�W �n���h���[

BOOL CVoiceEditDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO: �����ɏ�������ǉ����Ă�������
	for (int op = 0; op < 5; op++) {
		lstctls[op]->SetExtendedStyle(LVS_EX_FULLROWSELECT);
		lstctls[op]->InsertColumn(0, _T("Parameter"), LVCFMT_LEFT, 84);
		lstctls[op]->InsertColumn(1, _T("Value"), LVCFMT_RIGHT, 46);
	}

	UpdateVoiceView();
	spnNote.SetRange(0, 127);
	spnVelocity.SetRange(0, 127);
	spnNote.SetPos(60);
	spnVelocity.SetPos(127);

	pICh = new CInstCh(0, CFITOM::GetInstance());
	pICh->BankSelMSB(theDevice);
	pICh->BankSelLSB(theBank);
	pICh->SetVolume(100);
	pICh->SetExpress(127);
	pICh->SetPanpot(64);
	spnVelocity.SetRange32(0, 127);
	spnVelocity.SetPos32(127);
	spnNote.SetRange32(0, 127);
	spnNote.SetPos32(60);
	bModified = FALSE;
	return TRUE;  // return TRUE unless you set the focus to a control
	// ��O : OCX �v���p�e�B �y�[�W�͕K�� FALSE ��Ԃ��܂��B
}

void CVoiceEditDlg::UpdateVoiceView()
{
	CString tmp;
	int vg = CFITOM::GetDeviceVoiceGroupMask(theDevice);
	CFMBank* pbank = theConfig->GetFMBank(vg, theBank);
	if (pbank) {
		pbank->GetVoice(theProg, &theVoice);
		TCHAR devname[64];
		theConfig->GetDeviceName(theDevice, devname, _countof(devname));
		tmp.Format(_T("%02X:%s"), theDevice & 0xff, devname);
		edtDevice.SetWindowText(tmp);
		tmp.Format(_T("%03i:%s"), theBank, pbank->GetBankName());
		edtBank.SetWindowText(tmp);
		tmp.Format(_T("%3i"), theProg);
		edtProg.SetWindowText(tmp);
		char tmpname[17];
		memcpy(tmpname, theVoice.name, 16);
		tmpname[16] = 0;
		edtName.SetWindowText(CA2T(tmpname));
		for (int op = 0; op < 5; op++) {
			UpdateListCtrl(op, TRUE);
		}
	}
}

void CVoiceEditDlg::OnOK()
{
	// TODO: �����ɓ���ȃR�[�h��ǉ����邩�A�������͊��N���X���Ăяo���Ă��������B
	if (bModified) {
		int vg = CFITOM::GetDeviceVoiceGroupMask(theDevice);
		CFMBank* pbank = theConfig->GetFMBank(vg, theBank);
		if (pbank) {
			pbank->SetVoice(theProg, &theVoice);
			((CFITOMApp*)AfxGetApp())->SaveVoice(vg, theBank);
		}
		bModified = FALSE;
	}
}

void CVoiceEditDlg::OnBnClickedButtonPick()
{
	if (!bModified || ::AfxMessageBox(IDS_CONFIRM_VOICE_DISCARD, MB_YESNO) == IDYES) {
		CDlgVoicePicker dlg;
		dlg.SetDevice(theDevice);
		dlg.SetBank(theBank);
		dlg.SetProg(theProg);
		if (dlg.DoModal() == IDOK) {
			SetDevice(dlg.GetDevice());
			SetBank(dlg.GetBank());
			SetProg(dlg.Getprog());
			UpdateVoiceView();
			bModified = FALSE;
		}
	}
}


void CVoiceEditDlg::OnUpdateEditName()
{
	// TODO: �����ɃR���g���[���ʒm�n���h���[ �R�[�h��ǉ����Ă��������B
}

void CVoiceEditDlg::OnTestButtonDown()
{
	pICh->BankSelMSB(theDevice);
	pICh->SetVoiceData(&theVoice);
	pICh->NoteOn(spnNote.GetPos(), spnVelocity.GetPos());
}

void CVoiceEditDlg::OnTestButtonUp()
{
	pICh->NoteOff(spnNote.GetPos());
}

BOOL CVoiceEditDlg::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	// TODO: �����ɓ���ȃR�[�h��ǉ����邩�A�������͊��N���X���Ăяo���Ă��������B
	if (wParam == IDC_BTN_TEST) {
		NMHDR* pnms = (NMHDR*)lParam;
		switch (pnms->code) {
		case WM_LBUTTONDOWN:
			OnTestButtonDown();
			break;
		case WM_LBUTTONUP:
			OnTestButtonUp();
			break;
		}
	}
	return CDialogEx::OnNotify(wParam, lParam, pResult);
}


void CVoiceEditDlg::OnDblclkListCparam(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: �����ɃR���g���[���ʒm�n���h���[ �R�[�h��ǉ����܂��B
	*pResult = OnDblclkListParam(pNMItemActivate, 0);
}


void CVoiceEditDlg::OnDblclkListOpparam1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: �����ɃR���g���[���ʒm�n���h���[ �R�[�h��ǉ����܂��B
	*pResult = OnDblclkListParam(pNMItemActivate, 1);
}


void CVoiceEditDlg::OnDblclkListOpparam2(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: �����ɃR���g���[���ʒm�n���h���[ �R�[�h��ǉ����܂��B
	*pResult = OnDblclkListParam(pNMItemActivate, 2);
}


void CVoiceEditDlg::OnDblclkListOpparam3(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: �����ɃR���g���[���ʒm�n���h���[ �R�[�h��ǉ����܂��B
	*pResult = OnDblclkListParam(pNMItemActivate, 3);
}


void CVoiceEditDlg::OnDblclkListOpparam4(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: �����ɃR���g���[���ʒm�n���h���[ �R�[�h��ǉ����܂��B
	*pResult = OnDblclkListParam(pNMItemActivate, 4);
}

LRESULT CVoiceEditDlg::OnDblclkListParam(LPNMITEMACTIVATE pNMLV, int op)
{
	CListCtrl* plst = lstctls[op];
	if (pNMLV->iItem < 0) {
		return -1;
	}

	VoiceItem* pvi = (VoiceItem*)(plst->GetItemData(pNMLV->iItem));
	if (pvi->pSetter) {
		CRect ColumnRect;
		plst->GetSubItemRect(pNMLV->iItem, 1, LVIR_BOUNDS, ColumnRect);
		plst->ClientToScreen(ColumnRect);
		this->ScreenToClient(ColumnRect);
		editting_item = pvi;
		editting_op = op;
		edtInplace.SetWindowPos(plst, ColumnRect.left, ColumnRect.top, ColumnRect.Width(), ColumnRect.Height(), SWP_SHOWWINDOW | SWP_DRAWFRAME);
		edtInplace.ShowWindow(TRUE);
		edtInplace.SetWindowText(plst->GetItemText(pNMLV->iItem, 1));
		edtInplace.SetSel(0, -1);
		spnInplace.SetWindowPos(plst, ColumnRect.left, ColumnRect.top, 0, ColumnRect.Height(), SWP_SHOWWINDOW | SWP_DRAWFRAME);
		spnInplace.ShowWindow(TRUE);
		spnInplace.SetRange32(pvi->min, pvi->max);
		spnInplace.SetPos32((this->*(pvi->pGetter))(op - 1));
		//	spnInplace.SetBuddy(&edtInplace);

		edtInplace.SetFocus();
	}
	return 0;
}


void CVoiceEditDlg::OnEnChangeEditInplace()
{
	// TODO: �����ɃR���g���[���ʒm�n���h���[ �R�[�h��ǉ����Ă��������B
	if (editting_item) {
		bModified = TRUE;
		/*
		edtInplace.SetSel(0, -1);
		int val = spnInplace.GetPos32();
		if (editting_item->min <= val && editting_item->max >= val && editting_item->pSetter) {
			(this->*(editting_item->pSetter))(editting_op - 1, val);
			theFitom->SetVoice(&theVoice, theDevice, theBank, theProg);
		}
		*/
	}
}


void CVoiceEditDlg::OnKillfocusEditInplace()
{
	// TODO: �����ɃR���g���[���ʒm�n���h���[ �R�[�h��ǉ����܂��B
	if (editting_item) {
		TCHAR tmp1[40], tmp2[40], tmp3[40];
		edtInplace.GetWindowText(tmp1, _countof(tmp1));
		lstctls[editting_op]->GetItemText(lstctls[editting_op]->GetSelectionMark(), 1, tmp3, _countof(tmp3));
		if (lstrcmp(tmp1, tmp3) != 0) {
			bModified = TRUE;
			int val = ::_tcstol(tmp1, 0, 10);
			StringCchPrintf(tmp2, _countof(tmp2), _T("%i"), val);
			if (lstrcmp(tmp1, tmp2) == 0) { // is valid
				if (editting_item->min <= val && editting_item->max >= val && editting_item->pSetter) {
					spnInplace.SetPos32(val);
					(this->*(editting_item->pSetter))(editting_op - 1, val);
					pICh->SetVoiceData(&theVoice);
					UpdateListCtrl(editting_op, FALSE);
				}
			}
		}
	}
	edtInplace.ShowWindow(FALSE);
	spnInplace.ShowWindow(FALSE);
	editting_item = 0;
	editting_op = -1;
}

void CVoiceEditDlg::OnDeltaposSpinInplace(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: �����ɃR���g���[���ʒm�n���h���[ �R�[�h��ǉ����܂��B
	if (editting_item != 0) {
		edtInplace.SetSel(0, -1);
		if (editting_item->pSetter) {
			int val = spnInplace.GetPos32();
			if (editting_item->min <= val && editting_item->max >= val && editting_item->pSetter) {
				(this->*(editting_item->pSetter))(editting_op - 1, val);
				pICh->SetVoiceData(&theVoice);
			}
		}
		bModified = TRUE;
	}
	else{
		int nothing = 0;
	}
	*pResult = 0;
}


void CVoiceEditDlg::OnClickedButtonBank()
{
	// TODO: �����ɃR���g���[���ʒm�n���h���[ �R�[�h��ǉ����܂��B
}


BOOL CVoiceEditDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: �����ɓ���ȃR�[�h��ǉ����邩�A�������͊��N���X���Ăяo���Ă��������B
	if (WM_KEYDOWN == pMsg->message)
	{
		switch (pMsg->wParam)
		{
		case VK_RETURN:
			if (edtInplace.IsWindowVisible()) {
				OnKillfocusEditInplace();
			}
			return FALSE;
		case VK_ESCAPE:
			return FALSE;
		default:
			break;
		}
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}


void CVoiceEditDlg::OnBnClickedCancel()
{
	// TODO: �����ɃR���g���[���ʒm�n���h���[ �R�[�h��ǉ����܂��B
	if (!bModified || ::AfxMessageBox(IDS_CONFIRM_VOICE_DISCARD, MB_YESNO) == IDYES) {
		CDialogEx::OnCancel();
	}
}

void CVoiceEditDlg::OnChangeEditName()
{
	// TODO: �����ɃR���g���[���ʒm�n���h���[ �R�[�h��ǉ����Ă��������B
	TCHAR tmp[17];
	edtName.GetWindowText(tmp, 17);
	tmp[16] = _T(0);
	memcpy(theVoice.name, CT2A(tmp), 16);
	bModified = TRUE;
}

void CVoiceEditDlg::OnVoiceCopy()
{
	// TODO: �����ɃR�}���h �n���h���[ �R�[�h��ǉ����܂��B
	((CFITOMApp*)AfxGetApp())->VoiceCopy(&theVoice);
}

void CVoiceEditDlg::OnVoicePaste()
{
	// TODO: �����ɃR�}���h �n���h���[ �R�[�h��ǉ����܂��B
	CString str;
	TCHAR name[17];
	FMVOICE clip;
	if (((CFITOMApp*)AfxGetApp())->GetVoiceFromClipBoard(&clip)) {
		memcpy(name, clip.name, 16);
		name[16] = 0;
		str.Format(IDS_CONFIRM_VOICE_PASTE, theVoice.ID, LPCTSTR(name));
		if (AfxMessageBox(str, MB_YESNO) == IDYES) {
			DWORD vid = theVoice.ID;
			theVoice = clip;
			theVoice.ID = vid;
			for (int op = 0; op < 5; op++) {
				UpdateListCtrl(op, TRUE);
			}
			edtName.SetWindowText(name);
			bModified = TRUE;
		}
	}
}

void CVoiceEditDlg::OnUpdateVoicePaste(CCmdUI *pCmdUI)
{
	// TODO:�����ɃR�}���h�X�V UI �n���h���[ �R�[�h��ǉ����܂��B
	pCmdUI->Enable(((CFITOMApp*)AfxGetApp())->IsVoiceInClipBoard());
}

LRESULT CVoiceEditDlg::OnKickIdle(WPARAM wParam, LPARAM lParam)
{
	UpdateDialogControls(this, FALSE);

	return FALSE;
}


void CVoiceEditDlg::OnBnClickedVoiceText()
{
	// TODO: �����ɃR���g���[���ʒm�n���h���[ �R�[�h��ǉ����܂��B
	CTextVoiceEditDlg dlg(this);
	dlg.SetVoice(&theVoice);
	dlg.SetDevice(theDevice);
	if (dlg.DoModal() == IDOK) {
		dlg.GetVoice(&theVoice);
	}
}