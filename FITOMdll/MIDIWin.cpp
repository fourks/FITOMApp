#include "stdafx.h"
#include "MIDIDEV.h"
#include <MMSystem.h>

CW32MidiIn::CW32MidiIn(char* name) : rpt(0), wpt(0)
{
	MIDIINCAPS mic;
	int dev = -1;
	int numdevs = midiInGetNumDevs();
	for (int i=0; i<numdevs; i++) {
		midiInGetDevCaps(i, &mic, sizeof(mic));
		if (strcmp(name, mic.szPname) == 0) {
			dev = i;
			break;
		}
	}
	StringCchCopy(desc, 32, mic.szPname);
	MMRESULT res = midiInOpen(&hIn, dev, (DWORD_PTR)CW32MidiIn::MidiInProc, (DWORD_PTR)this, CALLBACK_FUNCTION);
	if (res != MMSYSERR_NOERROR) { throw new CResourceException() ; }
	mhdr.dwFlags = 0;
	mhdr.lpData = (LPSTR)(new UINT8[RING_MAX]);
	mhdr.dwBufferLength = RING_MAX;
	midiInPrepareHeader(hIn, &mhdr, sizeof(MIDIHDR));
	midiInStart(hIn);
}

CW32MidiIn::~CW32MidiIn()
{
	midiInStop(hIn);
	midiInReset(hIn);
	midiInUnprepareHeader(hIn, &mhdr, sizeof(MIDIHDR));
	midiInClose(hIn);
}

UINT8 CW32MidiIn::IsReceived(void)
{
    return (wpt!=rpt);
}

UINT8 CW32MidiIn::Read(void)
{
	UINT8 ret = 0;
	ret = buf[rpt];
	rpt = (rpt + 1) & (RING_MAX - 1);
	return ret;
}

int CW32MidiIn::AddBuffer(DWORD msg)
{
	buf[wpt] = LOBYTE(LOWORD(msg));
	wpt = (wpt + 1) & (RING_MAX - 1);
	buf[wpt] = HIBYTE(LOWORD(msg));
	wpt = (wpt + 1) & (RING_MAX - 1);
	buf[wpt] = LOBYTE(HIWORD(msg));
	wpt = (wpt + 1) & (RING_MAX - 1);
	return 0;
}

void CALLBACK CW32MidiIn::MidiInProc(HMIDIIN hMidiIn, UINT wMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
{
	switch(wMsg) {
	case MIM_DATA:
		{
			CW32MidiIn* mpu = (CW32MidiIn*)dwInstance;
			mpu->AddBuffer(dwParam1);
		}
		break;
	case MIM_OPEN:		// ���̓f�o�C�X �I�[�v�� �R�[���o�b�N
		break;
	case MIM_CLOSE:		// ���̓f�o�C�X���N���[�Y
		break;
	case MIM_LONGDATA:	// ���̓o�b�t�@�̃R�[���o�b�N
		break;
	case MIM_ERROR:		// ���̓f�o�C�X �G���[ �R�[���o�b�N
		break;
	case MIM_LONGERROR:	// �����ȃV�X�e�� �G�N�X�N���[�V�u ���b�Z�[�W�ɑ΂���R�[���o�b�N
		break;
	case MIM_MOREDATA:	// ???
		break;
	default:
		break;
	}
}

CW32MidiOut::CW32MidiOut(char* name) : txout(0)
{
	MIDIOUTCAPS moc;
	int dev = -1;
	int numdevs = midiInGetNumDevs();
	for (int i=0; i<numdevs; i++) {
		midiOutGetDevCaps(i, &moc, sizeof(moc));
		if (strcmp(name, moc.szPname) == 0) {
			dev = i;
			break;
		}
	}
	if (dev < 0) {
		dev = 0;
	}
	StringCchCopy(desc, 32, moc.szPname);
	midiOutOpen(&hOut, dev, (DWORD_PTR)CW32MidiOut::MidiOutProc, (DWORD_PTR)this, CALLBACK_FUNCTION);
}

void CALLBACK CW32MidiOut::MidiOutProc(HMIDIOUT hMidiOut, UINT wMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
{
	switch (wMsg) {
	case MOM_DONE:
		{
			CW32MidiOut* mpu = (CW32MidiOut*)dwInstance;
			LPMIDIHDR mhdr = (LPMIDIHDR)dwParam1;
			midiOutUnprepareHeader(hMidiOut, mhdr, sizeof(MIDIHDR));
			delete [] mhdr->lpData;
			delete mhdr;
			mpu->txout--;
		}
		break;
	}
}

void CW32MidiOut::Send(UINT8 data)
{
	Send(&data, 1);
}

void CW32MidiOut::Send(void* msg, size_t len)
{
	if (len <= 4) {
		midiOutShortMsg(hOut, *((DWORD*)msg));
	} else {
		txout++;
		LPMIDIHDR mhdr = new MIDIHDR;
		mhdr->dwFlags = 0;
		mhdr->lpData = (LPSTR)(new UINT8[len]);
		mhdr->dwBufferLength = len;
		memcpy(mhdr->lpData, msg, len);
		midiOutPrepareHeader(hOut, mhdr, sizeof(MIDIHDR));
		midiOutLongMsg(hOut, mhdr, sizeof(MIDIHDR));
	}
}

UINT8 CW32MidiOut::IsTxBusy()
{
	return UINT8(txout);
}

CW32RsMidi::CW32RsMidi(char* name)
{
	hCom = CreateFile(name, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL); // �V���A���|�[�g���J��
	GetCommState(hCom, &dcb); // ���݂̐ݒ�l��ǂݍ���

	dcb.BaudRate = 31500; // ���x
	dcb.ByteSize = 8; // �f�[�^��
	dcb.Parity = NOPARITY; // �p���e�B
	dcb.StopBits = ONESTOPBIT; // �X�g�b�v�r�b�g��
	dcb.fOutxCtsFlow = FALSE; // ���M��CTS�t���[
	dcb.fRtsControl = FALSE; // RTS�t���[

	SetCommState(hCom, &dcb); // �ύX�����ݒ�l����������
	StringCchPrintf(desc, 32, "%1 RS-MIDI(31500bps)", name);
}

const char* CW32RsMidi::GetDescriptor()
{
	return desc;
}

CW32RsMidi::~CW32RsMidi()
{
	CloseHandle(hCom);
}

UINT8 CW32RsMidi::IsReceived()
{
	DWORD errors;
	COMSTAT comStat;
	ClearCommError(hCom, &errors, &comStat);
	return (comStat.cbInQue > 0);
}

UINT8 CW32RsMidi::Read()
{
	UINT8 ret = 0;
	DWORD numberOfPut;
	ReadFile(hCom, &ret, sizeof(ret), &numberOfPut, NULL);
	return ret;
}

UINT8 CW32RsMidi::IsTxBusy()
{
	DWORD errors;
	COMSTAT comStat;
	ClearCommError(hCom, &errors, &comStat);
	return (comStat.cbOutQue > 0);
}

void CW32RsMidi::Send(void* msg, size_t len)
{
	DWORD ret = 0;
	WriteFile(hCom, msg, len, &ret, NULL);
}

void CW32RsMidi::Send(UINT8 data)
{
	Send(&data, 1);
}