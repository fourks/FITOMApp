#pragma once
#include "StretchPicture.h"


// CSplashDlg �_�C�A���O

class CSplashDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSplashDlg)

public:
	CSplashDlg(CWnd* pParent = NULL);   // �W���R���X�g���N�^�[
	virtual ~CSplashDlg();

// �_�C�A���O �f�[�^
	enum { IDD = IDD_SPLASH };
	void SetStatus(LPCTSTR str);
	void SetFilename(LPCTSTR str);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g

	DECLARE_MESSAGE_MAP()
	virtual void PostNcDestroy();
	virtual void OnOK();
	virtual void OnCancel();
public:
	CStretchPicture stcLogo;
};
