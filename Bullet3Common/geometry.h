#ifndef __GEOMETRY_H__
#define __GEOMETRY_H__
#define  _USE_MATH_DEFINES
 
#include <assert.h>
#include <math.h>
#include <vector>
#include <exception>
//#include <algorithm>
#ifdef _MSC_VER
#undef min
#undef max
#endif


template<typename T>
inline T sqr(const T& x)
{
    return x * x;
}

template<typename T>
inline T cube(const T& x)
{
    return x * x*x;
}

template<typename T>
inline T min(T a1, T a2)
{
    return (a1) <= (a2) ? (a1) : (a2);
}

template<typename T>
inline T min(T a1, T a2, T a3)
{
    return min(a1, min(a2, a3));
}

template<typename T>
inline T min(T a1, T a2, T a3, T a4)
{
    return min(min(a1, a2), min(a3, a4));
}

template<typename T>
inline T min(T a1, T a2, T a3, T a4, T a5)
{
    return min(min(a1, a2), min(a3, a4), a5);
}

template<typename T>
inline T min(T a1, T a2, T a3, T a4, T a5, T a6)
{
    return min(min(a1, a2), min(a3, a4), min(a5, a6));
}

template<typename T>
inline T max(T a1, T a2)
{
    return (a1) >= (a2) ? (a1) : (a2);
}

template<typename T>
inline T max(T a1, T a2, T a3)
{
    return max(a1, max(a2, a3));
}

template<typename T>
inline T max(T a1, T a2, T a3, T a4)
{
    return max(max(a1, a2), max(a3, a4));
}

template<typename T>
inline T max(T a1, T a2, T a3, T a4, T a5)
{
    return max(max(a1, a2), max(a3, a4), a5);
}

template<typename T>
inline T max(T a1, T a2, T a3, T a4, T a5, T a6)
{
    return max(max(a1, a2), max(a3, a4), max(a5, a6));
}

template<typename T>
inline void minmax(T a1, T a2, T& amin, T& amax)
{
    if (a1 < a2) {
        amin = a1;
        amax = a2;
    }
    else {
        amin = a2;
        amax = a1;
    }
}

template<typename T>
inline void minmax(T a1, T a2, T a3, T& amin, T& amax)
{
    if (a1 < a2) {
        if (a1 < a3) {
            amin = a1;
            if (a2 < a3) amax = a3;
            else amax = a2;
        }
        else {
            amin = a3;
            if (a1 < a2) amax = a2;
            else amax = a1;
        }
    }
    else {
        if (a2 < a3) {
            amin = a2;
            if (a1 < a3) amax = a3;
            else amax = a1;
        }
        else {
            amin = a3;
            amax = a1;
        }
    }
}

template<typename T>
inline void minmax(T a1, T a2, T a3, T a4, T& amin, T& amax)
{
    if (a1 < a2) {
        if (a3 < a4) {
            amin = min(a1, a3);
            amax = max(a2, a4);
        }
        else {
            amin = min(a1, a4);
            amax = max(a2, a3);
        }
    }
    else {
        if (a3 < a4) {
            amin = min(a2, a3);
            amax = max(a1, a4);
        }
        else {
            amin = min(a2, a4);
            amax = max(a1, a3);
        }
    }
}

template<typename T>
inline void minmax(T a1, T a2, T a3, T a4, T a5, T& amin, T& amax)
{
    //@@@ the logic could be shortcircuited a lot!
    amin = min(a1, a2, a3, a4, a5);
    amax = max(a1, a2, a3, a4, a5);
}

template<typename T>
inline void minmax(T a1, T a2, T a3, T a4, T a5, T a6, T& amin, T& amax)
{
    //@@@ the logic could be shortcircuited a lot!
    amin = min(a1, a2, a3, a4, a5, a6);
    amax = max(a1, a2, a3, a4, a5, a6);
}

template<typename T>
inline void update_minmax(T a1, T& amin, T& amax)
{
    if (a1 < amin) amin = a1;
    else if (a1 > amax) amax = a1;
}

template<typename T>
inline T clamp(T a, T lower, T upper)
{
    if (a < lower) return lower;
    else if (a > upper) return upper;
    else return a;
}

template <size_t DimCols, size_t DimRows, typename T>
class mat;

template <size_t N, typename T>
struct vec
{
    typedef typename T value_type;
	vec()
	{
		for (size_t i = N; i--; u[i] = T())
			;
	}
	T& operator[](const size_t i)
	{
		assert(i < N);
		return u[i];
	}
	const T& operator[](const size_t i) const
	{
		assert(i < N);
		return u[i];
	}

    explicit vec<N, T>(T value_for_all)
    {
        for (unsigned int i = 0; i < N; ++i) u[i] = value_for_all;
    }

    template<class S>
    explicit vec<N, T>(const S *source)
    {
        for (unsigned int i = 0; i < N; ++i) u[i] = (T)source[i];
    }

    template <class S>
    explicit vec<N, T>(const vec<N, S>& source)
    {
        for (unsigned int i = 0; i < N; ++i) u[i] = (T)source[i];
    }

    vec<N, T>(T v0, T v1)
    {
        assert(N == 2);
        u[0] = v0; u[1] = v1;
    }

    vec<N, T>(T v0, T v1, T v2)
    {
        assert(N == 3);
        u[0] = v0; u[1] = v1; u[2] = v2;
    }

    vec<N, T>(T v0, T v1, T v2, T v3)
    {
        assert(N == 4);
        u[0] = v0; u[1] = v1; u[2] = v2; u[3] = v3;
    }

    vec<N, T>(T v0, T v1, T v2, T v3, T v4)
    {
        assert(N == 5);
        u[0] = v0; u[1] = v1; u[2] = v2; u[3] = v3; u[4] = v4;
    }

    vec<N, T>(T v0, T v1, T v2, T v3, T v4, T v5)
    {
        assert(N == 6);
        u[0] = v0; u[1] = v1; u[2] = v2; u[3] = v3; u[4] = v4; u[5] = v5;
    }


    bool nonzero(void) const
    {
        for (unsigned int i = 0; i < N; ++i) if (u[i]) return true;
        return false;
    }

    vec<N, T> operator+=(const vec<N, T> &w)
    {
        for (unsigned int i = 0; i < N; ++i) u[i] += w[i];
        return *this;
    }

    vec<N, T> operator+(const vec<N, T> &w) const
    {
        vec<N, T> sum(*this);
        sum += w;
        return sum;
    }

