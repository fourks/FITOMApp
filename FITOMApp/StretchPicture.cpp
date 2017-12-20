// StretchPicture.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "FITOMApp.h"
#include "StretchPicture.h"


// CStretchPicture

IMPLEMENT_DYNAMIC(CStretchPicture, CStatic)

CStretchPicture::CStretchPicture()
{

}

CStretchPicture::~CStretchPicture()
{
}


BEGIN_MESSAGE_MAP(CStretchPicture, CStatic)
	ON_WM_PAINT()
	ON_WM_PAINT()
END_MESSAGE_MAP()



// CStretchPicture ���b�Z�[�W �n���h���[


void CStretchPicture::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: �����Ƀ��b�Z�[�W �n���h���[ �R�[�h��ǉ����܂��B
					   // �`�惁�b�Z�[�W�� CStatic::OnPaint() ���Ăяo���Ȃ��ł��������B
	HBITMAP hbmp = GetBitmap();
	if (hbmp) {
		CRect rc;
		this->GetWindowRect(&rc);
		Gdiplus::Graphics* gr = new Gdiplus::Graphics(dc);
		Gdiplus::Bitmap* bmp = new Gdiplus::Bitmap(hbmp, 0);
		gr->DrawImage(bmp, Gdiplus::Rect(0, 0, rc.Width(), rc.Height()));
	}
}
