// PipeTest1.cpp : �R���\�[�� �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B
//

#include "stdafx.h"
#include <windows.h>
#include <stdio.h>

int main()
{
	HANDLE namedpipe = ::CreateFile(_T("\\\\.\\pipe\\FITOM"), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (namedpipe != INVALID_HANDLE_VALUE) {
		TCHAR buf[1024];
		fgets(buf, 1024, stdin);
	}
	::CloseHandle(namedpipe);
	return 0;
}

