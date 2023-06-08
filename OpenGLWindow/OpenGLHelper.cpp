#include "OpenGLHelper.h"
 
void OpenGLHelper::b3CreateFrustum(float left, float right, float bottom, float top, float nearVal, float farVal, float frustum[16])
{
    frustum[0 * 4 + 0] = (float(2) * nearVal) / (right - left);
    frustum[0 * 4 + 1] = float(0);
    frustum[0 * 4 + 2] = float(0);
    frustum[0 * 4 + 3] = float(0);

    frustum[1 * 4 + 0] = float(0);
    frustum[1 * 4 + 1] = (float(2) * nearVal) / (top - bottom);
    frustum[1 * 4 + 2] = float(0);
    frustum[1 * 4 + 3] = float(0);

    frustum[2 * 4 + 0] = (right + left) / (right - left);
    frustum[2 * 4 + 1] = (top + bottom) / (top - bottom);
    frustum[2 * 4 + 2] = -(farVal + nearVal) / (farVal - nearVal);
    frustum[2 * 4 + 3] = float(-1);

    frustum[3 * 4 + 0] = float(0);
    frustum[3 * 4 + 1] = float(0);
    frustum[3 * 4 + 2] = -(float(2) * farVal * nearVal) / (farVal - nearVal);
    frustum[3 * 4 + 3] = float(0);
}

void OpenGLHelper::b3Matrix4x4Mul(float aIn[4][4], float bIn[4][4], float result[4][4])
{
    for (int j = 0; j < 4; j++)
        for (int i = 0; i < 4; i++)
            result[j][i] = aIn[0][i] * bIn[j][0] + aIn[1][i] * bIn[j][1] + aIn[2][i] * bIn[j][2] + aIn[3][i] * bIn[j][3];
}


void OpenGLHelper::b3Matrix4x4Mul16(float aIn[16], float bIn[16], float result[16])
{
    for (int j = 0; j < 4; j++)
        for (int i = 0; i < 4; i++)
            result[j * 4 + i] = aIn[0 * 4 + i] * bIn[j * 4 + 0] + aIn[1 * 4 + i] * bIn[j * 4 + 1] + aIn[2 * 4 + i] * bIn[j * 4 + 2] + aIn[3 * 4 + i] * bIn[j * 4 + 3];
}


void OpenGLHelper::b3CreateDiagonalMatrix(float value, float result[4][4])
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if (i == j)
            {
                result[i][j] = value;
            }
            else
            {
                result[i][j] = 0.f;
            }
        }
    }
}


void OpenGLHelper::b3CreateOrtho(float left, float right, float bottom, float top, float zNear, float zFar, float result[4][4])
{
    b3CreateDiagonalMatrix(1.f, result);

    result[0][0] = 2.f / (right - left);
    result[1][1] = 2.f / (top - bottom);
    result[2][2] = -2.f / (zFar - zNear);
    result[3][0] = -(right + left) / (right - left);
    result[3][1] = -(top + bottom) / (top - bottom);
    result[3][2] = -(zFar + zNear) / (zFar - zNear);
}


void OpenGLHelper::b3CreateLookAt(const b3Vector3& eye, const b3Vector3& center, const b3Vector3& up, float result[16])
{
    b3Vector3 f = (center - eye).normalized();
    b3Vector3 u = up.normalized();
    b3Vector3 s = (f.cross(u)).normalized();
    u = s.cross(f);

    result[0 * 4 + 0] = s.x;
    result[1 * 4 + 0] = s.y;
    result[2 * 4 + 0] = s.z;

    result[0 * 4 + 1] = u.x;
    result[1 * 4 + 1] = u.y;
    result[2 * 4 + 1] = u.z;

    result[0 * 4 + 2] = -f.x;
    result[1 * 4 + 2] = -f.y;
    result[2 * 4 + 2] = -f.z;

    result[0 * 4 + 3] = 0.f;
    result[1 * 4 + 3] = 0.f;
    result[2 * 4 + 3] = 0.f;

    result[3 * 4 + 0] = -s.dot(eye);
    result[3 * 4 + 1] = -u.dot(eye);
    result[3 * 4 + 2] = f.dot(eye);
    result[3 * 4 + 3] = 1.f;
}
 