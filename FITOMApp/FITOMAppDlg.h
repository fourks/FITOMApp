
// FITOMAppDlg.h : ヘッダー ファイル
//

#pragma once

#include "afxwin.h"
#include "afxcmn.h"

#include "LCDView.h"
#include "StretchPicture.h"

class CMidiMonitor;
class CFITOM;

// CFITOMAppDlg ダイアログ
class CFITOMAppDlg : public CDialogEx
{
// コンストラクション
public:
	CFITOMAppDlg(CWnd* pParent = NULL);	// 標準コンストラクター

// ダイアログ データ
	enum { IDD = IDD_FITOMAPP_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV サポート

// 実装
protected:
	HICON m_hIcon;
	CFITOM* theFitom;
	CMidiMonitor* dlgMidi;
	CButton btnStart;
	CButton btnMidiMon;
	CButton btnDevMon;
	bool bMidMon;
	// 生成された、メッセージ割り当て関数
	virtual bool OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual void OnOK();
	virtual void OnCancel() { OnOK(); };
	afx_msg void OnDevMon();
	afx_msg void OnConfig();
	afx_msg void OnMidiMon();
	afx_msg void OnVEdit();
	virtual bool OnCommand(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedButtonDedit();
	CSliderCtrl sldMasterVol;
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedButtonSysctl();
	CStretchPicture stcLogo;
};
