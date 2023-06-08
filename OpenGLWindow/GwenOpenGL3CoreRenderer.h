
#ifndef __GWEN_OPENGL3_CORE_RENDERER_H
#define __GWEN_OPENGL3_CORE_RENDERER_H

#include "Gwork/Gwork.h"
#include "Gwork/BaseRender.h"
#include "OpenGLWindow/GLPrimitiveRenderer.h"
#include "OpenGLWindow/OpenGLInclude.h"

struct sth_stash;
#include "fontstash.h"
#include "Gwork/PlatformTypes.h"

//#include "TwFonts.h"
static float extraSpacing = 0.;  //6f;
#include <assert.h>
#include <math.h>

template <class T>
inline void MyClamp(T& a, const T& lb, const T& ub)
{
	if (a < lb)
	{
		a = lb;
	}
	else if (ub < a)
	{
		a = ub;
	}
}
 
struct MyTextureLoader
{
	virtual ~MyTextureLoader()
	{
	}
	virtual void LoadTexture(const Gwk::Texture* pTexture) = 0;
	virtual void FreeTexture(const Gwk::Texture* pTexture) = 0;
};

class GwenOpenGL3CoreRenderer : public Gwk::Renderer::Base
{
	GLPrimitiveRenderer* m_primitiveRenderer;
	float m_currentColor[4];
	float m_yOffset;
	sth_stash* m_font;
	float m_screenWidth;
	float m_screenHeight;
	float m_fontScaling;
	float m_retinaScale;
	bool m_useTrueTypeFont;
	//const CTexFont* m_currentFont;

	GLuint m_fontTextureId;
	MyTextureLoader* m_textureLoader;

public:
	GwenOpenGL3CoreRenderer(Gwk::ResourcePaths& paths, GLPrimitiveRenderer* primRender, sth_stash* font, float screenWidth, float screenHeight, float retinaScale, MyTextureLoader* loader = 0)
		:Gwk::Renderer::Base(paths),
            m_primitiveRenderer(primRender),
		  m_font(font),
		  m_screenWidth(screenWidth),
		  m_screenHeight(screenHeight),
		  m_retinaScale(retinaScale),
		  m_useTrueTypeFont(false),
		  m_textureLoader(loader)
	{
		///only enable true type fonts on Macbook Retina, it looks gorgeous
		//if (retinaScale == 2.0f)
		{
			m_useTrueTypeFont = true;
		}
		m_currentColor[0] = 0.5;
		m_currentColor[1] = 0.5;
		m_currentColor[2] = 0.5;
		m_currentColor[3] = 1.0;

		m_fontScaling = 16.f * m_retinaScale;

		//TwGenerateDefaultFonts();

		//m_currentFont = g_DefaultNormalFont;
		//m_currentFont = g_DefaultNormalFontAA;

		//m_currentFont = g_DefaultLargeFont;
		//m_fontTextureId = BindFont(m_currentFont);
	}
    Gwk::Font::Status LoadFont(const Gwk::Font& font) override
    {
        return Gwk::Font::Status::Loaded;
    }
    void FreeFont(const Gwk::Font& font) override
    {

    }
	virtual ~GwenOpenGL3CoreRenderer()
	{
		//TwDeleteDefaultFonts();
	}
	virtual void Resize(int width, int height)
	{
		m_screenWidth = width;
		m_screenHeight = height;
	}

	virtual void Begin()
	{
		m_yOffset = 0;
		glEnable(GL_BLEND);
		assert(glGetError() == GL_NO_ERROR);

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		assert(glGetError() == GL_NO_ERROR);

		assert(glGetError() == GL_NO_ERROR);

		glDisable(GL_DEPTH_TEST);
		assert(glGetError() == GL_NO_ERROR);
		//glColor4ub(255,0,0,255);

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		//	saveOpenGLState(width,height);//m_glutScreenWidth,m_glutScreenHeight);

		assert(glGetError() == GL_NO_ERROR);

		glDisable(GL_CULL_FACE);

		glDisable(GL_DEPTH_TEST);
		assert(glGetError() == GL_NO_ERROR);

		glEnable(GL_BLEND);

		assert(glGetError() == GL_NO_ERROR);
	}
	virtual void End()
	{
		glDisable(GL_BLEND);
	}

	virtual void StartClip()
	{
		if (1)
			sth_flush_draw(m_font);
		Gwk::Rect rect = ClipRegion();

		// OpenGL's coords are from the bottom left
		// so we need to translate them here.
		{
			GLint view[4];
			glGetIntegerv(GL_VIEWPORT, &view[0]);
			rect.y = view[3] / m_retinaScale - (rect.y + rect.h);
		}

		glScissor(m_retinaScale * rect.x * Scale(), m_retinaScale * rect.y * Scale(), m_retinaScale * rect.w * Scale(), m_retinaScale * rect.h * Scale());
		glEnable(GL_SCISSOR_TEST);
		//glDisable( GL_SCISSOR_TEST );
	};