    vec<N, T> operator-=(const vec<N, T> &w)
    {
        for (unsigned int i = 0; i < N; ++i) u[i] -= w[i];
        return *this;
    }

    vec<N, T> operator-(void) const // unary minus
    {
        vec<N, T> negative;
        for (unsigned int i = 0; i < N; ++i) negative.u[i] = -u[i];
        return negative;
    }

    vec<N, T> operator-(const vec<N, T> &w) const // (binary) subtraction
    {
        vec<N, T> diff(*this);
        diff -= w;
        return diff;
    }

    vec<N, T> operator*=(T a)
    {
        for (unsigned int i = 0; i < N; ++i) u[i] *= a;
        return *this;
    }

    vec<N, T> operator*(T a) const
    {
        vec<N, T> w(*this);
        w *= a;
        return w;
    }

    vec<N, T> operator*=(const vec<N, T> &w)
    {
        for (unsigned int i = 0; i < N; ++i) u[i] *= w.u[i];
        return *this;
    }

    vec<N, T> operator*(const vec<N, T> &w) const
    {
        vec<N, T> componentwise_product;
        for (unsigned int i = 0; i < N; ++i) componentwise_product[i] = u[i] * w.u[i];
        return componentwise_product;
    }

    vec<N, T> operator/=(T a)
    {
        for (unsigned int i = 0; i < N; ++i) u[i] /= a;
        return *this;
    }

    vec<N, T> operator/(T a) const
    {
        vec<N, T> w(*this);
        w /= a;
        return w;
    }


     
    T norm2()const
    {
        T l = sqr(u[0]);
        for (unsigned int i = 1; i < N; ++i) l += sqr(u[i]);
        return l;
    }
     
    T norm()const
    {
        return (T)sqrt(norm2());
    }
     
    inline T dist2(const vec<N, T> &b)const
    {
        T d = sqr(u[0] - b.u[0]);
        for (unsigned int i = 1; i < N; ++i) d += sqr(u[i] - b.u[i]);
        return d;
    }
     
    inline T dist(const vec<N, T> &b)const
    {
        return sqrt(dist2(b));
    }
     
    inline void normalize( )
    { 
        *this /= norm();
    }
     
    inline vec<N, T> normalized()const
    {
        return *this / norm();
    }
     
    inline T infnorm()const
    {
        T d = fabs(u[0]);
        for (unsigned int i = 1; i < N; ++i) d = max(fabs(u[i]), d);
        return d;
    }
 
    void zero()
    {
        for (unsigned int i = 0; i < N; ++i)
            u[i] = 0;
    }

     
    inline T min()const
    {
        T m = u[0];
        for (unsigned int i = 1; i < N; ++i) if (u[i] < m) m = u[i];
        return m;
    }

     
    inline T max()const
    {
        T m = u[0];
        for (unsigned int i = 1; i < N; ++i) if (u[i] > m) m = u[i];
        return m;
    }

 
    inline T dot( const vec<N, T> &b)const
    {
        T d = u[0] * b.u[0];
        for (unsigned int i = 1; i < N; ++i) d += u[i] * b.u[i];
        return d;
    }

    inline bool operator==(  const vec<N, T> &b) const
    {
        bool t = (u[0] == b.u[0]);
        unsigned int i = 1;
        while (i < N && t) {
            t = t && (u[i] == b.u[i]);
            ++i;
        }
        return t;
    }
 
    inline bool operator!=(  const vec<N, T> &b)const
    {
        bool t = (u[0] != b.u[0]);
        unsigned int i = 1;
        while (i < N && !t) {
            t = t || (u[i] != b.u[i]);
            ++i;
        }
        return t;
    }
 

    
    inline vec<N, int> round()const
    {
        vec<N, int> rounded;
        for (unsigned int i = 0; i < N; ++i)
            rounded.u[i] = lround(u[i]);
        return rounded;
    }

    
    inline vec<N, int> floor()const
    {
        vec<N, int> rounded;
        for (unsigned int i = 0; i < N; ++i)
            rounded.u[i] = (int)floor(u[i]);
        return rounded;
    }

    
    inline vec<N, int> ceil()const
    {
        vec<N, int> rounded;
        for (unsigned int i = 0; i < N; ++i)
            rounded.u[i] = (int)ceil(u[i]);
        return rounded;
    }

    
    inline vec<N, T> abs( )const
    {
        vec<N, T> result;
        for (unsigned int i = 0; i < N; ++i)
            result.u[i] = (T)fabs(u[i]);
        return result;
    }
     
 
    inline void update_minmax( vec<N, T> &xmin, vec<N, T> &xmax)const
    {
        for (unsigned int i = 0; i < N; ++i) update_minmax(u[i], xmin[i], xmax[i]);
    }
public:
	T u[N];
};

/////////////////////////////////////////////////////////////////////////////////

template <typename T>
struct vec<2, T>
{
    typedef typename T value_type;
	vec() : x(T()), y(T()) {}
	vec(T X) : x(X), y(X) {}
	vec(T X, T Y) : x(X), y(Y) {}
	template <class U>
	vec<2, T>(const vec<2, U>& v);
    inline void assign(T &a0, T &a1) const
    {
        a0 = u[0]; a1 = u[1];
    }
	T& operator[](const size_t i)
	{
		assert(i < 2);
        return u[i];
	}
	const T& operator[](const size_t i) const
	{
		assert(i < 2);
        return u[i];
	}
    inline bool operator == (const vec<2, T>& v) const { return u[0] == v.u[0] && u[1] == v.u[1]; }

    inline bool operator != (const vec<2, T>& v) const { return u[0] != v.u[0] || u[1] != v.u[1]; }

    inline bool operator <  (const vec<2, T>& v) const
    {
        if (u[0] < v.u[0]) return true;
        else if (u[0] > v.u[0]) return false;
        else return (u[1] < v.u[1]);
    }


    /** not Dot product. */
    inline vec<2, T> operator * (const vec<2, T>& rhs) const
    {
        return vec<2, T>(u[0] * rhs.u[0] , u[1] * rhs.u[1]);
    }

    /** Multiply by scalar. */
    inline const vec<2, T> operator * (T rhs) const
    {
        return vec<2, T>(u[0] * rhs, u[1] * rhs);
    }

    /** Unary multiply by scalar. */
    inline vec<2, T>& operator *= (T rhs)
    {
        u[0] *= rhs;
        u[1] *= rhs;
        return *this;
    }

    /** Divide by scalar. */
    inline const vec<2, T> operator / (T rhs) const
    {
        return vec<2, T>(u[0] / rhs, u[1] / rhs);
    }

