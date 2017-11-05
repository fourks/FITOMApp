#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CInstDevDlg �_�C�A���O

class CInstDevDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CInstDevDlg)

public:
	CInstDevDlg(CWnd* pParent = NULL);   // �W���R���X�g���N�^�[
	virtual ~CInstDevDlg();

// �_�C�A���O �f�[�^
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_INST_DLG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl lstDevice;
	CComboBox cmbModule;
	CComboBox cmbMode;
	CComboBox cmbType;
};
