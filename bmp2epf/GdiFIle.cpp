#include <iostream>
#include <windows.h>
#include <gdiplus.h>
#include "GdiFIle.h"
using namespace Gdiplus;
using namespace std;

void gdi_extract(std::string fileName, BmpData* outa)
{
	GdiplusStartupInput gpStartupInput;
	ULONG_PTR gpToken;
	GdiplusStartup(&gpToken, &gpStartupInput, NULL);
	HBITMAP result = NULL;

	std::wstring fname(fileName.begin(), fileName.end());
	Gdiplus::Bitmap* bitmap = Gdiplus::Bitmap::FromFile(fname.c_str(), false);

	if (!bitmap)
		throw exception("cant open input file");
	
	int width = bitmap->GetWidth();
	int height = bitmap->GetHeight();

	outa->m_height = height;
	outa->m_width = width;
	outa->m_data.clear();

	Color pixelColor;
	for (int y = 1; y <= height; y++) {
		for (int x = 1; x <= width; x++) {
			bitmap->GetPixel(x, y, &pixelColor);
			outa->m_data.push_back(Pixel(pixelColor.GetRed(), pixelColor.GetGreen(), pixelColor.GetBlue()));
		}
	}
}