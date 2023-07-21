#include "TimeSeriesCanvas.h"
#include "CommonInterfaces/Common2dCanvasInterface.h"
#include "Bullet3Common/b3AlignedObjectArray.h"
//#include "TimeSeriesFontData.h"
#include "Bullet3Common/b3Vector3.h"
#include "OpenGLWindow/SimpleOpenGL3App.h"
#include "OpenGLWindow/fontstash.h"
#include <stdio.h>
extern unsigned char OpenSansData[];
struct DataSource
{
	unsigned char m_red;
	unsigned char m_green;
	unsigned char m_blue;
    bool m_hasLastValue;
	float m_lastValue;

	DataSource()
		: m_hasLastValue(false)
	{
	}
};

struct TimeSeriesInternalData
{
	b3AlignedObjectArray<DataSource> m_dataSources;

	Common2dCanvasInterface* m_canvasInterface;
	int m_canvasIndex;
	int m_width;
	int m_height;

	float m_pixelsPerUnit;
	float m_zero;
	int m_timeTicks;
	int m_ticksPerSecond;
	float m_yScale;
	int m_bar;
    struct sth_stash* m_fontStash;
    RenderCallbacks* m_renderCallbacks;
    int m_droidRegular;
	Colorb m_background;

    Colorb m_textColor;

	float getTime()
	{
		return m_timeTicks / (float)m_ticksPerSecond;
	}

	TimeSeriesInternalData(int width, int height)
		: m_width(width),
		  m_height(height),
		  m_pixelsPerUnit(-100),
		  m_zero(height / 2.0),
		  m_timeTicks(0),
		  m_ticksPerSecond(100),
		  m_yScale(1),
		  m_bar(0),
		  m_background(Colorb(255, 255, 255, 255)),
		  m_textColor(Colorb(0, 0, 255, 255)),
        m_fontStash(0),
        m_renderCallbacks(0)
	{
	}
};
struct CanvasRenderCallbacks : public RenderCallbacks
{///TODO...
    CanvasRenderCallbacks(Common2dCanvasInterface* canvasInterface, int canvasIndex, int w, int h, const Colorf& backColor)
    {
        m_canvasInterface = canvasInterface;
        m_canvasIndex = canvasIndex;
        m_width = w;
        m_height = h;

 
        {
            m_color[0] = 0.1; 
            m_color[0] = 0.1;
            m_color[0] = 0.1;
            m_color[0] = 1.0; 
            m_back = backColor;
        }
        m_posx = m_posy = 0;
    }
    virtual ~CanvasRenderCallbacks()
    {
        m_rgbaTexture.clear();
    }
    virtual void setWorldPosition(float pos[3])
    {
        m_posx = pos[0];
        m_posy = pos[1];
        strNum = pos[2];
    }
    virtual void setWorldOrientation(float orn[4])
    {}
    virtual void setColorRGBA(const Colorf& color)
    {
        for (int i = 0; i < 4; i++)
        {
            m_color[i] = color[i];//  
        }
    }
    virtual void updateTexture(sth_texture* texture, sth_glyph* glyph, int textureWidth, int textureHeight)
    {
        if (glyph)
        {
            render(texture);
        }
        else
        {
            if (textureWidth && textureHeight)
            {
                this->textureWidth = textureWidth;
                this->textureHeight = textureHeight;
                texture->m_texels = (unsigned char*)malloc(textureWidth * textureHeight);
                memset(texture->m_texels, 0, textureWidth * textureHeight);
            }
            else
            {
                delete texture->m_texels;
                texture->m_texels = 0;
                
            }
        }
    }
    virtual void render(sth_texture* texture)
    {
        m_rgbaTexture.resize(textureWidth * textureHeight);
        for (int i = 0; i < textureWidth * textureHeight; i++)
        {
            m_rgbaTexture[i] = texture->m_texels[i];
        }
        Colorb color(m_color.r * 255, m_color.g * 255, m_color.b * 255, m_color.a * 255);
        //int w = b3Min(textureWidth, strNum * 16);
        for (int j = 0; j < 16; j++)
        {
            for (int i = 0; i < textureWidth; i++)
            {
                unsigned char packedColor = m_rgbaTexture[j*textureWidth + i];
                Colorb color1 = color;
                if (packedColor < 255 && packedColor > 0)
                {
                    float a = packedColor/ 255.f;
                    color1.r = (m_color.r * a + m_back.r * (1 - a)) * 255;
                    color1.g = (m_color.g * a + m_back.g * (1 - a)) * 255;
                    color1.b = (m_color.b * a + m_back.b * (1 - a)) * 255;
                }
                if (packedColor && 
                    (m_posx + i >= 0) && (m_posx + i < m_width) &&
                    (m_posy + j >= 0) && (m_posy + j < m_height))
                {
                    m_canvasInterface->setPixel(m_canvasIndex, m_posx + i, m_posy + j, color1);
                }
            }
        }
    }
    Common2dCanvasInterface* m_canvasInterface;
    int m_canvasIndex;
    b3AlignedObjectArray<unsigned char> m_rgbaTexture;
    int m_posx, m_posy;
    Colorf m_color;
    Colorf m_back;
    int m_width;
    int m_height;
    int textureWidth, textureHeight;
    int strNum;

};
TimeSeriesCanvas::TimeSeriesCanvas(Common2dCanvasInterface* canvasInterface, int width, int height, const char* windowTitle)
{
	m_data = new TimeSeriesInternalData(width, height);

	m_data->m_canvasInterface = canvasInterface;

	if (canvasInterface)
	{
		m_data->m_canvasIndex = m_data->m_canvasInterface->createCanvas(windowTitle, m_data->m_width, m_data->m_height, 20, 50);
	}
    Colorf clrf(m_data->m_background.r / 256.f, m_data->m_background.g / 256.f, m_data->m_background.b / 256.f, m_data->m_background.a / 256.f );
    m_data->m_renderCallbacks = new CanvasRenderCallbacks(m_data->m_canvasInterface, m_data->m_canvasIndex, 
        m_data->m_width, m_data->m_height, clrf);//??
    m_data->m_fontStash = sth_create(512, 512, m_data->m_renderCallbacks);  //256,256);//,1024);//512,512);
    unsigned char* data = OpenSansData;
    if (!(m_data->m_droidRegular = sth_add_font_from_memory(m_data->m_fontStash, data)))
    {
        b3Warning("error!\n");
    }
}

