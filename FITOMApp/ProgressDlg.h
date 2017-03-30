#pragma once
#include "afxcmn.h"


// CProgressDlg �_�C�A���O

class CProgressDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CProgressDlg)

public:
	CProgressDlg(CWnd* pParent = NULL);   // �W���R���X�g���N�^�[
	virtual ~CProgressDlg();

// �_�C�A���O �f�[�^
	enum { IDD = IDD_PROGRESS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g

	DECLARE_MESSAGE_MAP()
	virtual void PostNcDestroy();
	virtual void OnOK();
	virtual void OnCancel();
public:
	void SetRange(int lower, int upper);
	int SetPos(int pos);
	void SetStatus(LPCTSTR str);
	void SetTarget(LPCTSTR str);
	CProgressCtrl progBar;
};