    /** Unary divide by scalar. */
    inline vec<2, T>& operator /= (T rhs)
    {
        u[0] /= rhs;
        u[1] /= rhs;
        return *this;
    }

    /** Binary vector add. */
    inline const vec<2, T> operator + (const vec<2, T>& rhs) const
    {
        return vec<2, T>(u[0] + rhs.u[0], u[1] + rhs.u[1]);
    }

    /** Unary vector add. Slightly more efficient because no temporary
      * intermediate object.
    */
    inline vec<2, T>& operator += (const vec<2, T>& rhs)
    {
        u[0] += rhs.u[0];
        u[1] += rhs.u[1];
        return *this;
    }

    /** Binary vector subtract. */
    inline const vec<2, T> operator - (const vec<2, T>& rhs) const
    {
        return vec<2, T>(u[0] - rhs.u[0], u[1] - rhs.u[1]);
    }

    /** Unary vector subtract. */
    inline vec<2, T>& operator -= (const vec<2, T>& rhs)
    {
        u[0] -= rhs.u[0];
        u[1] -= rhs.u[1];
        return *this;
    }

    /** Negation operator. Returns the negative of the vec<2, T>. */
    inline const vec<2, T> operator - () const
    {
        return vec<2, T>(-u[0], -u[1]);
    }
    T norm() const { return sqrt(x * x + y * y); }
    T norm2() const { return (x * x + y * y); }
    vec<2, T> normalized(T l = 1) const
    {
        vec<2, T> vn = (*this) * (l / norm());
        return vn;
    }
    void normalize(T l = 1)
    {
        (*this) *= (l / norm());
    }
    void zero()
    {
        x = y = 0;
    }
    void setValue(T X, T Y)
    {
        x = X; y = Y;
    }
    inline T dot (const vec<2, T>& rhs) const
    {
        return (u[0] * rhs.u[0] + u[1] * rhs.u[1]);
    }
    inline vec<2, T> abs()const
    { 
        return vec<2, T>(fabs(u[0]), fabs(u[1]));
    }

    union
    {
        struct
        {
            T x, y;
        };
        T u[2];
    };
    inline vec<2, T>& operator=(const vec<2, T> &xyz) {   x = xyz.x;  y = xyz.y; return *this; }
    vec(const vec<2, T>& xyz) : x(xyz.x), y(xyz.y) {}
    ~vec<2, T>() {}
};

/////////////////////////////////////////////////////////////////////////////////

template <typename T>
struct vec<3, T>
{
    typedef typename T value_type;
	vec() : x(T()), y(T()), z(T()) {}
	vec(T X) : x(X), y(X), z(X) {}
	vec(T X, T Y, T Z) : x(X), y(Y), z(Z) {}
	vec(const vec<2, T>& xy, T Z) : x(xy.x), y(xy.y), z(Z) {}
	template <class U>
	vec<3, T>(const vec<3, U>& v);
    inline void assign(T &a0, T &a1, T &a2)const
    { 
        a0 = u[0]; a1 = u[1]; a2 = u[2];
    }


	T& operator[](const size_t i)
	{
		assert(i < 3);
        return u[i];
	}
	const T& operator[](const size_t i) const
	{
		assert(i < 3);
        return u[i];
	}
    vec<4, T> to4() const
    {
        return vec<4, T>(x, y, z, 1);
    }
	T norm() const { return sqrt(x * x + y * y + z * z); }
	T norm2() const { return (x * x + y * y + z * z); }
	vec<3, T> normalized(T l = 1) const
	{
        vec<3, T> vn = (*this) * (l / norm());
		return vn;
	}
    void normalize(T l = 1) 
    {
        (*this) *= (l / norm()); 
    }
    inline bool operator == (const vec<3, T>& v) const { return u[0] == v.u[0] && u[1] == v.u[1] && u[2] == v.u[2]; }

    inline bool operator != (const vec<3, T>& v) const { return u[0] != v.u[0] || u[1] != v.u[1] || u[2] != v.u[2]; }

    inline bool operator <  (const vec<3, T>& v) const
    {
        if (u[0] < v.u[0]) return true;
        else if (u[0] > v.u[0]) return false;
        else if (u[1] < v.u[1]) return true;
        else if (u[1] > v.u[1]) return false;
        else return (u[2] < v.u[2]);
    }

    /** not Dot product. */
    inline vec<3, T> operator * (const vec<3, T>& rhs) const
    {
        return vec<3, T>(u[0] * rhs.u[0] , u[1] * rhs.u[1] , u[2] * rhs.u[2]);
    }

    /** Cross product. */
    inline const vec<3, T> operator ^ (const vec<3, T>& rhs) const
    {
        return vec<3, T>(u[1] * rhs.u[2] - u[2] * rhs.u[1],
            u[2] * rhs.u[0] - u[0] * rhs.u[2],
            u[0] * rhs.u[1] - u[1] * rhs.u[0]);
    }

    /** Multiply by scalar. */
    inline const vec<3, T> operator * (T rhs) const
    {
        return vec<3, T>(u[0] * rhs, u[1] * rhs, u[2] * rhs);
    }

    /** Unary multiply by scalar. */
    inline vec<3, T>& operator *= (T rhs)
    {
        u[0] *= rhs;
        u[1] *= rhs;
        u[2] *= rhs;
        return *this;
    }

    /** Divide by scalar. */
    inline const vec<3, T> operator / (T rhs) const
    {
        return vec<3, T>(u[0] / rhs, u[1] / rhs, u[2] / rhs);
    }

    /** Unary divide by scalar. */
    inline vec<3, T>& operator /= (T rhs)
    {
        u[0] /= rhs;
        u[1] /= rhs;
        u[2] /= rhs;
        return *this;
    }

    /** Binary vector add. */
    inline const vec<3, T> operator + (const vec<3, T>& rhs) const
    {
        return vec<3, T>(u[0] + rhs.u[0], u[1] + rhs.u[1], u[2] + rhs.u[2]);
    }

    /** Unary vector add. Slightly more efficient because no temporary
      * intermediate object.
    */
    inline vec<3, T>& operator += (const vec<3, T>& rhs)
    {
        u[0] += rhs.u[0];
        u[1] += rhs.u[1];
        u[2] += rhs.u[2];
        return *this;
    }

    /** Binary vector subtract. */
    inline const vec<3, T> operator - (const vec<3, T>& rhs) const
    {
        return vec<3, T>(u[0] - rhs.u[0], u[1] - rhs.u[1], u[2] - rhs.u[2]);
    }

