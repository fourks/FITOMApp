#pragma once
#include "afxcmn.h"
#include <vector>

// CAdPcmDevDlg �_�C�A���O

class CAdPcmDevDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CAdPcmDevDlg)

public:
	CAdPcmDevDlg(CWnd* pParent = NULL);   // �W���R���X�g���N�^�[
	virtual ~CAdPcmDevDlg();

// �_�C�A���O �f�[�^
	enum { IDD = IDD_ADPCM_DEV_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g
	struct scciDeviceProperty {
		DWORD ifid;
		DWORD slid;
		SCCI_SOUND_CHIP_INFO* pssci;
		SoundChip* psc;
	};
	struct scciInterface {
		SoundInterface* psi;
		SCCI_INTERFACE_INFO* psii;
		std::vector<scciDeviceProperty*> sdps;
	};
	std::vector<scciInterface*> sis;
	struct adpcmDevCfg {
		SC_CHIP_TYPE type;
		DWORD subtype;
		DWORD ifid;
		DWORD slid;
		TCHAR bnkfile[MAX_PATH];
	};
	std::vector<adpcmDevCfg> devuse;
	DECLARE_MESSAGE_MAP()
public:
	CListCtrl lstAdpcmDevice;
	LRESULT OnKickIdle(WPARAM wParam, LPARAM lParam);
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	void Refresh();
	afx_msg void OnBtnUp();
	afx_msg void OnBtnDown();
	afx_msg void OnBtnDel();
	afx_msg void OnUpdateBtn(CCmdUI *pCmdUI);
	afx_msg void OnDblclkListAdpcm(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDestroy();
};
