#pragma once
#include "afxwin.h"


// CLineInputDlg �_�C�A���O

class CLineInputDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CLineInputDlg)

public:
	CLineInputDlg(CWnd* pParent = NULL);   // �W���R���X�g���N�^�[
	virtual ~CLineInputDlg();

// �_�C�A���O �f�[�^
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MVOL_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g

	DECLARE_MESSAGE_MAP()
public:
	CComboBox cmbLineIn;
	virtual BOOL OnInitDialog();
	virtual void OnOK();
//	afx_msg void OnNcDestroy();
//	virtual void PostNcDestroy();
	afx_msg void OnClose();
};
