#pragma once
#include "afxcmn.h"
#include "FITOM.h"

// CDrumMapPicker �_�C�A���O

class CFITOMConfig;

class CDrumMapPicker : public CDialogEx
{
	DECLARE_DYNAMIC(CDrumMapPicker)

public:
	CDrumMapPicker(CWnd* pParent = NULL);   // �W���R���X�g���N�^�[
	virtual ~CDrumMapPicker();

// �_�C�A���O �f�[�^
	enum { IDD = IDD_DLG_DPICK };

protected:
	CFITOMConfig* theConfig;
	int theProg;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDblclkDrummapList(NMHDR *pNMHDR, LRESULT *pResult);
	virtual bool OnInitDialog();
	virtual void OnOK();
	void SetProg(int prog) { theProg = prog; };
	int GetProg() { return theProg; };
	CListCtrl lstDrumMap;
};
