#ifndef COMMON_2D_CANVAS_INTERFACE_H
#define COMMON_2D_CANVAS_INTERFACE_H
#include "Bullet3Common/Color.h"
struct Common2dCanvasInterface
{
	virtual ~Common2dCanvasInterface() {}
	virtual int createCanvas(const char* canvasName, int width, int height, int xPos, int yPos) = 0;
	virtual void destroyCanvas(int canvasId) = 0;
	virtual void setPixel(int canvasId, int x, int y, const Colorb& clr) = 0;
	virtual void getPixel(int canvasId, int x, int y, Colorb& clr) = 0;

	virtual void refreshImageData(int canvasId) = 0;
};

#endif  //COMMON_2D_CANVAS_INTERFACE_H
