#include <iostream>
#include <fstream>
#include <string.h>
#include <time.h>
#include <math.h>
#include "tgaimage.h"
#include "CommonInterfaces/CommonFileIOInterface.h"
#include "Bullet3Common/b3AlignedObjectArray.h"
#include "stb_image/stb_image.h"
#include "stb_image/stb_image_write.h"

TGAImage::TGAImage(int bpp) : data(NULL), width(0), height(0), bytespp(bpp) {}

TGAImage::TGAImage(int w, int h, int bpp) : data(NULL), width(w), height(h), bytespp(bpp)
{
	unsigned long nbytes = width * height * bytespp;
	data = new unsigned char[nbytes];
	//memset(data, 0, nbytes);
}

TGAImage::TGAImage(const TGAImage &img) : data(NULL), width(img.width), height(img.height), bytespp(img.bytespp)
{
	unsigned long nbytes = width * height * bytespp;
	data = new unsigned char[nbytes];
	//memcpy(data, img.data, nbytes);
}

TGAImage::~TGAImage()
{
	if (data) delete[] data;
}

void TGAImage::copy(const TGAImage &img)
{
	if (this != &img)
	{
		if (data) delete[] data;
		width = img.width;
		height = img.height;
		bytespp = img.bytespp;
		unsigned long nbytes = width * height * bytespp;
		data = new unsigned char[nbytes];
		memcpy(data, img.data, nbytes);
	}
	//return *this;
}

bool TGAImage::read_tga_file(const char *filename,   CommonFileIOInterface* fileIO)
{

    b3AlignedObjectArray<char> buffer;
    buffer.reserve(1024);
    int fileId = fileIO->fileOpen(filename, "rb");
    if (fileId >= 0)
    {
        int size = fileIO->getFileSize(fileId);
        if (size > 0)
        {
            buffer.resize(size);
            int actual = fileIO->fileRead(fileId, &buffer[0], size);
            if (actual != size)
            {
                b3Warning("STL filesize mismatch!\n");
                buffer.resize(0);
            }
        }
        fileIO->fileClose(fileId);
    }

    if (buffer.size())
    {
        int n;
        data = stbi_load_from_memory((const unsigned char*)&buffer[0], buffer.size(), &width, &height, &n, 3);
    }
    return true;
}
bool TGAImage::read_tga_file(const char *filename)
{ 
 
    int n;
    data = stbi_load(filename, &width, &height, &n, bytespp);
    return true;
    
}
 
bool TGAImage::write_tga_file(const char *filename) const
{ 
    stbi_write_tga(filename, width, height, bytespp, data);
    return true;
}
 
 
Colorb TGAImage::get(int x, int y) const
{
	if (!data || x < 0 || y < 0 || x >= width || y >= height)
	{
		return Colorb(128.f, 128.f, 128.f, 255.f);
	}
	return Colorb(data + (x + y * width) * bytespp, bytespp);
}

bool TGAImage::set(int x, int y, Colorb &c)
{
	if (!data || x < 0 || y < 0 || x >= width || y >= height)
	{
		return false;
	}
	memcpy(data + (x + y * width) * bytespp, c.c, bytespp);
	return true;
}

bool TGAImage::set(int x, int y, const Colorb &c)
{
	if (!data || x < 0 || y < 0 || x >= width || y >= height)
	{
		return false;
	}
	memcpy(data + (x + y * width) * bytespp, c.c, bytespp);
	return true;
}


void TGAImage::swap(TGAImage& img)
{
    b3Assert(bytespp == img.bytespp);
    std::swap(width, img.width);
    std::swap(height, img.height);
    std::swap(data, img.data);

}

int TGAImage::get_bytespp()
{
	return bytespp;
}

int TGAImage::get_width()
{
	return width;
}

int TGAImage::get_height()
{
	return height;
}

bool TGAImage::flip_horizontally()
{
	if (!data) return false;
	int half = width >> 1;
	for (int i = 0; i < half; i++)
	{
		for (int j = 0; j < height; j++)
		{
			Colorb c1 = get(i, j);
			Colorb c2 = get(width - 1 - i, j);
			set(i, j, c2);
			set(width - 1 - i, j, c1);
		}
	}
	return true;
}

bool TGAImage::flip_vertically()
{
	if (!data) return false;
	unsigned long bytes_per_line = width * bytespp;
	unsigned char *line = new unsigned char[bytes_per_line];
	int half = height >> 1;
	for (int j = 0; j < half; j++)
	{
		unsigned long l1 = j * bytes_per_line;
		unsigned long l2 = (height - 1 - j) * bytes_per_line;
		memmove((void *)line, (void *)(data + l1), bytes_per_line);
		memmove((void *)(data + l1), (void *)(data + l2), bytes_per_line);
		memmove((void *)(data + l2), (void *)line, bytes_per_line);
	}
	delete[] line;
	return true;
}

unsigned char *TGAImage::buffer()
{
	return data;
}

void TGAImage::clear()
{
	memset((void *)data, 0, width * height * bytespp);
}

bool TGAImage::scale(int w, int h)
{
	if (w <= 0 || h <= 0 || !data) return false;
	unsigned char *tdata = new unsigned char[w * h * bytespp];
	int nscanline = 0;
	int oscanline = 0;
	int erry = 0;
	unsigned long nlinebytes = w * bytespp;
	unsigned long olinebytes = width * bytespp;
	for (int j = 0; j < height; j++)
	{
		int errx = width - w;
		int nx = -bytespp;
		int ox = -bytespp;
		for (int i = 0; i < width; i++)
		{
			ox += bytespp;
			errx += w;
			while (errx >= (int)width)
			{
				errx -= width;
				nx += bytespp;
				memcpy(tdata + nscanline + nx, data + oscanline + ox, bytespp);
			}
		}
		erry += h;
		oscanline += olinebytes;
		while (erry >= (int)height)
		{
			if (erry >= (int)height << 1)  // it means we jump over a scanline
				memcpy(tdata + nscanline + nlinebytes, tdata + nscanline, nlinebytes);
			erry -= height;
			nscanline += nlinebytes;
		}
	}
	delete[] data;
	data = tdata;
	width = w;
	height = h;
	return true;
}
