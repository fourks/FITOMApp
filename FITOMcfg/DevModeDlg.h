#pragma once


// CDevModeDlg �_�C�A���O

class CDevModeDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CDevModeDlg)

public:
	CDevModeDlg(CWnd* pParent = NULL);   // �W���R���X�g���N�^�[
	virtual ~CDevModeDlg();

// �_�C�A���O �f�[�^
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DEVMODE_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g

	DECLARE_MESSAGE_MAP()
};
