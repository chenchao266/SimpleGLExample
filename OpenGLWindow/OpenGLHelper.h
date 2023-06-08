#ifndef OPENGL_HELPER_H
#define OPENGL_HELPER_H

 
#include "Bullet3Common/b3Vector3.h"
#include "OpenGLWindow/OpenGLInclude.h"

struct OpenGLHelper
{
 
    static void    b3CreateFrustum(
        float left,
        float right,
        float bottom,
        float top,
        float nearVal,
        float farVal,
        float frustum[16]);
 

    static void b3Matrix4x4Mul(float aIn[4][4], float bIn[4][4], float result[4][4]);

    static void b3Matrix4x4Mul16(float aIn[16], float bIn[16], float result[16]);

    static void b3CreateDiagonalMatrix(float value, float result[4][4]);

    static void b3CreateOrtho(float left, float right, float bottom, float top, float zNear, float zFar, float result[4][4]);

    static void b3CreateLookAt(const b3Vector3& eye, const b3Vector3& center, const b3Vector3& up, float result[16]);
    
    template <typename T>
    static inline int projectWorldCoordToScreen(T objx, T objy, T objz,
        const T modelMatrix[16],
        const T projMatrix[16],
        const int viewport[4],
        T* winx, T* winy, T* winz)
    {
        int i;
        T in2[4];
        T tmp[4];

        in2[0] = objx;
        in2[1] = objy;
        in2[2] = objz;
        in2[3] = T(1.0);

        for (i = 0; i < 4; i++)
        {
            tmp[i] = in2[0] * modelMatrix[0 * 4 + i] + in2[1] * modelMatrix[1 * 4 + i] +
                in2[2] * modelMatrix[2 * 4 + i] + in2[3] * modelMatrix[3 * 4 + i];
        }

        T out[4];
        for (i = 0; i < 4; i++)
        {
            out[i] = tmp[0] * projMatrix[0 * 4 + i] + tmp[1] * projMatrix[1 * 4 + i] + tmp[2] * projMatrix[2 * 4 + i] + tmp[3] * projMatrix[3 * 4 + i];
        }

        if (out[3] == T(0.0))
            return 0;
        out[0] /= out[3];
        out[1] /= out[3];
        out[2] /= out[3];
        /* Map x, y and z to range 0-1 */
        out[0] = out[0] * T(0.5) + T(0.5);
        out[1] = out[1] * T(0.5) + T(0.5);
        out[2] = out[2] * T(0.5) + T(0.5);

        /* Map x,y to viewport */
        out[0] = out[0] * viewport[2] + viewport[0];
        out[1] = out[1] * viewport[3] + viewport[1];

        *winx = out[0];
        *winy = out[1];
        *winz = out[2];
        return 1;
    }
};


struct caster2
{
    void setInt(int v)
    {
        i = v;
    }
    float getFloat()
    {
        float v = ((float)i) + .25;
        return v;
    }

    union {
        int i;
        float f;
    };
};

#endif  //OPENGL_GUI_HELPER_H