    /** Unary vector subtract. */
    inline vec<3, T>& operator -= (const vec<3, T>& rhs)
    {
        u[0] -= rhs.u[0];
        u[1] -= rhs.u[1];
        u[2] -= rhs.u[2];
        return *this;
    }

    /** Negation operator. Returns the negative of the vec<3, T>. */
    inline const vec<3, T> operator - () const
    {
        return vec<3, T>(-u[0], -u[1], -u[2]);
    }
    vec<3, T> cross(vec<3, T> v2) const
    {
        return vec<3, T>(y * v2.z - z * v2.y, z * v2.x - x * v2.z, x * v2.y - y * v2.x);
    }
    void zero()
    {
        x = y = z = 0;
    }
    void setValue(T X, T Y, T Z)
    {
        x = X; y = Y; z = Z;
    }
    inline T dot (const vec<3, T>& rhs) const
    {
        return (u[0] * rhs.u[0] + u[1] * rhs.u[1] + u[2] * rhs.u[2]);
    }
    inline vec<3, T> abs()const
    {
        return vec<3, T>(fabs(u[0]), fabs(u[1]), fabs(u[2]));
    }

    union
    {
        struct
        {
            T x, y, z;
        };
        T u[3];
    };
    inline vec<3, T>& operator=(const vec<3, T> &xyz) { x = xyz.x;  y = xyz.y; z = xyz.z; return *this; }
    vec(const vec<3, T>& xyz) : x(xyz.x), y(xyz.y), z(xyz.z) {}
    ~vec<3, T>() {}
};

template <typename T>
struct vec<4, T>
{
    typedef typename T value_type;
    vec() : x(T()), y(T()), z(T()), w(T()) {}
    vec(T X) : x(X), y(X), z(X), w(X) {}
    vec(T X, T Y, T Z, T W) : x(X), y(Y), z(Z), w(W) {}
    vec(const vec<3, T>& xyz, T W) : x(xyz.x), y(xyz.y), z(xyz.z), w(W) {}
    template <class U>
    vec<4, T>(const vec<4, U>& v);

    inline void assign(T &a0, T &a1, T &a2, T &a3) const
    { 
        a0 = u[0]; a1 = u[1]; a2 = u[2]; a3 = u[3];
    }
    T& operator[](const size_t i)
    {
        assert(i < 4);
        return u[i];
    }
    const T& operator[](const size_t i) const
    {
        assert(i < 4);
        return u[i];
    }
    vec<3, T> head3() const
    {
        return vec<3, T>(x, y, z);
    }
    T norm() const { return sqrt(x * x + y * y + z * z + w * w); }
    T norm2() const { return (x * x + y * y + z * z + w * w); }
    vec<4, T> normalized(T l = 1) const
    {
        vec<4, T> vn = (*this) * (l / norm());
        return vn;
    }
    void normalize(T l = 1)
    {
        (*this) *= (l / norm());
    }
    void zero()
    {
        x = y = z = w = 0;
    }

    inline bool operator == (const vec<4, T>& v) const { return u[0] == v.u[0] && u[1] == v.u[1] && u[2] == v.u[2] && u[3] == v.u[3]; }

    inline bool operator != (const vec<4, T>& v) const { return u[0] != v.u[0] || u[1] != v.u[1] || u[2] != v.u[2] || u[3] != v.u[3]; }

    inline bool operator <  (const vec<4, T>& v) const
    {
        if (u[0] < v.u[0]) return true;
        else if (u[0] > v.u[0]) return false;
        else if (u[1] < v.u[1]) return true;
        else if (u[1] > v.u[1]) return false;
        else if (u[2] < v.u[2]) return true;
        else if (u[2] > v.u[2]) return false;
        else return (u[3] < v.u[3]);
    }


    /** not Dot product. */
    inline vec<4, T> operator * (const vec<4, T>& rhs) const
    {
        return vec<4, T>(u[0] * rhs.u[0], u[1] * rhs.u[1],
            u[2] * rhs.u[2], u[3] * rhs.u[3]);
    }

    /** Multiply by scalar. */
    inline vec<4, T> operator * (T rhs) const
    {
        return vec<4, T>(u[0] * rhs, u[1] * rhs, u[2] * rhs, u[3] * rhs);
    }

    /** Unary multiply by scalar. */
    inline vec<4, T>& operator *= (T rhs)
    {
        u[0] *= rhs;
        u[1] *= rhs;
        u[2] *= rhs;
        u[3] *= rhs;
        return *this;
    }

    /** Divide by scalar. */
    inline vec<4, T> operator / (T rhs) const
    {
        return vec<4, T>(u[0] / rhs, u[1] / rhs, u[2] / rhs, u[3] / rhs);
    }

    /** Unary divide by scalar. */
    inline vec<4, T>& operator /= (T rhs)
    {
        u[0] /= rhs;
        u[1] /= rhs;
        u[2] /= rhs;
        u[3] /= rhs;
        return *this;
    }

    /** Binary vector add. */
    inline vec<4, T> operator + (const vec<4, T>& rhs) const
    {
        return vec<4, T>(u[0] + rhs.u[0], u[1] + rhs.u[1],
            u[2] + rhs.u[2], u[3] + rhs.u[3]);
    }

    /** Unary vector add. Slightly more efficient because no temporary
      * intermediate object.
    */
    inline vec<4, T>& operator += (const vec<4, T>& rhs)
    {
        u[0] += rhs.u[0];
        u[1] += rhs.u[1];
        u[2] += rhs.u[2];
        u[3] += rhs.u[3];
        return *this;
    }

    /** Binary vector subtract. */
    inline vec<4, T> operator - (const vec<4, T>& rhs) const
    {
        return vec<4, T>(u[0] - rhs.u[0], u[1] - rhs.u[1],
            u[2] - rhs.u[2], u[3] - rhs.u[3]);
    }

    /** Unary vector subtract. */
    inline vec<4, T>& operator -= (const vec<4, T>& rhs)
    {
        u[0] -= rhs.u[0];
        u[1] -= rhs.u[1];
        u[2] -= rhs.u[2];
        u[3] -= rhs.u[3];
        return *this;
    }

