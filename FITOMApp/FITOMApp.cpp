
// FITOMApp.cpp : �A�v���P�[�V�����̃N���X������`���܂��B
//

#include "stdafx.h"

#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/filesystem.hpp>

#include "FITOM.h"
#include "SoundDev.h"
#include "FITOMApp.h"
#include "FITOMAppDlg.h"
#include "FITOMCfg.h"
#include "SCCIWrapper.h"
#include "MIDIDEV.h"
#include "MIDI.h"
#include "codec.h"
#include "ADPCM.h"
#include "SplashDlg.h"
#include "ProgressDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CFITOMApp

BEGIN_MESSAGE_MAP(CFITOMApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()

CFITOMApp::ERROR_MAP CFITOMApp::errmap[] = {
	{ ERROR_MIDI_IN, IDS_ERROR_MIDI, },
	{ ERROR_FM_DEV, IDS_ERROR_DEV, },
	{ ERROR_FM_BANK, IDS_ERROR_VOICE, },
	{ ERROR_DRUM_MAP, IDS_ERROR_DRUM, },
	{ ERROR_SCCI, IDS_ERROR_SCCI, },
	{ -98, IDS_ERROR_PROFILE, },
	{ -99, IDS_ERROR_INI, },
	{ 0, 0, },
};

// CFITOMApp �R���X�g���N�V����

CFITOMApp::CFITOMApp() : hTimer(0), hPolling(0), theFitom(0), bRunning(FALSE), clipvoice(0), clipdrum(0)
{
	// �ċN���}�l�[�W���[���T�|�[�g���܂�
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: ���̈ʒu�ɍ\�z�p�R�[�h��ǉ����Ă��������B
	// ������ InitInstance ���̏d�v�ȏ��������������ׂċL�q���Ă��������B
}


// �B��� CFITOMApp �I�u�W�F�N�g�ł��B

CFITOMApp theApp;

void SplashStatus(LPCTSTR str)
{
	theApp.SplashStatus(str);
}

void SplashMessage(LPCTSTR str)
{
	theApp.SplashMessage(str);
}

// CFITOMApp ������

BOOL CFITOMApp::InitInstance()
{
	// �A�v���P�[�V���� �}�j�t�F�X�g�� visual �X�^�C����L���ɂ��邽�߂ɁA
	// ComCtl32.dll Version 6 �ȍ~�̎g�p���w�肷��ꍇ�́A
	// Windows XP �� InitCommonControlsEx() ���K�v�ł��B�����Ȃ���΁A�E�B���h�E�쐬�͂��ׂĎ��s���܂��B
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// �A�v���P�[�V�����Ŏg�p���邷�ׂẴR���� �R���g���[�� �N���X���܂߂�ɂ́A
	// �����ݒ肵�܂��B
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();
	pSplash = new CSplashDlg();
	pSplash->ShowWindow(SW_SHOW);

	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	// �_�C�A���O�ɃV�F�� �c���[ �r���[�܂��̓V�F�� ���X�g �r���[ �R���g���[����
	// �܂܂�Ă���ꍇ�ɃV�F�� �}�l�[�W���[���쐬���܂��B
	CShellManager *pShellManager = new CShellManager;

	// MFC �R���g���[���Ńe�[�}��L���ɂ��邽�߂ɁA"Windows �l�C�e�B�u" �̃r�W���A�� �}�l�[�W���[���A�N�e�B�u��
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// �W��������
	// �����̋@�\���g�킸�ɍŏI�I�Ȏ��s�\�t�@�C����
	// �T�C�Y���k���������ꍇ�́A�ȉ�����s�v�ȏ�����
	// ���[�`�����폜���Ă��������B
	// �ݒ肪�i�[����Ă��郌�W�X�g�� �L�[��ύX���܂��B
	// TODO: ��Ж��܂��͑g�D���Ȃǂ̓K�؂ȕ������
	// ���̕������ύX���Ă��������B
	SetRegistryKey(_T("FITOM"));
#ifdef _DEBUG
	::AllocConsole();
	::freopen("CONOUT$", "w", stdout);
	::freopen("CONOUT$", "w", stderr);
#endif
	int res = FITOMStart();
	pSplash->ShowWindow(SW_HIDE);
	pSplash->EndDialog(0);
	if (!res) {
		CFITOMAppDlg dlg;
		m_pMainWnd = &dlg;
		INT_PTR nResponse = dlg.DoModal();
		if (nResponse == IDOK)
		{
			// TODO: �_�C�A���O�� <OK> �ŏ����ꂽ���̃R�[�h��
			//  �L�q���Ă��������B
		}
		else if (nResponse == IDCANCEL)
		{
			// TODO: �_�C�A���O�� <�L�����Z��> �ŏ����ꂽ���̃R�[�h��
			//  �L�q���Ă��������B
		}
		else if (nResponse == -1)
		{
			TRACE(traceAppMsg, 0, "�x��: �_�C�A���O�̍쐬�Ɏ��s���܂����B�A�v���P�[�V�����͗\�������ɏI�����܂��B\n");
			TRACE(traceAppMsg, 0, "�x��: �_�C�A���O�� MFC �R���g���[�����g�p���Ă���ꍇ�A#define _AFX_NO_MFC_CONTROLS_IN_DIALOGS ���w��ł��܂���B\n");
		}
		FITOMStop();
	}
	else {
		int ids = IDS_ERROR_OTHER;
		for (int i = 0; errmap[i].FITOM_ERR != 0; i++) {
			if (errmap[i].FITOM_ERR == res) { 
				ids = errmap[i].ERR_IDS;
				break;
			}
		}
		AfxMessageBox(ids);
	}

#ifdef _DEBUG
	::FreeConsole();
#endif
	// ��ō쐬���ꂽ�V�F�� �}�l�[�W���[���폜���܂��B
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	// �_�C�A���O�͕����܂����B�A�v���P�[�V�����̃��b�Z�[�W �|���v���J�n���Ȃ���
	//  �A�v���P�[�V�������I�����邽�߂� FALSE ��Ԃ��Ă��������B
	return FALSE;
}

void CFITOMApp::DoEvents()
{
	MSG msg;
	while (::PeekMessage(&msg, NULL, NULL, NULL, PM_NOREMOVE))
	{
		if (!this->PumpMessage()) {
			return;
		}
	}
}

void CFITOMApp::SplashStatus(LPCTSTR str)
{
	DoEvents();
	pSplash->SetStatus(str);
}

void CFITOMApp::SplashMessage(LPCTSTR str)
{
	DoEvents();
	pSplash->SetFilename(str);
}

int CFITOMApp::FITOMStart()
{
	int res = 0;
	if (!PathFileExists(_T(".\\FITOM.ini"))) { return -99; }
	theConfig = LoadConfig();

	if (theConfig) {
		if (theConfig->GetScci() == 0) { return ERROR_SCCI; }
		if (theConfig->GetLogDevs() == 0) { return ERROR_FM_DEV; }
		if (theConfig->GetMidiInputs() == 0) { return ERROR_MIDI_IN; }
		theFitom = CFITOM::GetInstance();
		int res = theFitom->InitInstance(theConfig);
		if (res) { return res; }
		::timeBeginPeriod(1);
		hTimer = timeSetEvent(10 - 1, 0, TimerProc, (DWORD)this, TIME_PERIODIC | TIME_CALLBACK_FUNCTION);
		//hPolling = (HANDLE)_beginthreadex(0, 0, PollingProc, (void*)this, 0, 0);
		bRunning = TRUE;
	}
	return res;
}

int CFITOMApp::FITOMStop()
{
	bRunning = FALSE;
	timeKillEvent(hTimer);
	timeEndPeriod(1);
	::Sleep(100);
	//::TerminateThread(hPolling, 0);
	::Sleep(100);
	theFitom->ExitInstance();
	delete theConfig;
	return 0;
}

void CALLBACK CFITOMApp::TimerProc(UINT uTimerID, UINT uMsg, DWORD dwUser, DWORD dummy1, DWORD dummy2)
{
	CFITOMApp* theFitomApp = (CFITOMApp*)dwUser;
	if (theFitomApp->bRunning && theFitomApp->theFitom) {
		theFitomApp->theFitom->TimerCallBack(theFitomApp->lltickcount);
		theFitomApp->lltickcount++;
	}
}

unsigned int CFITOMApp::PollingProc(void* params)
{
	CFITOMApp* theFitomApp = (CFITOMApp*)params;
	while (theFitomApp->bRunning && theFitomApp->theFitom) {
		theFitomApp->theFitom->PollingCallBack();
		Sleep(0);
	}
	return 0;
}

void CFITOMApp::VoiceCopy(FMVOICE* voice)
{
	if (!clipvoice) {
		clipvoice = new FMVOICE();
	}
	*clipvoice = *voice;
}

BOOL CFITOMApp::IsVoiceInClipBoard()
{
	return (clipvoice != 0);
}

BOOL CFITOMApp::GetVoiceFromClipBoard(FMVOICE* voice)
{
	if (clipvoice) {
		*voice = *clipvoice;
		delete clipvoice;
		clipvoice = 0;
		return TRUE;
	}
	return FALSE;
}

void CFITOMApp::DrumCopy(DRUMMAP* drumnote)
{
	if (!clipdrum) {
		clipdrum = new DRUMMAP();
	}
	*clipdrum = *drumnote;
}

BOOL CFITOMApp::IsDrumInClipBoard()
{
	return (clipdrum != 0);
}

BOOL CFITOMApp::GetDrumFromClipBoard(DRUMMAP* Drum)
{
	if (clipdrum) {
		*Drum = *clipdrum;
		delete clipdrum;
		clipdrum = 0;
		return TRUE;
	}
	return FALSE;
}

CFITOMConfigWin32* CFITOMApp::LoadConfig()
{
	CFITOMConfigWin32* ret = new CFITOMConfigWin32(_T(".\\FITOM.ini"));
	ret->SetProgressMsgFunc(::SplashStatus);
	ret->SetProgressFileFunc(::SplashMessage);
	ret->LoadConfig();

#ifdef _DEBUG
	fprintf_s(stderr, _T("[Logical Devices]\n"));
	for (int i = 0; i < ret->GetLogDevs(); i++) {
		CSoundDevice* pdev = ret->GetLogDeviceFromIndex(i);
		TCHAR portdesc[80];
		TCHAR devdesc[80];
		pdev->GetDevPort()->GetDesc(portdesc, _countof(portdesc));
		pdev->GetDescriptor(devdesc, _countof(devdesc));
		fprintf_s(stderr, _T("%02i %02X:%s (%s)\n"), i, pdev->GetDevice(), devdesc, portdesc);
	}
	fprintf_s(stderr, _T("[Physical Devices]\n"));
	for (int i = 0; i < ret->GetPhyDevs(); i++) {
		CSoundDevice* pdev = ret->GetPhysDeviceFromIndex(i);
		TCHAR portdesc[80];
		TCHAR devdesc[80];
		pdev->GetDevPort()->GetDesc(portdesc, _countof(portdesc));
		pdev->GetDescriptor(devdesc, _countof(devdesc));
		fprintf_s(stderr, _T("%02i %02X:%s (%s)\n"), i, pdev->GetDevice(), devdesc, portdesc);
	}
#endif
	return ret;
}


void CFITOMApp::SaveVoice(int group, int bank) {

	CFMBank* pbank = 0;
	CDrumBank* dbank = 0;
	if (group == VOICE_GROUP_RHYTHM) {
		dbank = theConfig->GetDrumBank(bank);
	}
	else {
		pbank = theConfig->GetFMBank(group, bank);
	}
	if (pbank || dbank) {
		CProgressDlg* pDlg = new CProgressDlg(m_pMainWnd);
		pDlg->ShowWindow(SW_SHOW);
		pDlg->SetRange(0, 129);
		LPCTSTR filename = pbank ? pbank->GetFileName() : (dbank ? dbank->GetFileName() : _T(""));
		pDlg->SetStatus(filename);
		pDlg->SetTarget(_T("Header"));
		pDlg->SetPos(0);
		switch (group) {
		case VOICE_GROUP_OPM:
			SaveOPMVoice(pbank, pDlg);
			break;
		case VOICE_GROUP_OPNA:
			SaveOPNVoice(pbank, pDlg);
			break;
		case VOICE_GROUP_OPL2:
			SaveOPL2Voice(pbank, pDlg);
			break;
		case VOICE_GROUP_OPL3:
			SaveOPL3Voice(pbank, pDlg);
			break;
		case VOICE_GROUP_OPLL:
			SaveOPLLVoice(pbank, pDlg);
			break;
		case VOICE_GROUP_PSG:
			SavePSGVoice(pbank, pDlg);
			break;
		case VOICE_GROUP_RHYTHM:
			SaveDrumMap(dbank, pDlg);
			break;
		}
		pDlg->EndDialog(0);
	}
}

void CFITOMApp::WriteLFOParam(FMVOICE* voice, LPCTSTR secname, LPCTSTR filename)
{
	TCHAR param[80];
	theConfig->BuildLFOParam(voice, 0, param, _countof(param));
	::WritePrivateProfileString(secname, _T("LFO0"), param, filename);
	for (int k = 0; k < 4; k++) {
		TCHAR keyname[16];
		StringCchPrintf(keyname, _countof(keyname), _T("LFO%i"), k + 1);
		theConfig->BuildLFOParam(voice, k + 1, param, _countof(param));
		::WritePrivateProfileString(secname, keyname, param, filename);
	}
}

void CFITOMApp::SaveOPMVoice(CFMBank* bank, CProgressDlg* pDlg) {
	LPCTSTR filename = bank->GetFileName();
	if (::PathFileExists(filename)) {
		TCHAR secname[16];
		::WritePrivateProfileSection(_T("Header"), NULL, filename);
		::WritePrivateProfileSection(_T("Bank"), NULL, filename);
		::WritePrivateProfileString(_T("Header"), _T("Type"), _T("OPM"), filename);
		::WritePrivateProfileString(_T("Header"), _T("BankName"), bank->GetBankName(), filename);
		pDlg ? pDlg->SetPos(1) : 0;
		for (int i = 0; i < 128; i++) {
			TCHAR param[80];

			FMVOICE voice;
			bank->GetVoice(i, &voice);
			char name[17];
			memcpy(name, voice.name, 16);
			name[16] = 0;
			pDlg ? pDlg->SetTarget(CA2T(name)) : 0;
			StringCchPrintf(secname, _countof(secname), _T("Prog%i"), i);
			::WritePrivateProfileSection(secname, NULL, filename);
			WriteLFOParam(&voice, secname, filename);
			::WritePrivateProfileString(secname, _T("Name"), CA2T(name), filename);
			theConfig->BuildOPMVoice(&voice, 0, param, _countof(param));
			::WritePrivateProfileString(secname, _T("ALFB"), param, filename);
			for (int k = 0; k < 4; k++) {
				TCHAR keyname[16];
				StringCchPrintf(keyname, _countof(keyname), _T("OP%i"), k + 1);
				theConfig->BuildOPMVoice(&voice, k+1, param, _countof(param));
				::WritePrivateProfileString(secname, keyname, param, filename);
			}
			pDlg ? pDlg->SetPos(i + 2) : 0;
		}
	}
}

void CFITOMApp::SaveOPNVoice(CFMBank* bank, CProgressDlg* pDlg) {
	LPCTSTR filename = bank->GetFileName();
	if (::PathFileExists(filename)) {
		TCHAR secname[16];
		::WritePrivateProfileSection(_T("Header"), NULL, filename);
		::WritePrivateProfileSection(_T("Bank"), NULL, filename);
		::WritePrivateProfileString(_T("Header"), _T("Type"), _T("OPN"), filename);
		::WritePrivateProfileString(_T("Header"), _T("BankName"), bank->GetBankName(), filename);
		pDlg ? pDlg->SetPos(1) : 0;
		for (int i = 0; i < 128; i++) {
			TCHAR param[80];

			FMVOICE voice;
			bank->GetVoice(i, &voice);
			char name[17];
			memcpy(name, voice.name, 16);
			name[16] = 0;

			StringCchPrintf(secname, _countof(secname), _T("Prog%i"), i);
			::WritePrivateProfileSection(secname, NULL, filename);
			WriteLFOParam(&voice, secname, filename);
			::WritePrivateProfileString(secname, _T("Name"), CA2T(name), filename);
			theConfig->BuildOPNVoice(&voice, 0, param, _countof(param));
			::WritePrivateProfileString(secname, _T("ALFB"), param, filename);
			for (int k = 0; k < 4; k++) {
				TCHAR keyname[16];
				StringCchPrintf(keyname, _countof(keyname), _T("OP%i"), k + 1);
				theConfig->BuildOPNVoice(&voice, k+1, param, _countof(param));
				::WritePrivateProfileString(secname, keyname, param, filename);
			}
			pDlg ? pDlg->SetPos(i + 2) : 0;
		}
	}
}

void CFITOMApp::SaveOPL3Voice(CFMBank* bank, CProgressDlg* pDlg) {
	LPCTSTR filename = bank->GetFileName();
	if (::PathFileExists(filename)) {
		TCHAR secname[16];
		::WritePrivateProfileSection(_T("Header"), NULL, filename);
		::WritePrivateProfileSection(_T("Bank"), NULL, filename);
		::WritePrivateProfileString(_T("Header"), _T("Type"), _T("OPL3"), filename);
		::WritePrivateProfileString(_T("Header"), _T("BankName"), bank->GetBankName(), filename);
		pDlg ? pDlg->SetPos(1) : 0;
		for (int i = 0; i < 128; i++) {
			TCHAR param[80];

			FMVOICE voice;
			bank->GetVoice(i, &voice);
			char name[17];
			memcpy(name, voice.name, 16);
			name[16] = 0;
			pDlg ? pDlg->SetTarget(CA2T(name)) : 0;

			StringCchPrintf(secname, _countof(secname), _T("Prog%i"), i);
			::WritePrivateProfileSection(secname, NULL, filename);
			WriteLFOParam(&voice, secname, filename);
			::WritePrivateProfileString(secname, _T("Name"), CA2T(name), filename);
			theConfig->BuildOPL3Voice(&voice, 0, param, _countof(param));
			::WritePrivateProfileString(secname, _T("ALFB"), param, filename);
			for (int k = 0; k < 4; k++) {
				TCHAR keyname[16];
				StringCchPrintf(keyname, _countof(keyname), _T("OP%i"), k + 1);
				theConfig->BuildOPL3Voice(&voice, k + 1, param, _countof(param));
				::WritePrivateProfileString(secname, keyname, param, filename);
			}
			pDlg ? pDlg->SetPos(i + 2) : 0;
		}
	}
}

void CFITOMApp::SaveOPL2Voice(CFMBank* bank, CProgressDlg* pDlg) {
	LPCTSTR filename = bank->GetFileName();
	if (::PathFileExists(filename)) {
		TCHAR secname[16];
		::WritePrivateProfileSection(_T("Header"), NULL, filename);
		::WritePrivateProfileSection(_T("Bank"), NULL, filename);
		::WritePrivateProfileString(_T("Header"), _T("Type"), _T("OPL2"), filename);
		::WritePrivateProfileString(_T("Header"), _T("BankName"), bank->GetBankName(), filename);
		pDlg ? pDlg->SetPos(1) : 0;
		for (int i = 0; i < 128; i++) {
			TCHAR param[80];

			FMVOICE voice;
			bank->GetVoice(i, &voice);
			char name[17];
			memcpy(name, voice.name, 16);
			name[16] = 0;
			pDlg ? pDlg->SetTarget(CA2T(name)) : 0;

			StringCchPrintf(secname, _countof(secname), _T("Prog%i"), i);
			::WritePrivateProfileSection(secname, NULL, filename);
			WriteLFOParam(&voice, secname, filename);
			::WritePrivateProfileString(secname, _T("Name"), CA2T(name), filename);
			theConfig->BuildOPL2Voice(&voice, 0, param, _countof(param));
			::WritePrivateProfileString(secname, _T("ALFB"), param, filename);
			for (int k = 0; k < 2; k++) {
				TCHAR keyname[16];
				StringCchPrintf(keyname, _countof(keyname), _T("OP%i"), k + 1);
				theConfig->BuildOPL2Voice(&voice, k + 1, param, _countof(param));
				::WritePrivateProfileString(secname, keyname, param, filename);
			}
			pDlg ? pDlg->SetPos(i + 2) : 0;
		}
	}
}

void CFITOMApp::SaveOPLLVoice(CFMBank* bank, CProgressDlg* pDlg) {
	LPCTSTR filename = bank->GetFileName();
	if (::PathFileExists(filename)) {
		TCHAR secname[16];
		::WritePrivateProfileSection(_T("Header"), NULL, filename);
		::WritePrivateProfileSection(_T("Bank"), NULL, filename);
		::WritePrivateProfileString(_T("Header"), _T("Type"), _T("OPLL"), filename);
		::WritePrivateProfileString(_T("Header"), _T("BankName"), bank->GetBankName(), filename);
		pDlg ? pDlg->SetPos(1) : 0;
		for (int i = 0; i < 128; i++) {
			TCHAR param[80];

			FMVOICE voice;
			bank->GetVoice(i, &voice);
			char name[17];
			memcpy(name, voice.name, 16);
			name[16] = 0;
			pDlg ? pDlg->SetTarget(CA2T(name)) : 0;

			StringCchPrintf(secname, _countof(secname), _T("Prog%i"), i);
			::WritePrivateProfileSection(secname, NULL, filename);
			WriteLFOParam(&voice, secname, filename);
			::WritePrivateProfileString(secname, _T("Name"), CA2T(name), filename);
			theConfig->BuildOPLLVoice(&voice, 0, param, _countof(param));
			::WritePrivateProfileString(secname, _T("ALFB"), param, filename);
			for (int k = 0; k < 2 && !(voice.AL & 0x40); k++) {
				TCHAR keyname[16];
				StringCchPrintf(keyname, _countof(keyname), _T("OP%i"), k + 1);
				theConfig->BuildOPLLVoice(&voice, k + 1, param, _countof(param));
				::WritePrivateProfileString(secname, keyname, param, filename);
			}
			pDlg ? pDlg->SetPos(i + 2) : 0;
		}
	}
}

void CFITOMApp::SavePSGVoice(CFMBank* bank, CProgressDlg* pDlg) {
	LPCTSTR filename = bank->GetFileName();
	if (::PathFileExists(filename)) {
		TCHAR secname[16];
		::WritePrivateProfileSection(_T("Header"), NULL, filename);
		::WritePrivateProfileSection(_T("Bank"), NULL, filename);
		::WritePrivateProfileString(_T("Header"), _T("Type"), _T("PSG"), filename);
		::WritePrivateProfileString(_T("Header"), _T("BankName"), bank->GetBankName(), filename);
		pDlg ? pDlg->SetPos(1) : 0;
		for (int i = 0; i < 128; i++) {
			TCHAR param[80];

			FMVOICE voice;
			bank->GetVoice(i, &voice);
			char name[17];
			memcpy(name, voice.name, 16);
			name[16] = 0;
			pDlg ? pDlg->SetTarget(CA2T(name)) : 0;

			StringCchPrintf(secname, _countof(secname), _T("Prog%i"), i);
			::WritePrivateProfileSection(secname, NULL, filename);
			WriteLFOParam(&voice, secname, filename);
			::WritePrivateProfileString(secname, _T("Name"), CA2T(name), filename);
			theConfig->BuildPSGVoice(&voice, 0, param, _countof(param));
			::WritePrivateProfileString(secname, _T("ALFB"), param, filename);
			for (int k = 0; k < 1; k++) {
				TCHAR keyname[16];
				StringCchPrintf(keyname, _countof(keyname), _T("OP%i"), k + 1);
				theConfig->BuildPSGVoice(&voice, k + 1, param, _countof(param));
				::WritePrivateProfileString(secname, keyname, param, filename);
			}
			pDlg ? pDlg->SetPos(i + 2) : 0;
		}
	}
}

void CFITOMApp::SaveDrumMap(CDrumBank* bank, CProgressDlg* pDlg)
{
	LPCTSTR filename = bank->GetFileName();
	if (::PathFileExists(filename)) {
		::WritePrivateProfileSection(_T("Header"), NULL, filename);
		::WritePrivateProfileSection(_T("Bank"), NULL, filename);
		::WritePrivateProfileString(_T("Header"), _T("Type"), _T("RHYTHM"), filename);
		::WritePrivateProfileString(_T("Header"), _T("BankName"), bank->GetBankName(), filename);
		pDlg ? pDlg->SetPos(1) : 0;
		for (int i = 0; i < 128; i++) {
			TCHAR param[128];
			DRUMMAP drumnote;
			bank->GetVoice(i, &drumnote);
			pDlg ? pDlg->SetTarget(drumnote.name) : 0;
			if (drumnote.device != 0){
				TCHAR note[20];
				if (drumnote.num & 0x80) {
					if (drumnote.fnum) {
						StringCchPrintf(note, _countof(note), _T("#%i:%04X"), drumnote.num & 0x7f, drumnote.fnum);
					}
					else {
						StringCchPrintf(note, _countof(note), _T("#%i"), drumnote.num & 0x7f);
					}
				}
				else {
					if (drumnote.fnum) {
						StringCchPrintf(note, _countof(note), _T("%i"), drumnote.num);
					}
					else {
						StringCchPrintf(note, _countof(note), _T("%i:%i"), drumnote.num, drumnote.fnum);
					}
				}
				TCHAR devparam[80];
				UINT32 phyid;
				phyid = drumnote.device->GetDevPort()->GetPhysicalId();
				if (drumnote.devID & 0x80000000) {	//logical device
					StringCchPrintf(devparam, _countof(devparam), _T("%s"), CFITOM::GetDeviceNameFromID(drumnote.devID & 0xff));
				}
				else if (theConfig->isPcmDevice(drumnote.devID)) {
					StringCchPrintf(devparam, _countof(devparam), _T("%s:%i"), CFITOM::GetDeviceNameFromID(drumnote.devID & 0xff),
						theConfig->GetPcmDeviceIndex((CAdPcmBase*)drumnote.device));
				} else
				{ //physical device
					StringCchPrintf(devparam, _countof(devparam), _T("%s:%i:%i"), CFITOM::GetDeviceNameFromID(drumnote.devID & 0xff),
						(phyid >> 16) & 0xff, (phyid >> 8) & 0xff);
				}
				StringCchPrintf(param, _countof(param), _T("%s,%s,%i,%i,%s,%i,%i"),
					drumnote.name, devparam, drumnote.bank, drumnote.prog, note, drumnote.pan, drumnote.gate);
				TCHAR keyname[20];
				StringCchPrintf(keyname, _countof(keyname), _T("Note%i"), i);
				::WritePrivateProfileString(_T("Bank"), keyname, param, filename);
			}
			pDlg ? pDlg->SetPos(i + 2) : 0;
		}
	}
}

BOOL CFITOMApp::OnIdle(LONG lCount)
{
	// TODO: �����ɓ���ȃR�[�h��ǉ����邩�A�������͊��N���X���Ăяo���Ă��������B
	//if (theFitom && bRunning) {
	//	theFitom->PollingCallBack();
	//}
	return CWinApp::OnIdle(lCount);
}