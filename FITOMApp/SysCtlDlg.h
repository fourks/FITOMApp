#pragma once


// CSysCtlDlg �_�C�A���O

class CSysCtlDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSysCtlDlg)

public:
	CSysCtlDlg(CWnd* pParent = NULL);   // �W���R���X�g���N�^�[
	virtual ~CSysCtlDlg();

// �_�C�A���O �f�[�^
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SYSCTL_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnDevinit();
	afx_msg void OnBnClickedBtnRestart();
	afx_msg void OnBnClickedBtnReload();
};