    /** Negation operator. Returns the negative of the vec<4, T>. */
    inline const vec<4, T> operator - () const
    {
        return vec<4, T>(-u[0], -u[1], -u[2], -u[3]);
    }
    void setValue(T X, T Y, T Z, T W)
    {
        x = X; y = Y; z = Z; w = W;
    }
    inline T dot (const vec<4, T>& rhs) const
    {
        return u[0] * rhs.u[0] +
            u[1] * rhs.u[1] +
            u[2] * rhs.u[2] +
            u[3] * rhs.u[3];
    }
    inline vec<4, T> abs()const
    {
        return vec<4, T>(fabs(u[0]), fabs(u[1]), fabs(u[2]), fabs(u[3]));
    }
    union
    {
        struct
        {
            T x, y, z, w;
        };
        T u[4];
    };
    inline vec<4, T>& operator=(const vec<4, T> &xyz) { x = xyz.x;  y = xyz.y; z = xyz.z; w = xyz.w; return *this; }
    vec(const vec<4, T>& xyz) : x(xyz.x), y(xyz.y), z(xyz.z), w(xyz.w) {}
    ~vec<4, T>() {}

};
/////////////////////////////////////////////////////////////////////////////////

template<size_t N, typename T>
inline T dist2(const vec<N, T> &a, const vec<N, T> &b)
{
    T d = sqr(a.u[0] - b.u[0]);
    for (unsigned int i = 1; i < N; ++i) d += sqr(a.u[i] - b.u[i]);
    return d;
}

template<size_t N, typename T>
inline T dist(const vec<N, T> &a, const vec<N, T> &b)
{
    return sqrt(dist2(a, b));
}

template <size_t N, typename T>
vec<N, T> operator+(vec<N, T> lhs, const vec<N, T>& rhs)
{
	for (size_t i = N; i--; lhs[i] += rhs[i])
		;
	return lhs;
}

template <size_t N, typename T>
vec<N, T> operator-(vec<N, T> lhs, const vec<N, T>& rhs)
{
	for (size_t i = N; i--; lhs[i] -= rhs[i])
		;
	return lhs;
}

template <size_t N, typename T, typename U>
vec<N, T> operator*(vec<N, T> lhs, const U& rhs)
{
	for (size_t i = N; i--; lhs[i] *= rhs)
		;
	return lhs;
}

template <size_t N, typename T, typename U>
vec<N, T> operator*(const U& rhs, vec<N, T> lhs)
{
    for (size_t i = N; i--; lhs[i] *= rhs)
        ;
    return lhs;
}

template <size_t N, typename T, typename U>
vec<N, T> operator/(vec<N, T> lhs, const U& rhs)
{
	for (size_t i = N; i--; lhs[i] /= rhs)
		;
	return lhs;
}

template <size_t LEN, size_t N, typename T>
vec<LEN, T> embed(const vec<N, T>& v, T fill = 1)
{
	vec<LEN, T> ret;
	for (size_t i = LEN; i--; ret[i] = (i < N ? v[i] : fill))
		;
	return ret;
}

template <size_t LEN, size_t N, typename T>
vec<LEN, T> proj(const vec<N, T>& v)
{
	vec<LEN, T> ret;
	for (size_t i = LEN; i--; ret[i] = v[i])
		;
	return ret;
}
 
/////////////////////////////////////////////////////////////////////////////////


template<typename T>
inline vec<2, T> rotate(const vec<2, T>& a, const T& angle)
{
    T c = cos(angle);
    T s = sin(angle);
    return vec<2, T>(c*a[0] - s * a[1], s*a[0] + c * a[1]); // counter-clockwise rotation
}

// Rotate the point (x,y,z) around the vector (u,u,w)
template<typename T>
inline vec<3, T> rotate(const vec<3, T>& x, const T& angle, const vec<3, T>& u)
{
    T ux = u[0] * x[0];
    T uy = u[0] * x[1];
    T uz = u[0] * x[2];
    T vx = u[1] * x[0];
    T vy = u[1] * x[1];
    T vz = u[1] * x[2];
    T wx = u[2] * x[0];
    T wy = u[2] * x[1];
    T wz = u[2] * x[2];

    T sa = (T)sin(angle);
    T ca = (T)cos(angle);

    return vec<3, T>(u[0] * (ux + vy + wz) + (x[0] * (u[1] * u[1] + u[2] * u[2]) - u[0] * (vy + wz))*ca + (-wy + vz)*sa,
        u[1] * (ux + vy + wz) + (x[1] * (u[0] * u[0] + u[2] * u[2]) - u[1] * (ux + wz))*ca + (wx - uz)*sa,
        u[2] * (ux + vy + wz) + (x[2] * (u[0] * u[0] + u[1] * u[1]) - u[2] * (ux + vy))*ca + (-vx + uy)*sa);

}

template<typename T>
inline vec<2, T> perp(const vec<2, T> &a)
{
    return vec<2, T>(-a.u[1], a.u[0]);
} // counter-clockwise rotation by 90 degrees

template<typename T>
inline T triple(const vec<3, T> &a, const vec<3, T> &b, const vec<3, T> &c)
{
    return a.u[0] * (b.u[1] * c.u[2] - b.u[2] * c.u[1])
        + a.u[1] * (b.u[2] * c.u[0] - b.u[0] * c.u[2])
        + a.u[2] * (b.u[0] * c.u[1] - b.u[1] * c.u[0]);
}

template<size_t N, typename T>
inline vec<N, T> min_union(const vec<N, T> &a, const vec<N, T> &b)
{
    vec<N, T> m;
    for (unsigned int i = 0; i < N; ++i)
        (a.u[i] < b.u[i]) ? m.u[i] = a.u[i] : m.u[i] = b.u[i];
    return m;
}
 
template<size_t N, typename T>
inline vec<N, T> max_union(const vec<N, T> &a, const vec<N, T> &b)
{
    vec<N, T> m;
    for (unsigned int i = 0; i < N; ++i)
        (a.u[i] > b.u[i]) ? m.u[i] = a.u[i] : m.u[i] = b.u[i];
    return m;
}
template <size_t N, typename T>
struct dt
{
	static T det(const mat<N, N, T>& src)
	{
		T ret = 0;
		for (size_t i = N; i--; ret += src[0][i] * src.cofactor(0, i))
			;
		return ret;
	}
};

template <typename T>
struct dt<1, T>
{
	static T det(const mat<1, 1, T>& src)
	{
		return src[0][0];
	}
};

/////////////////////////////////////////////////////////////////////////////////

template <size_t DimRows, size_t DimCols, typename T>
class mat
{
	vec<DimCols, T> rows[DimRows];

public:
	mat() {}
    mat(const T in[DimRows][DimCols]) { from(in); }
    mat(const T in[DimRows*DimCols]) { from(in); }

