// FITOMdll.cpp : DLL �A�v���P�[�V�����p�ɃG�N�X�|�[�g�����֐����`���܂��B
//

#include "stdafx.h"
#include "FITOMdll.h"


// ����́A�G�N�X�|�[�g���ꂽ�ϐ��̗�ł��B
FITOMDLL_API int nFITOMdll=0;

// ����́A�G�N�X�|�[�g���ꂽ�֐��̗�ł��B
FITOMDLL_API int fnFITOMdll(void)
{
	return 42;
}

// ����́A�G�N�X�|�[�g���ꂽ�N���X�̃R���X�g���N�^�[�ł��B
// �N���X��`�Ɋւ��Ă� FITOMdll.h ���Q�Ƃ��Ă��������B
CFITOMdll::CFITOMdll()
{
	return;
}
