
// FITOMcfgDlg.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "FITOMcfg.h"
#include "FITOMcfgDlg.h"
#include "afxdialogex.h"

#include "MidiCfgDlg.h"
#include "DevCfgDlg.h"
#include "VoiceBankDlg.h"
#include "AdPcmDevDlg.h"
#include "PcmBankDlg.h"
#include "ExportIns.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// �A�v���P�[�V�����̃o�[�W�������Ɏg���� CAboutDlg �_�C�A���O

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �_�C�A���O �f�[�^
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g

// ����
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CFITOMcfgDlg �_�C�A���O



CFITOMcfgDlg::CFITOMcfgDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CFITOMcfgDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFITOMcfgDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CFITOMcfgDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_DEV, &CFITOMcfgDlg::OnClickedButtonDev)
	ON_BN_CLICKED(IDC_BUTTON_MIDI, &CFITOMcfgDlg::OnClickedButtonMidi)
	ON_BN_CLICKED(IDC_BUTTON_BANK, &CFITOMcfgDlg::OnBnClickedButtonBank)
	ON_BN_CLICKED(IDC_BUTTON_ADPCM, &CFITOMcfgDlg::OnBnClickedButtonAdpcm)
	ON_BN_CLICKED(IDC_BUTTON_ADPCM_BANK, &CFITOMcfgDlg::OnBnClickedButtonAdpcmBank)
	ON_BN_CLICKED(IDC_BUTTON_EXPORT, &CFITOMcfgDlg::OnBnClickedButtonExport)
END_MESSAGE_MAP()


// CFITOMcfgDlg ���b�Z�[�W �n���h���[

BOOL CFITOMcfgDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// "�o�[�W�������..." ���j���[���V�X�e�� ���j���[�ɒǉ����܂��B

	// IDM_ABOUTBOX �́A�V�X�e�� �R�}���h�͈͓̔��ɂȂ���΂Ȃ�܂���B
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���̃_�C�A���O�̃A�C�R����ݒ肵�܂��B�A�v���P�[�V�����̃��C�� �E�B���h�E���_�C�A���O�łȂ��ꍇ�A
	//  Framework �́A���̐ݒ�������I�ɍs���܂��B
	SetIcon(m_hIcon, TRUE);			// �傫���A�C�R���̐ݒ�
	SetIcon(m_hIcon, FALSE);		// �������A�C�R���̐ݒ�

	// TODO: �������������ɒǉ����܂��B

	return TRUE;  // �t�H�[�J�X���R���g���[���ɐݒ肵���ꍇ�������ATRUE ��Ԃ��܂��B
}

void CFITOMcfgDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �_�C�A���O�ɍŏ����{�^����ǉ�����ꍇ�A�A�C�R����`�悷�邽�߂�
//  ���̃R�[�h���K�v�ł��B�h�L�������g/�r���[ ���f�����g�� MFC �A�v���P�[�V�����̏ꍇ�A
//  ����́AFramework �ɂ���Ď����I�ɐݒ肳��܂��B

void CFITOMcfgDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �`��̃f�o�C�X �R���e�L�X�g

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// �N���C�A���g�̎l�p�`�̈���̒���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �A�C�R���̕`��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// ���[�U�[���ŏ��������E�B���h�E���h���b�O���Ă���Ƃ��ɕ\������J�[�\�����擾���邽�߂ɁA
//  �V�X�e�������̊֐����Ăяo���܂��B
HCURSOR CFITOMcfgDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CFITOMcfgDlg::OnClickedButtonDev()
{
	// TODO: Add your control notification handler code here
	CDevCfgDlg dlg(this);
	if (dlg.DoModal() == IDOK) {

	}
}


void CFITOMcfgDlg::OnClickedButtonMidi()
{
	// TODO: Add your control notification handler code here
	CMidiCfgDlg dlg(this);
	if (dlg.DoModal() == IDOK) {

	}
}


void CFITOMcfgDlg::OnBnClickedButtonBank()
{
	// TODO: Add your control notification handler code here
	CVoiceBankDlg dlg(this);
	if (dlg.DoModal() == IDOK) {

	}
}


void CFITOMcfgDlg::OnBnClickedButtonAdpcm()
{
	// TODO: �����ɃR���g���[���ʒm�n���h���[ �R�[�h��ǉ����܂��B
	CAdPcmDevDlg dlg(this);
	if (dlg.DoModal() == IDOK) {

	}
}


void CFITOMcfgDlg::OnBnClickedButtonAdpcmBank()
{
	// TODO: �����ɃR���g���[���ʒm�n���h���[ �R�[�h��ǉ����܂��B
	CPcmBankDlg dlg(this);
	if (dlg.DoModal() == IDOK) {

	}
}


void CFITOMcfgDlg::OnBnClickedButtonExport()
{
	// TODO: �����ɃR���g���[���ʒm�n���h���[ �R�[�h��ǉ����܂��B
	ExportInsFile();
}