    mat<DimRows, DimCols, T>(T a0, T a1, T a2, T a3)
    { 
        T arr[] = { a0, a1, a2, a3 };
        from(arr);
    }

    mat<DimRows, DimCols, T>(T a0, T a1, T a2, T a3, T a4, T a5)
    {
        T arr[] = { a0, a1, a2, a3, a4, a5 };
        from(arr);
    }

    mat<DimRows, DimCols, T>(T a0, T a1, T a2, T a3, T a4, T a5, T a6, T a7, T a8)
    {
        T arr[] = { a0, a1, a2, a3, a4, a5, a6, a7, a8 };
        from(arr);
    }

    inline T* ptr() { return rows[0].u; }
    inline const T* ptr() const { return rows[0].u; }
    void from(const T in[DimRows][DimCols])
    {
        for (size_t i = DimRows; i--;)
            for (size_t j = DimCols; j--; rows[i][j] = in[i][j])
                ;
    }
    void from(const T in[DimRows*DimCols])
    {
        int k = 0;
        for (size_t i = 0; i < DimRows; ++i)
            for (size_t j = 0; j < DimCols; ++j)
                rows[i][j] = in[k++];
    }
	vec<DimCols, T>& operator[](const size_t idx)
	{
		assert(idx < DimRows);
		return rows[idx];
	}

	const vec<DimCols, T>& operator[](const size_t idx) const
	{
		assert(idx < DimRows);
		return rows[idx];
	}
    T operator()(const size_t idx, const size_t k) const
    {
        assert(idx < DimRows);
        return rows[idx][k];
    }
    T& operator()(const size_t idx, const size_t k)  
    {
        assert(idx < DimRows);
        return rows[idx][k];
    }
	vec<DimRows, T> col(const size_t idx) const
	{
		assert(idx < DimCols);
		vec<DimRows, T> ret;
		for (size_t i = DimRows; i--; ret[i] = rows[i][idx])
			;
		return ret;
	}

	void set_col(size_t idx, vec<DimRows, T> v)
	{
		assert(idx < DimCols);
		for (size_t i = DimRows; i--; rows[i][idx] = v[i])
			;
	}
    static mat<DimRows, DimCols, T> translate(vec<DimRows - 1, T> v)
    {
        mat<DimRows, DimCols, T> ret = identity();
        ret.setTranslate(v);
        return ret;
    }
    vec<DimRows - 1, T> getTranslate() const
    {
        vec<DimRows - 1, T> v;
        return v;
    }
    void setTranslate(vec<DimRows - 1, T> v)
    {

    }
	static mat<DimRows, DimCols, T> identity()
	{
		mat<DimRows, DimCols, T> ret;
		for (size_t i = DimRows; i--;)
			for (size_t j = DimCols; j--; ret[i][j] = (i == j))
				;
		return ret;
	}
    inline void zero() {
        memset(ptr(), 0, DimRows * DimCols * sizeof(T));
    }

	T det() const
	{
		return dt<DimCols, T>::det(*this);
	}

	mat<DimRows - 1, DimCols - 1, T> get_minor(size_t row, size_t col) const
	{
		mat<DimRows - 1, DimCols - 1, T> ret;
		for (size_t i = DimRows - 1; i--;)
			for (size_t j = DimCols - 1; j--; ret[i][j] = rows[i < row ? i : i + 1][j < col ? j : j + 1])
				;
		return ret;
	}

	T cofactor(size_t row, size_t col) const
	{
		return get_minor(row, col).det() * ((row + col) % 2 ? -1 : 1);
	}

	mat<DimRows, DimCols, T> adjugate() const
	{
		mat<DimRows, DimCols, T> ret;
		for (size_t i = DimRows; i--;)
			for (size_t j = DimCols; j--; ret[i][j] = cofactor(i, j))
				;
		return ret;
	}

	mat<DimRows, DimCols, T> invert_transpose() const
	{
		mat<DimRows, DimCols, T> ret = adjugate();
		T tmp = ret[0].dot(rows[0]);
		return ret / tmp;
	}

	mat<DimRows, DimCols, T> invert() const
	{
		return invert_transpose().transpose();
	}

	mat<DimCols, DimRows, T> transpose() const
	{
		mat<DimCols, DimRows, T> ret;
		for (size_t i = DimCols; i--; ret[i] = this->col(i))
			;
		return ret;
	}

    mat<DimRows, DimCols, T> operator+=(const mat<DimRows, DimCols, T> &b)
    {
        for (unsigned int i = 0; i < DimRows; ++i) rows[i] += b.rows[i];
        return *this;
    }

    mat<DimRows, DimCols, T> operator+(const mat<DimRows, DimCols, T> &b) const
    {
        mat<DimRows, DimCols, T> sum(*this);
        sum += b;
        return sum;
    }

    mat<DimRows, DimCols, T> operator-=(const mat<DimRows, DimCols, T> &b)
    {
        for (unsigned int i = 0; i < DimRows; ++i) rows[i] -= b.rows[i];
        return *this;
    }

    mat<DimRows, DimCols, T> operator-(const mat<DimRows, DimCols, T> &b) const
    {
        mat<DimRows, DimCols, T> diff(*this);
        diff -= b;
        return diff;
    }

    mat<DimRows, DimCols, T> operator*=(T scalar)
    {
        for (unsigned int i = 0; i < DimRows; ++i) rows[i] *= scalar;
        return *this;
    }

    mat<DimRows, DimCols, T> operator*(T scalar) const
    {
        mat<DimRows, DimCols, T> b(*this);
        b *= scalar;
        return b;
    }
 
    vec<DimRows, T> operator*(const vec<DimCols, T>& rhs)const
    {
        vec<DimRows, T> ret;
        for (size_t i = DimRows; i--; ret[i] = rows[i].dot(rhs))
            ;
        return ret;
    }
  
 
    mat<DimRows, DimCols, T> operator/=(T scalar)
    {
        for (unsigned int i = 0; i < DimRows; ++i) rows[i] /= scalar;
        return *this;
    }

    mat<DimRows, DimCols, T> operator/(T scalar) const
    {
        mat<DimRows, DimCols, T> b(*this);
        b /= scalar;
        return b;
    }

    template<unsigned int N, typename T>
    inline void make_identity(mat<N, N, T>& mat)
    {
        memset(mat.a, 0, N*N * sizeof(T));
        for (unsigned int i = 0; i < N; ++i)
            mat.a[(N + 1)*i] = 1;
    }

};

