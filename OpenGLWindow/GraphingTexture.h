#ifndef GRAPHING_TEXTURE_H
#define GRAPHING_TEXTURE_H
#include "Bullet3Common/b3AlignedObjectArray.h"
#include "Bullet3Common/Color.h"

struct GraphingTexture
{
	int m_textureId;
	//assume rgba (8 bit per component, total of 32bit per pixel, for m_width*m_height pixels)
	b3AlignedObjectArray<unsigned char> m_imageData;
	int m_width;
	int m_height;

	GraphingTexture();
	virtual ~GraphingTexture();

	bool create(int texWidth, int texHeight);
	void destroy();

	void setPixel(int x, int y, const Colorb& clr)
	{
		if (y >= 0 && y < m_height && x >= 0 && x < m_width)
		{
			m_imageData[x * 4 + y * 4 * m_width + 0] = clr.r;
			m_imageData[x * 4 + y * 4 * m_width + 1] = clr.g;
			m_imageData[x * 4 + y * 4 * m_width + 2] = clr.b;
			m_imageData[x * 4 + y * 4 * m_width + 3] = clr.a;
		}
	}

	void getPixel(int x, int y, Colorb& clr)
	{
		clr.r = m_imageData[x * 4 + y * 4 * m_width + 0];
        clr.g = m_imageData[x * 4 + y * 4 * m_width + 1];
        clr.b = m_imageData[x * 4 + y * 4 * m_width + 2];
        clr.a = m_imageData[x * 4 + y * 4 * m_width + 3];
	}
	void uploadImageData();

	int getTextureId()
	{
		return m_textureId;
	}
};

#endif  //GRAPHING_TEXTURE_H
