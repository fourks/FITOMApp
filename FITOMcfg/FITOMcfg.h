
// FITOMcfg.h : PROJECT_NAME �A�v���P�[�V�����̃��C�� �w�b�_�[ �t�@�C���ł��B
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH �ɑ΂��Ă��̃t�@�C�����C���N���[�h����O�� 'stdafx.h' ���C���N���[�h���Ă�������"
#endif

#include "resource.h"		// ���C�� �V���{��

#include "scci.h"
#include "SCCIDefines.h"

// CFITOMcfgApp:
// ���̃N���X�̎����ɂ��ẮAFITOMcfg.cpp ���Q�Ƃ��Ă��������B
//

struct chipSymbols {
	SC_CHIP_TYPE type;
	LPCTSTR chipname;
	LPCTSTR chipcode;
};

class CFITOMcfgApp : public CWinApp
{
protected:
	HMODULE hScci;
	SoundInterfaceManager *pManager;

public:
	CFITOMcfgApp();
	BOOL InitSCCI();
	BOOL ExitSCCI();
	SoundInterfaceManager* GetManager() { return pManager; };
// �I�[�o�[���C�h
public:
	virtual BOOL InitInstance();
	SC_CHIP_TYPE GetChipTypeFromName(LPCTSTR sym);
	SC_CHIP_TYPE GetChipTypeFromCode(LPCTSTR sym);
	LPCTSTR GetChipName(SC_CHIP_TYPE type);
	// ����
	DECLARE_MESSAGE_MAP()
};

extern CFITOMcfgApp theApp;