/////////////////////////////////////////////////////////////////////////////////
template <size_t R1, size_t C1, typename T>
bool operator ==(const mat<R1, C1, T>& lhs, const mat<R1, C1, T>& rhs)
{
  
    for (size_t i = R1; i--;) if (lhs[i] != rhs[i])
    {
        return false;
    }
 
    return  true;
}
template <size_t R1, size_t C1, typename T>
bool operator !=(const mat<R1, C1, T>& lhs, const mat<R1, C1, T>& rhs)
{

    for (size_t i = R1; i--;) if (lhs[i] != rhs[i])
    {
        return true;
    }

    return  false;
}
template <size_t DimRows, size_t DimCols, typename T>
void preMultTranslate( mat<DimRows, DimCols, T>& lhs, const vec<DimCols, T>& rhs)
{
    
    //??
    return  ;
}

template <size_t DimRows, size_t DimCols, typename T>
void preMultScale(mat<DimRows, DimCols, T>& lhs, const vec<DimCols, T>& rhs)
{

    //??
    return;
}

template <size_t DimRows, size_t DimCols, typename T>
void postMultTranslate(mat<DimRows, DimCols, T>& lhs, const vec<DimCols, T>& rhs)
{

    //??
    return;
}

template <size_t DimRows, size_t DimCols, typename T>
void postMultScale(mat<DimRows, DimCols, T>& lhs, const vec<DimCols, T>& rhs)
{

    //??
    return;
}

template<unsigned int N, typename T>
inline T trace(mat<N, N, T>& mat)
{
    T t = 0;
    for (unsigned int i = 0; i < N; ++i)
        t += mat.a[(N + 1)*i];
    return t;
}

template<size_t DimRows, size_t DimCols, typename T>
inline mat<DimRows, DimCols, T> operator*(T scalar, const mat<DimRows, DimCols, T> &a)
{
    mat<DimRows, DimCols, T> b(a);
    b *= scalar;
    return b;
}


template <size_t R1, size_t C1, size_t C2, typename T>
mat<R1, C2, T> operator*(const mat<R1, C1, T>& lhs, const mat<C1, C2, T>& rhs)
{
	mat<R1, C2, T> result;
	for (size_t i = R1; i--;)
		for (size_t j = C2; j--; result[i][j] = lhs[i].dot(rhs.col(j)))
			;
	return result;
}


template<size_t DimRows, size_t DimCols, typename T>
inline mat<DimRows, DimCols, T> outer(const vec<DimRows, T> &x, const vec<DimCols, T> &y)
{
    mat<DimRows, DimCols, T> r;
    T *pr = r.ptr();
    for (unsigned int j = 0; j < DimCols; ++j)
        for (unsigned int i = 0; i < DimRows; ++i, ++pr)
            *pr = x[i] * y[j];
    return r;
}

template<typename T>
inline T determinant(const mat<3, 3, T> &m)
{
    return m(0, 0)*(m(2, 2)*m(1, 1) - m(2, 1)*m(1, 2)) -
        m(1, 0)*(m(2, 2)*m(0, 1) - m(2, 1)*m(0, 2)) +
        m(2, 0)*(m(1, 2)*m(0, 1) - m(1, 1)*m(0, 2));
}

template<typename T>
inline mat<2, 2, T> inverse(const mat<2, 2, T> &m)
{
    const T& a = m(0, 0);
    const T& b = m(0, 1);
    const T& c = m(1, 0);
    const T& d = m(1, 1);

    T invdet = 1.0 / (a*d - b * c);
    return mat<2, 2, T>(invdet*d, -invdet * c, -invdet * b, invdet*a);
}


template<typename T>
inline mat<3, 3, T> inverse(const mat<3, 3, T> &m)
{
    T invdet = 1.0 / determinant(m);
    return mat<3, 3, T>(
        invdet*(m(2, 2)*m(1, 1) - m(2, 1)*m(1, 2)), -invdet * (m(2, 2)*m(0, 1) - m(2, 1)*m(0, 2)), invdet*(m(1, 2)*m(0, 1) - m(1, 1)*m(0, 2)),
        -invdet * (m(2, 2)*m(1, 0) - m(2, 0)*m(1, 2)), invdet*(m(2, 2)*m(0, 0) - m(2, 0)*m(0, 2)), -invdet * (m(1, 2)*m(0, 0) - m(1, 0)*m(0, 2)),
        invdet*(m(2, 1)*m(1, 0) - m(2, 0)*m(1, 1)), -invdet * (m(2, 1)*m(0, 0) - m(2, 0)*m(0, 1)), invdet*(m(1, 1)*m(0, 0) - m(1, 0)*m(0, 1))
        );
}



template<typename T>
inline mat<3, 3, T> star_matrix(const vec<3, T> &w)
{
    return mat<3, 3, T>(0, -w.u[2], w.u[1],
        w.u[2], 0, -w.u[0],
        -w.u[1], w.u[0], 0);
}


 
typedef mat<2, 2, double> Mat22d;
typedef mat<2, 2, float>  Mat22f;
typedef mat<2, 2, int>    Mat22i;
typedef mat<3, 2, double> Mat32d;
typedef mat<3, 2, float>  Mat32f;
typedef mat<3, 2, int>    Mat32i;
typedef mat<2, 3, double> Mat23d;
typedef mat<2, 3, float>  Mat23f;
typedef mat<2, 3, int>    Mat23i;
typedef mat<3, 3, double> Mat33d;
typedef mat<3, 3, float>  Mat33f;
typedef mat<3, 3, int>    Mat33i;
typedef mat<4, 4, double> Mat44d;
typedef mat<4, 4, float>  Mat44f;
typedef mat<4, 4, int>    Mat44i;

typedef mat<4, 4, float> Matrixf;
typedef mat<4, 4, double> Matrixd;

/////////////////////////////////////////////////////////////////////////////////

typedef vec<2, float> Vec2f;
typedef vec<2, double> Vec2d;
typedef vec<2, int> Vec2i;
typedef vec<2, unsigned> Vec2ui;
typedef vec<2, short> Vec2s;
typedef vec<2, unsigned short> Vec2us;
typedef vec<2, char>           Vec2c;
typedef vec<2, unsigned char>  Vec2uc;
typedef vec<2, size_t>         Vec2st;

