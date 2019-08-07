
#pragma once

#include <vector>
#include "image.h"

class CGoodsData
{
public:
	CGoodsData()
	{
		tilte = L"";
		link = L"";
		imageUrl = L"";
		imageFilePath = L"";
		lprice = 0;
		hprice = 0;
	}
	~CGoodsData()
	{
	}
public:
	std::wstring tilte;
	std::wstring link;
	std::wstring imageUrl;
	std::wstring imageFilePath;
	int lprice;
	int hprice;

	CWMImage sampleImage;
};