	virtual void EndClip()
	{
		if (1)
			sth_flush_draw(m_font);
		glDisable(GL_SCISSOR_TEST);
	};

	virtual void SetDrawColor(Gwk::Color color)
	{
		m_currentColor[0] = color.r / 256.f;
		m_currentColor[1] = color.g / 256.f;
		m_currentColor[2] = color.b / 256.f;
		m_currentColor[3] = color.a / 256.f;
	}
	virtual void DrawFilledRect(Gwk::Rect rect)
	{
		//		B3_PROFILE("GWEN_DrawFilledRect");
		Translate(rect);

		m_primitiveRenderer->drawRect(rect.x, rect.y + m_yOffset,
									  rect.x + rect.w, rect.y + rect.h + m_yOffset, m_currentColor);

		//		m_primitiveRenderer->drawTexturedRect2a(rect.x, rect.y+m_yOffset,
		//		rect.x+rect.w, rect.y+rect.h+m_yOffset, m_currentColor,0,0,1,1);
		//		m_yOffset+=rect.h+10;
	}

	void RenderText(const Gwk::Font& pFont, Gwk::Point rasterPos, const Gwk::String& text)
	{
		//		B3_PROFILE("GWEN_RenderText");

		Gwk::String str = (text);
		const char* unicodeText = (const char*)str.c_str();

		Gwk::Rect r;
		r.x = rasterPos.x;
		r.y = rasterPos.y;
		r.w = 0;
		r.h = 0;

		//
		//printf("str = %s\n",unicodeText);
		//int xpos=0;
		//int ypos=0;
		float dx;
          
		if (1)
		{
			float yoffset = 0.f;
			if (m_retinaScale == 2.0f)
			{
				yoffset = -12;
			}
			Translate(r);
			sth_draw_text(m_font, 1, m_fontScaling, r.x, r.y + yoffset,
						  unicodeText, &dx, m_screenWidth, m_screenHeight, false, m_retinaScale);
		}
        else
        {
        }
	}
	Gwk::Point MeasureText(const Gwk::Font& pFont, const Gwk::String& text)
	{
		//		B3_PROFILE("GWEN_MeasureText");
		Gwk::String str = (text);
		const char* unicodeText = (const char*)str.c_str();

		// printf("str = %s\n",unicodeText);
		int xpos = 0;
		int ypos = 0;

 
		float dx = 0;
		if (1)
		{
			sth_draw_text(m_font, 1, m_fontScaling, xpos, ypos,
						  unicodeText, &dx, m_screenWidth, m_screenHeight, true);

			Gwk::Point pt;

			if (m_retinaScale == 2.0f)
			{
				pt.x = dx * Scale() / 2.f;
				pt.y = m_fontScaling / 2 * Scale() + 1;
			}
			else
			{
				pt.x = dx * Scale();
				pt.y = m_fontScaling * Scale() + 1;
			}
			return pt;
		}
		else
		{ 
		}

		return Gwk::Renderer::Base::MeasureText(pFont, text);
	}

	virtual Gwk::Texture::Status LoadTexture(const Gwk::Texture& pTexture)
	{
		if (m_textureLoader)
			m_textureLoader->LoadTexture(&pTexture);
        return Gwk::Texture::Status::Loaded;
	}
	virtual void FreeTexture(const Gwk::Texture& pTexture)
	{
		if (m_textureLoader)
			m_textureLoader->FreeTexture(&pTexture);
	}
    Gwk::TextureData GetTextureData(const Gwk::Texture& texture) const override
    {
        return Gwk::TextureData();
    }
	virtual void DrawTexturedRect(const Gwk::Texture&  pTexture, Gwk::Rect rect, float u1 = 0.0f, float v1 = 0.0f, float u2 = 1.0f, float v2 = 1.0f)
	{
		//		B3_PROFILE("DrawTexturedRect");
		Translate(rect);

		//float eraseColor[4] = {0,0,0,0};
		//m_primitiveRenderer->drawRect(rect.x, rect.y+m_yOffset, rect.x+rect.w, rect.y+rect.h+m_yOffset, eraseColor);

		GLint texHandle = (GLint)pTexture.m_intData;
		//if (!texHandle)
		//	return;

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texHandle);
	 
		assert(glGetError() == GL_NO_ERROR);
         
		static float add = 0.0;
		//add+=1./512.;//0.01;
		Colorf color(1, 1, 1, 1);

		m_primitiveRenderer->drawTexturedRect(rect.x, rect.y + m_yOffset, rect.x + rect.w, rect.y + rect.h + m_yOffset, color, 0 + add, 0, 1 + add, 1, true);

		assert(glGetError() == GL_NO_ERROR);
	}
};
#endif  //__GWEN_OPENGL3_CORE_RENDERER_H
