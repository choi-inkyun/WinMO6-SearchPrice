
#pragma once

#include <Imaging.h>

class CWMImage
{
public:

	CWMImage();
	~CWMImage();

	bool LoadImage(LPCTSTR szImagePath);
	void UnloadImage();
	bool GetImageInfo(ImageInfo	* pInfo);
	bool Draw(HDC hDC, RECT* pDestRect, RECT* pSourceRect);
	bool IsImageLoaded();

	int GetWidth();
	int GetHeight();

private:
	IImage* m_pImage;
	ImageInfo m_ImageInfo;
	RECT m_Rect;
};