void TimeSeriesCanvas::addDataSource(const char* dataSourceLabel, const Colorb& clr)
{
	DataSource dataSource;
	dataSource.m_red = clr.r;
	dataSource.m_green = clr.g;
	dataSource.m_blue = clr.b;
	dataSource.m_lastValue = 0;
	dataSource.m_hasLastValue = false;

	if (dataSourceLabel)
	{
		int numSources = m_data->m_dataSources.size();
		int row = numSources % 3;
		int column = numSources / 3;
		grapicalPrintf(dataSourceLabel,  50 + 200 * column, m_data->m_height - 48 + row * 16, clr);
	}

	m_data->m_dataSources.push_back(dataSource);
}
void TimeSeriesCanvas::setupTimeSeries(float yScale, int ticksPerSecond, int startTime, bool clearCanvas)
{
	if (0 == m_data->m_canvasInterface)
		return;

	m_data->m_pixelsPerUnit = -(m_data->m_height / 3.f) / yScale;
	m_data->m_ticksPerSecond = ticksPerSecond;
	m_data->m_yScale = yScale;
	m_data->m_dataSources.clear();

	if (clearCanvas)
	{
		for (int i = 0; i < m_data->m_width; i++)
		{
			for (int j = 0; j < m_data->m_height; j++)
			{
				m_data->m_canvasInterface->setPixel(m_data->m_canvasIndex, i, j, m_data->m_background);
			}
		}
	}

	float zeroPixelCoord = m_data->m_zero;
	float pixelsPerUnit = m_data->m_pixelsPerUnit;

	float yPos = zeroPixelCoord + pixelsPerUnit * yScale;
	float yNeg = zeroPixelCoord + pixelsPerUnit * -yScale;

	grapicalPrintf("0", 2, zeroPixelCoord, m_data->m_textColor);
	char label[1024];
	sprintf(label, "%2.1f", yScale);
	grapicalPrintf(label, 2, yPos, m_data->m_textColor);
	sprintf(label, "%2.1f", -yScale);
	grapicalPrintf(label, 2, yNeg, m_data->m_textColor);

	m_data->m_canvasInterface->refreshImageData(m_data->m_canvasIndex);
}

TimeSeriesCanvas::~TimeSeriesCanvas()
{
	if (m_data->m_canvasInterface && m_data->m_canvasIndex >= 0)
	{
		m_data->m_canvasInterface->destroyCanvas(m_data->m_canvasIndex);
	}
	delete m_data;
}

float TimeSeriesCanvas::getCurrentTime() const
{
	return m_data->getTime();
}

void TimeSeriesCanvas::grapicalPrintf(const char* str,  int rasterposx, int rasterposy, const Colorb& clr)
{//??
    sth_stash* m_font = m_data->m_fontStash;
    float dx = 0;
    float pos[3] = { (float)rasterposx, (float)rasterposy, (float)strlen(str) };
    m_data->m_renderCallbacks->setWorldPosition(pos);
    Colorf clrf(clr.r / 256.f, clr.g / 256.f, clr.b / 256.f, clr.a / 256.f );
    m_data->m_renderCallbacks->setColorRGBA(clrf.c);
 
    sth_draw_text(m_font, m_data->m_droidRegular, 16.0f, 0, 0,
        str, &dx, 512, 512, false, 1.0, clrf.c);
    sth_flush_draw(m_font);
 }