typedef vec<3, float> Vec3f;
typedef vec<3, double> Vec3d;
typedef vec<3, int> Vec3i;
typedef vec<3, unsigned> Vec3ui;
typedef vec<3, short> Vec3s;
typedef vec<3, unsigned short> Vec3us;
typedef vec<3, char>           Vec3c;
typedef vec<3, unsigned char>  Vec3uc;
typedef vec<3, size_t>         Vec3st;

typedef vec<4, float> Vec4f;
typedef vec<4, double> Vec4d;
typedef vec<4, int> Vec4i; 
typedef vec<4, unsigned> Vec4ui; 
typedef vec<4, short> Vec4s; 
typedef vec<4, unsigned short> Vec4us;
typedef vec<4, char>           Vec4c;
typedef vec<4, unsigned char>  Vec4uc;
typedef vec<4, size_t>         Vec4st;


typedef vec<6, double>         Vec6d;
typedef vec<6, float>          Vec6f;
typedef vec<6, unsigned int>   Vec6ui;
typedef vec<6, int>            Vec6i;
typedef vec<6, short>          Vec6s;
typedef vec<6, unsigned short> Vec6us;
typedef vec<6, char>           Vec6c;
typedef vec<6, unsigned char>  Vec6uc;
typedef vec<6, size_t>         Vec6st;

template <size_t N, typename T>
inline void assign(const vec<N,T>& vs, T &a0, T &a1)
{
    assert(N == 2);
    a0 = vs.u[0]; a1 = vs.u[1];
}

template <size_t N, typename T>
inline void assign(const vec<N, T>& vs, T &a0, T &a1, T &a2)
{
    assert(N == 3);
    a0 = vs.u[0]; a1 = vs.u[1]; a2 = vs.u[2];
}

template <size_t N, typename T>
inline void assign(const vec<N, T>& vs, T &a0, T &a1, T &a2, T &a3)
{
    assert(N == 4);
    a0 = vs.u[0]; a1 = vs.u[1]; a2 = vs.u[2]; a3 = vs.u[3];
}

template <size_t N, typename T>
inline void assign(const vec<N, T>& vs, T &a0, T &a1, T &a2, T &a3, T &a4, T &a5)
{
    assert(N == 6);
    a0 = vs.u[0]; a1 = vs.u[1]; a2 = vs.u[2]; a3 = vs.u[3]; a4 = vs.u[4]; a5 = vs.u[5];
}

struct WrongDimensionException : public std::exception { };

template <typename T>
struct vec<-1, T> 
{
public:
    vec<-1, T>() : u(nullptr), Cols(0)
    {
    }

    explicit vec<-1, T>(int dim) :
        u(new T[dim]), Cols(dim)
    {
    }

    T& operator[](int index) { return u[index]; }
    const T& operator[](int index) const { return u[index]; }
    vec<-1, T>& operator+=(const  vec<-1, T>& rhs);
    T normalize();
    T norm() const;
    T norm2() const;
    vec<-1, T>& operator-=(const  vec<-1, T>& rhs);
    vec<-1, T>& operator*=(const T& rhs);
    vec<-1, T>& operator/=(const T& rhs);
    int length() const;
    void resize(int size);
    void clear();
private:
    T* u;
    int Cols;
};


template <typename T>
vec<-1, T> operator+(const vec<-1, T>& lhs, const vec<-1, T>& rhs)
{
    return vec<-1, T>(lhs) += rhs;
}
template <typename T>
vec<-1, T> operator-(const vec<-1, T>& lhs, const vec<-1, T>& rhs)
{
    return vec<-1, T>(lhs) -= rhs;
}
template <typename T>
vec<-1, T> operator*(const vec<-1, T>& lhs, const double& rhs)
{
    return vec<-1, T>(lhs) *= rhs;
}
template <typename T>
vec<-1, T> operator*(const double& lhs, const vec<-1, T>& rhs)
{
    return vec<-1, T>(rhs) *= lhs;
}
template <typename T>
vec<-1, T> operator/(const vec<-1, T>& lhs, const double& rhs)
{
    return vec<-1, T>(lhs) /= rhs;
}
template <typename T>
vec<-1, T> operator-(const vec<-1, T>& lhs)
{
    return vec<-1, T>(lhs) *= -1;
}
template <typename T>
T InnerProduct(const vec<-1, T>& lhs, const vec<-1, T>& rhs)
{
    if (lhs.length() != rhs.length()) throw WrongDimensionException();

    T accum(0);
    int len = lhs.length();
    for (int i = 0; i < len; i++)
    {
        accum += lhs[i] * rhs[i];
    }
    return accum;
}
template <typename T>
vec<-1, T>& vec<-1, T>::operator+=(const vec<-1, T>& rhs)
{
    if (Cols != rhs.Cols) throw WrongDimensionException();

    for (int i = 0; i < Cols; i++)
    {
        u[i] += rhs[i];
    }
    return *this;
}
template <typename T>
T vec<-1, T>::normalize()
{
    double mag = norm();
    if (mag <= 0) return 0;
    //			assert(mag > 0);
    *this /= mag;
    return mag;
}
template <typename T>
T vec<-1, T>::norm() const
{
    double mag = sqrt(norm2());
    return mag;
}
template <typename T>
T vec<-1, T>::norm2() const
{
    double mag2 = 0;
    for (int i = 0; i < Cols; i++)
        mag2 += u[i] * u[i];
    return mag2;
}
template <typename T>
vec<-1, T>& vec<-1, T>::operator-=(const vec<-1, T>& rhs)
{
    if (Cols != rhs.Cols) throw WrongDimensionException();

    for (int i = 0; i < Cols; i++)
    {
        u[i] -= rhs[i];
    }
    return *this;
}
template <typename T>
vec<-1, T>& vec<-1, T>::operator*=(const T& rhs)
{
    for (int i = 0; i < Cols; i++)
    {
        u[i] *= rhs;
    }
    return *this;
}
template <typename T>
vec<-1, T>& vec<-1, T>::operator/=(const T& rhs)
{
    for (int i = 0; i < Cols; i++)
    {
        u[i] /= rhs;
    }
    return *this;
}
template <typename T>
int vec<-1, T>::length() const
{
    return Cols;
}
template <typename T>
void vec<-1, T>::resize(int size)
{
    if (u)
    {
        delete[] u;
    }
    u = new T[size];
    std::fill(u, u + size, 0);
    Cols = size;
}
template <typename T>
void vec<-1, T>::clear()
{
    std::fill(u, u + Cols, 0);
}


typedef vec<-1, float> VecXf;
typedef vec<-1, double> VecXd;
 

#endif  //__GEOMETRY_H__
