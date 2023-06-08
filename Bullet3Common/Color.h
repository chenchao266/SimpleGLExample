#ifndef __COLOR__H__
#define __COLOR__H__

#define COLOR_CONV (0.003921568627450980392156862745098f)
#include <assert.h>
#include <stdint.h>

template<typename T>
struct ColorTrait
{
    constexpr static T MAX = 0;
};

template<>
struct ColorTrait<float>
{
    constexpr static float MAX = 1.0F;
};

template<>
struct ColorTrait<unsigned char>
{
    constexpr static unsigned char MAX = 255;
};

template<>
struct ColorTrait<unsigned short>
{
    constexpr static unsigned short MAX = 65535;
};

template <typename T>
class Color
{
public:
	union
	{
		struct
		{
			T r;
			T g;
			T b;
			T a;
		};

		T c[4];
	};

	inline Color() {}
	inline Color(const T f) : r(f), g(f), b(f), a(ColorTrait<T>::MAX) {}
	inline Color(const T _r, const T _g, const T _b) : r(_r), g(_g), b(_b), a(ColorTrait<T>::MAX) {}
	inline Color(const T _r, const T _g, const T _b, const T _a) : r(_r), g(_g), b(_b), a(_a) {}
	inline Color(const T* pfComponent) : r(pfComponent[0]), g(pfComponent[1]), b(pfComponent[2]), a(pfComponent[3]) {}
    Color(const T *p, unsigned char bpp) //: bytespp(bpp)
    {
        if (bpp == 1)
        {
            c[0] = c[1] = c[2] = p[0];
        }
        else
        {
            c[0] = p[0];
            c[1] = p[1];
            c[2] = p[2];
        }

        if (bpp < 4)
        {
            c[3] = ColorTrait<T>::MAX;//??
        }
        else
        {
            c[3] = p[3];
        }
    }
    T& operator[](const size_t i)
    {
        assert(i < 4);
        return c[i];
    }
    const T& operator[](const size_t i) const
    {
        assert(i < 4); 
        return c[i];
    }

	inline Color operator + (const Color& c) const					{return Color(r + c.r, g + c.g, b + c.b, a + c.a);}
	inline Color operator - (const Color& c) const					{return Color(r - c.r, g - c.g, b - c.b, a - c.a);}
	inline Color operator - () const									{return Color(-r, -g, -b, -a);}
	inline Color operator * (const Color& c) const					{return Color(r * c.r, g * c.g, b * c.b, a * c.a);}
	inline Color operator * (const float f) const						{return Color(r * f, g * f, b * f, a * f);}
	inline Color operator / (const Color &c) const					{return Color(r / c.r, g / c.g, b / c.b, a / c.a);}
	inline Color operator / (const float f) const						{return Color(r / f, g / f, b / f, a / f);}
	inline friend Color operator * (const float f, const Color& c)	{return Color(c.r * f, c.g * f, c.b * f, c.a * f);}

	inline Color operator = (const Color& c)	{r = c.r; g = c.g; b = c.b; a = c.a; return *this;}
	inline Color operator += (const Color& c)	{r += c.r; g += c.g; b += c.b; a += c.a; return *this;}
	inline Color operator -= (const Color& c)	{r -= c.r; g -= c.g; b -= c.b; a -= c.a; return *this;}
	inline Color operator *= (const Color& c)	{r *= c.r; g *= c.g; b *= c.b; a *= c.a; return *this;}
	inline Color operator *= (const float f)		{r *= f; g *= f; b *= f; a *= f; return *this;}
	inline Color operator /= (const Color& c)	{r /= c.r; g /= c.g; b /= c.b; a /= c.a; return *this;}
	inline Color operator /= (const float f)		{r /= f; g /= f; b /= f; a /= f; return *this;}

	inline bool operator == (const Color& c) const {if(r != c.r) return false; if(g != c.g) return false; if(b != c.b) return false; return a == c.a;}
	inline bool operator != (const Color& c) const {if(r != c.r) return true; if(g != c.g) return true; if(b != c.b) return true; return a != c.a;}
};
typedef Color<float> Colorf;
typedef Color<unsigned char> Colorb;

template <typename T>
inline Color<T>	ColorNegate(const Color<T>& c)											{return Color<T>(ColorTrait<T>::MAX - c.r, ColorTrait<T>::MAX - c.g, ColorTrait<T>::MAX - c.b, ColorTrait<T>::MAX - c.a);}
 
template <typename T>
inline float ColorBrightness(const Color<T>& c)										{return c.r * 0.299f + c.g *  0.587f + c.b * 0.114f;}

template <typename T>
inline Color<T>	ColorInterpolate(const Color<T>& c1, const Color<T>& c2, const float p)	{return c1 + p * (c2 - c1);}

const Colorf white(1.0f, 1.0f, 1.0f, 1.0f);
const Colorf black(0.0f, 0.0f, 0.0f, 0.0f);

const Colorf red(1.0f, 0.0f, 0.0f, 1.0f);
const Colorf green(0.0f, 1.0f, 0.0f, 1.0f);
const Colorf blue(0.0f, 0.0f, 1.0f, 1.0f);
#endif