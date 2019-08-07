
#include "stdafx.h"
#include "Image.h"

#include <windows.h>
#include <aygshell.h>
#include <atlbase.h>

#include <initguid.h>
#include <imgguids.h>

CWMImage::CWMImage()
{
	m_pImage = NULL;
	ZeroMemory(&m_ImageInfo, sizeof(m_ImageInfo));
}
CWMImage::~CWMImage()
{
	UnloadImage();
}
bool CWMImage::LoadImage(LPCTSTR szImagePath)
{
	bool retVal = FALSE;
	IImagingFactory* pImageFactory;
	HRESULT hr;

	if (m_pImage == NULL || SUCCEEDED(m_pImage->Release()))
	{
		m_pImage = NULL;
		hr = CoCreateInstance(	CLSID_ImagingFactory,
			NULL,
			CLSCTX_INPROC_SERVER,
			IID_IImagingFactory,
			(void**) &pImageFactory);
		if (SUCCEEDED(hr))
		{
			hr = pImageFactory->CreateImageFromFile(szImagePath,
				&m_pImage);
			if (SUCCEEDED(hr))
			{
				hr = m_pImage->GetImageInfo(&m_ImageInfo);
				if (SUCCEEDED(hr))
				{
					retVal = TRUE;
				}
				else
				{
					m_pImage->Release();
					m_pImage = NULL;
				}
			}
			pImageFactory->Release();
		}
	}

	return retVal;
}
void CWMImage::UnloadImage()
{
	if (m_pImage)
	{
		ULONG lRef = m_pImage->Release();
		m_pImage = NULL;
	}
}
bool CWMImage::GetImageInfo(ImageInfo* pInfo)
{
	bool retVal = FALSE;
	if (m_pImage != NULL)
	{
		*pInfo = m_ImageInfo;
		retVal = TRUE;
	}

	return retVal;
}

bool CWMImage::Draw(HDC hDC, RECT* pDestRect, RECT* pSourceRect)
{
	bool retVal = FALSE;
	HRESULT hr;
	if (m_pImage != NULL)
	{
		if (pDestRect)
			m_Rect = *pDestRect;
		hr = m_pImage->Draw(hDC, &m_Rect, NULL);
		if (SUCCEEDED(hr))
			retVal = TRUE;
	}

	return retVal;
}

bool CWMImage::IsImageLoaded()
{
	bool retVal;
	if (m_pImage == NULL)
		retVal = FALSE;
	else
		retVal = TRUE;

	return retVal;
}

int CWMImage::GetWidth()
{
	if (m_pImage == NULL) return 0;
	return m_ImageInfo.Width;
}
int CWMImage::GetHeight()
{
	if (m_pImage == NULL) return 0;
	return m_ImageInfo.Height;
}
