#ifndef __IMAGE_H__
#define __IMAGE_H__

#include <fstream>
#include "Bullet3Common/Color.h"
#pragma pack(push, 1)
struct TGA_Header
{
	char idlength;
	char colormaptype;
	char datatypecode;
	short colormaporigin;
	short colormaplength;
	char colormapdepth;
	short x_origin;
	short y_origin;
	short width;
	short height;
	char bitsperpixel;
	char imagedescriptor;
};
#pragma pack(pop)

struct CommonFileIOInterface;

class TGAImage
{
protected:
	unsigned char *data;
	int width;
	int height;
	int bytespp;

public:
	enum Format
	{
		GRAYSCALE = 1,
		RGB = 3,
		RGBA = 4
	};

	TGAImage(int bpp = 3);
	TGAImage(int w, int h, int bpp);
	TGAImage(const TGAImage &img);
    void swap(TGAImage &img);
	bool read_tga_file(const char *filename);
    bool read_tga_file(const char *filename,   CommonFileIOInterface* fileIO);
    bool write_tga_file(const char *filename) const;
	bool flip_horizontally();
	bool flip_vertically();
	bool scale(int w, int h);
	Colorb get(int x, int y) const;

	bool set(int x, int y, Colorb &c);
	bool set(int x, int y, const Colorb &c);
	~TGAImage();
    void copy(const TGAImage &img);//??
	int get_width();
	int get_height();
	int get_bytespp();
	unsigned char *buffer();
	void clear();
     
};

#endif  //__IMAGE_H__
