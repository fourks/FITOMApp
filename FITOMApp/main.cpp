#include "stdafx.h"
#include "FITOM.h"
#include "Screen.h"
#include "timer.h"

static const char* errmsg[] = {
	"�R���t�B�O�t�@�C�����I�[�v���ł��܂���\n",
	"MIDI�|�[�g���w�肳��Ă��܂���\n",
	"FM�����f�o�C�X���w�肳��Ă��܂���\n",
};

CFITOM* theApp;
CTimer* theTimer;
CMainScreen* MainScr;

int volatile exec = 1;

void TimerProc()
{
	if (exec) {
		if (theApp) { theApp->TimerCallBack(theTimer->GetClock()); }
		if (MainScr) { MainScr->Show(1); };
	}
}

void Error(int code)
{
	fprintf(stderr, errmsg[-code]);
	exit(-1);
}

int main(int argc, char* argv[])
{
	int res;
	int save = 0;
	if (argc > 1 && (strcmp("-save", argv[1]) == 0)) {
		save = 1;
	}
	theApp = CFITOM::GetInstance();
	MainScr = new CMainScreen(theApp);

	if (!(res=theApp->InitInstance()))
	{
		theTimer = CTimer::GetInstance(10);
		theTimer->SetIntr(TimerProc);
		exec = 1;
		MainScr->Modal(0);
		exec = 0;
		theTimer->Stop();
	} else {
		Error(res);
	}
	theApp->ExitInstance(save);
	delete MainScr;
	return 0;
}