void TimeSeriesCanvas::shift1PixelToLeft()
{
	int resetVal = 10;
	int countdown = resetVal;

	//shift pixture one pixel to the left
	for (int j = 50; j < m_data->m_height - 48; j++)
	{
		for (int i = 40; i < this->m_data->m_width; i++)
		{
			Colorb clr;
			m_data->m_canvasInterface->getPixel(m_data->m_canvasIndex, i, j, clr);
			m_data->m_canvasInterface->setPixel(m_data->m_canvasIndex, i - 1, j, clr);
		}
		if (!m_data->m_bar)
		{
			if (!countdown--)
			{
				countdown = resetVal;
                Colorb clr(0, 0, 0, 255);
				m_data->m_canvasInterface->setPixel(m_data->m_canvasIndex, m_data->m_width - 1, j, clr);
			}
			else
            {
                Colorb clr(255, 255, 255, 255);
				m_data->m_canvasInterface->setPixel(m_data->m_canvasIndex, m_data->m_width - 1, j, clr);
			}
		}
		else
        {
            Colorb clr(255, 255, 255, 255);
			m_data->m_canvasInterface->setPixel(m_data->m_canvasIndex, m_data->m_width - 1, j, clr);
		}
	}

	{
		int resetVal = 2;
		static int countdown = resetVal;
		if (!countdown--)
		{
			countdown = resetVal;
            Colorb clr(0, 0, 0, 255);
			m_data->m_canvasInterface->setPixel(m_data->m_canvasIndex, m_data->m_width - 1, m_data->m_zero, clr);
		}
	}

	{
		int resetVal = 10;
		static int countdown = resetVal;
		if (!countdown--)
		{
			countdown = resetVal;
			float zeroPixelCoord = m_data->m_zero;
			float pixelsPerUnit = m_data->m_pixelsPerUnit;

			float yPos = zeroPixelCoord + pixelsPerUnit * m_data->m_yScale;
			float yNeg = zeroPixelCoord + pixelsPerUnit * -m_data->m_yScale;
            Colorb clr(0, 0, 0, 255);
			m_data->m_canvasInterface->setPixel(m_data->m_canvasIndex, m_data->m_width - 1,
														yPos, clr);
			m_data->m_canvasInterface->setPixel(m_data->m_canvasIndex, m_data->m_width - 1,
														yNeg, clr);
		}
	}

	if (!m_data->m_bar)
	{
		char buf[1024];
		float time = m_data->getTime();
		sprintf(buf, "%2.0f", time);
        Colorb clr(0, 0, 0, 255);
		grapicalPrintf(buf, m_data->m_width - 25, m_data->m_zero + 3, clr);

		m_data->m_bar = m_data->m_ticksPerSecond;
	}
	m_data->m_timeTicks++;

	m_data->m_bar--;
}

void TimeSeriesCanvas::insertDataAtCurrentTime(float orgV, int dataSourceIndex, bool connectToPrevious)
{
	if (0 == m_data->m_canvasInterface)
		return;

	b3Assert(dataSourceIndex < m_data->m_dataSources.size());

	float zero = m_data->m_zero;
	float amp = m_data->m_pixelsPerUnit;
	//insert some new value(s) in the right-most column
	{
		//	float time = m_data->getTime();
        Colorb clr(m_data->m_dataSources[dataSourceIndex].m_red,
            m_data->m_dataSources[dataSourceIndex].m_green,
            m_data->m_dataSources[dataSourceIndex].m_blue,
            255);
		float v = zero + amp * orgV;

		m_data->m_canvasInterface->setPixel(m_data->m_canvasIndex, m_data->m_width - 1, v, clr);

		if (connectToPrevious && m_data->m_dataSources[dataSourceIndex].m_hasLastValue)
		{
			for (int value = m_data->m_dataSources[dataSourceIndex].m_lastValue; value <= v; value++)
			{
				if (value >= 0 && value < float(m_data->m_height - 1))
				{
					m_data->m_canvasInterface->setPixel(m_data->m_canvasIndex, m_data->m_width - 1, value, clr);
				}
			}

			for (int value = v; value <= m_data->m_dataSources[dataSourceIndex].m_lastValue; value++)
			{
				if (value >= 0 && value < float(m_data->m_height - 1))
				{
					m_data->m_canvasInterface->setPixel(m_data->m_canvasIndex, m_data->m_width - 1, value, clr);
				}
			}
		}
		m_data->m_dataSources[dataSourceIndex].m_lastValue = v;
		m_data->m_dataSources[dataSourceIndex].m_hasLastValue = true;
	}
}
void TimeSeriesCanvas::nextTick()
{
	shift1PixelToLeft();
	m_data->m_canvasInterface->refreshImageData(m_data->m_canvasIndex);
}
