
// FITOMcfgDlg.h : �w�b�_�[ �t�@�C��
//

#pragma once


// CFITOMcfgDlg �_�C�A���O
class CFITOMcfgDlg : public CDialogEx
{
// �R���X�g���N�V����
public:
	CFITOMcfgDlg(CWnd* pParent = NULL);	// �W���R���X�g���N�^�[

// �_�C�A���O �f�[�^
	enum { IDD = IDD_FITOMCFG_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �T�|�[�g


// ����
protected:
	HICON m_hIcon;

	// �������ꂽ�A���b�Z�[�W���蓖�Ċ֐�
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnClickedButtonDev();
	afx_msg void OnClickedButtonMidi();
	afx_msg void OnBnClickedButtonBank();
	afx_msg void OnBnClickedButtonAdpcm();
	afx_msg void OnBnClickedButtonAdpcmBank();
	afx_msg void OnBnClickedButtonExport();
};
