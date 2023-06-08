#ifndef TIME_SERIES_CANVAS_H
#define TIME_SERIES_CANVAS_H
#include "CommonInterfaces/Common2dCanvasInterface.h"
class TimeSeriesCanvas
{
protected:
	struct TimeSeriesInternalData* m_data;
	void shift1PixelToLeft();

public:
	TimeSeriesCanvas(struct Common2dCanvasInterface* canvasInterface, int width, int height, const char* windowTitle);
	virtual ~TimeSeriesCanvas();

	void setupTimeSeries(float yScale, int ticksPerSecond, int startTime, bool clearCanvas = true);
	void addDataSource(const char* dataSourceLabel, const Colorb& clr);
	void insertDataAtCurrentTime(float value, int dataSourceIndex, bool connectToPrevious);
	float getCurrentTime() const;
	void grapicalPrintf(const char* str,  int rasterposx, int rasterposy, const Colorb& clr);

	virtual void nextTick();
};

#endif  //TIME_SERIES_CANVAS_H