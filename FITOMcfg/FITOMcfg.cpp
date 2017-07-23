
// FITOMcfg.cpp : �A�v���P�[�V�����̃N���X������`���܂��B
//

#include "stdafx.h"
#include "FITOMcfg.h"
#include "FITOMcfgDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CFITOMcfgApp

BEGIN_MESSAGE_MAP(CFITOMcfgApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CFITOMcfgApp �R���X�g���N�V����

CFITOMcfgApp::CFITOMcfgApp()
{
	// �ċN���}�l�[�W���[���T�|�[�g���܂�
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: ���̈ʒu�ɍ\�z�p�R�[�h��ǉ����Ă��������B
	// ������ InitInstance ���̏d�v�ȏ��������������ׂċL�q���Ă��������B
}


// �B��� CFITOMcfgApp �I�u�W�F�N�g�ł��B

CFITOMcfgApp theApp;

extern chipSymbols symbols[] = {
	{ SC_TYPE_NONE, _T("NONE"), _T("Nothing"), },
	{ SC_TYPE_YM2608, _T("OPNA"), _T("YM2608"), },
	{ SC_TYPE_YM2151, _T("OPM"), _T("YM2151"), },
	{ SC_TYPE_YM2610, _T("OPNB"), _T("YM2610"), },
	{ SC_TYPE_YM2203, _T("OPN"), _T("YM2203"), },
	{ SC_TYPE_YM2612, _T("OPN2"), _T("YM2612"), },
	{ SC_TYPE_AY8910, _T("PSG"), _T("AY3-8910"), },
	{ SC_TYPE_SN76489, _T("DCSG"), _T("SN76489"), },
	{ SC_TYPE_YM3812, _T("OPL2"), _T("YM3812"), },
	{ SC_TYPE_YMF262, _T("OPL3"), _T("YMF262"), },
	{ SC_TYPE_YM2413, _T("OPLL"), _T("YM2413"), },
	{ SC_TYPE_YM3526, _T("OPL"), _T("YM3526"), },
	{ SC_TYPE_YMF288, _T("OPN3L"), _T("YMF288"), },
	{ SC_TYPE_SCC, _T("SCC"), _T("2212P003"), },
	{ SC_TYPE_SCCS, _T("SCC+"), _T("2312P001"), },
	{ SC_TYPE_Y8950, _T("Y8950"), _T("YM3801"), },
	{ SC_TYPE_YM2164, _T("OPP"), _T("YM2164"), },
	{ SC_TYPE_YM2414, _T("OPZ"), _T("YM2414"), },
	{ SC_TYPE_AY8930, _T("EPSG"), _T("AY8930"), },
	{ SC_TYPE_YM2149, _T("SSG"), _T("YM2149"), },
	{ SC_TYPE_YMZ294, _T("SSGL"), _T("YMZ294"), },
	{ SC_TYPE_SN76496, _T("DCSG"), _T("SN76496"), },
	{ SC_TYPE_YM2420, _T("OPLL2"), _T("YM2420"), },
	{ SC_TYPE_YMF281, _T("OPLLP"), _T("YMF281"), },
	{ SC_TYPE_YMF276, _T("OPN2L"), _T("YMF276"), },
	{ SC_TYPE_YM2610B, _T("OPNBB"), _T("YM2610B"), },
	{ SC_TYPE_YMF286, _T("OPNBK"), _T("YMF286K"), },
	{ SC_TYPE_YM2602, _T("315-5124"), _T("YM2602"), },
	{ SC_TYPE_UM3567, _T("OPLL"), _T("UM3567"), },
	{ SC_TYPE_YMF274, _T("OPL4"), _T("YMF274"), },
	{ SC_TYPE_YM3806, _T("OPQ"), _T("YM3806"), },
	{ SC_TYPE_YM2163, _T("DSG"), _T("YM2163"), },
	{ SC_TYPE_YM7129, _T("OPK2"), _T("YM7129"), },
	{ SC_TYPE_YMZ280, _T("PCMD8"), _T("YMZ280"), },
	{ SC_TYPE_YMZ705, _T("SSGS"), _T("YMZ705"), },
	{ SC_TYPE_YMZ735, _T("FMS"), _T("YMZ735"), },
	{ SC_TYPE_YM2423, _T("OPLLX"), _T("YM2423"), },
	{ SC_TYPE_SPC700, _T("SPC700"), _T("SPC700"), },
	{ SC_TYPE_NBV4,	_T("NBV4") _T("NBV4"), },	// NBV4�p
	{ SC_TYPE_AYB02, _T("AYB02"), _T("AYB02"), },		// AYB02�p
	{ SC_TYPE_8253,	_T("i8253"), _T("i8253"), },	// i8253�i�y�ь݊��`�b�v�p�j
	{ SC_TYPE_OTHER, _T("OTHER"), _T("OTHER"), },
	{ SC_TYPE_UNKNOWN, _T("UNKNOWN"), _T("UNKNOWN"), },
	{ SC_TYPE_MAX, 0, 0, },
};

// CFITOMcfgApp ������

BOOL CFITOMcfgApp::InitInstance()
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
	SetRegistryKey(_T("�A�v���P�[�V���� �E�B�U�[�h�Ő������ꂽ���[�J�� �A�v���P�[�V����"));

	CFITOMcfgDlg dlg;
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

	// ��ō쐬���ꂽ�V�F�� �}�l�[�W���[���폜���܂��B
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	// �_�C�A���O�͕����܂����B�A�v���P�[�V�����̃��b�Z�[�W �|���v���J�n���Ȃ���
	//  �A�v���P�[�V�������I�����邽�߂� FALSE ��Ԃ��Ă��������B
	return FALSE;
}

BOOL CFITOMcfgApp::InitSCCI()
{
	// scci.dll�̓Ǎ���
	if (!(hScci = ::LoadLibrary(_T("scci.dll")))) {
		return -1;
	}

	// �T�E���h�C���^�[�t�F�[�X�}�l�[�W���[�擾�p�֐��A�h���X�擾
	SCCIFUNC getSoundInterfaceManager = (SCCIFUNC)(::GetProcAddress(hScci, _T("getSoundInterfaceManager")));
	if (getSoundInterfaceManager == NULL){
		return -1;
	}

	// �T�E���h�C���^�[�t�F�[�X�}�l�[�W���[�擾
	if (!(pManager = getSoundInterfaceManager())) {
		return -1;
	}

	// �T�E���h�C���^�[�t�F�[�X�}�l�[�W���[�C���X�^���X������
	pManager->initializeInstance();

	// ���Z�b�g���s��
	pManager->init();
	pManager->reset();
	pManager->setLevelDisp(FALSE);

	//pManager->setDelay(0);
	return 0;
}

BOOL CFITOMcfgApp::ExitSCCI()
{
	if (pManager) {
		pManager->releaseAllSoundChip();
		pManager->releaseAllInterface();
		pManager->releaseInstance();
		pManager = 0;
	}
	if (hScci) {
		::FreeLibrary(hScci);
		hScci = 0;
	}
	return TRUE;
}

SC_CHIP_TYPE CFITOMcfgApp::GetChipTypeFromName(LPCTSTR sym)
{
	for (int i = 0; i < SC_TYPE_MAX; i++) {
		if (lstrcmp(symbols[i].chipname, sym) == 0) {
			return symbols[i].type;
		}
	}
	return SC_TYPE_NONE;
}

SC_CHIP_TYPE CFITOMcfgApp::GetChipTypeFromCode(LPCTSTR sym)
{
	for (int i = 0; i < SC_TYPE_MAX; i++) {
		if (lstrcmp(symbols[i].chipcode, sym) == 0) {
			return symbols[i].type;
		}
	}
	return SC_TYPE_NONE;
}

LPCTSTR CFITOMcfgApp::GetChipName(SC_CHIP_TYPE type)
{
	for (int i = 0; i < SC_TYPE_MAX; i++) {
		if (symbols[i].type == type) {
			return symbols[i].chipname;
		}
	}
	return _T("NONE");
}