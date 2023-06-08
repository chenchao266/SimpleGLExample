#ifndef __OUR_GL_H__
#define __OUR_GL_H__

#include "Bullet3Common/geometry.h"
#include "Bullet3Common/Color.h"
#include "Bullet3Common/b3AlignedObjectArray.h"
class TGAImage;
class Model;
 
struct IShader
{
	float m_nearPlane;
	float m_farPlane;
	virtual ~IShader();
	virtual Vec4f vertex(int iface, int nthvert) = 0;
	virtual bool fragment(Vec3f bar, Colorb &color) = 0;
};


struct DepthShader : public IShader
{
    Model* m_model;
    Matrixf& m_modelMat;
    Matrixf m_invModelMat;

    Matrixf& m_projectionMat;
    Vec3f m_localScaling;
    Matrixf& m_lightModelView;
    float m_lightDistance;

    mat<2, 3, float> varying_uv;   // triangle uv coordinates, written by the vertex shader, read by the fragment shader
    mat<4, 3, float> varying_tri;  // triangle coordinates (clip coordinates), written by VS, read by FS

    mat<3, 3, float> varying_nrm;  // normal per vertex to be interpolated by FS

    DepthShader(Model* model, Matrixf& lightModelView, Matrixf& projectionMat, Matrixf& modelMat, Vec3f localScaling, float lightDistance);
    virtual Vec4f vertex(int iface, int nthvert);

    virtual bool fragment(Vec3f bar, Colorb& color);
};

struct Shader : public IShader
{
    Model* m_model;
    Vec3f m_light_dir_local;
    Vec3f m_light_color;
    Matrixf& m_modelMat;
    Matrixf m_invModelMat;
    Matrixf& m_modelView1;
    Matrixf& m_projectionMat;
    Vec3f m_localScaling;
    Matrixf& m_lightModelView;
    Colorf m_colorRGBA;
    Matrixf& m_viewportMat;
    Matrixf m_projectionModelViewMat;
    Matrixf m_projectionLightViewMat;
    float m_ambient_coefficient;
    float m_diffuse_coefficient;
    float m_specular_coefficient;

    b3AlignedObjectArray<float>* m_shadowBuffer;

    int m_width;
    int m_height;

    int m_index;

    mat<2, 3, float> varying_uv;   // triangle uv coordinates, written by the vertex shader, read by the fragment shader
    mat<4, 3, float> varying_tri;  // triangle coordinates (clip coordinates), written by VS, read by FS
    mat<4, 3, float> varying_tri_light_view;
    mat<3, 3, float> varying_nrm;  // normal per vertex to be interpolated by FS
    mat<4, 3, float> world_tri;    // model triangle coordinates in the world space used for backface culling, written by VS

    Shader(Model* model, Vec3f light_dir_local, Vec3f light_color, Matrixf& modelView, Matrixf& lightModelView, Matrixf& projectionMat, Matrixf& modelMat, Matrixf& viewportMat,
        Vec3f localScaling, const Colorf& colorRGBA, int width, int height, b3AlignedObjectArray<float>* shadowBuffer, 
        float ambient_coefficient = 0.6, float diffuse_coefficient = 0.35, float specular_coefficient = 0.05);
    virtual Vec4f vertex(int iface, int nthvert);

    virtual bool fragment(Vec3f bar, Colorb& color);
};

struct TinyHelper
{
   static Matrixf viewport(int x, int y, int w, int h);
   static Matrixf projection(float coeff = 0.f);  // coeff = -1/c
   static Matrixf lookat(Vec3f eye, Vec3f center, Vec3f up);
   static Vec3f barycentric(Vec2f A, Vec2f B, Vec2f C, Vec2f P);
   static void triangle(mat<4, 3, float> &pts, IShader &shader, TGAImage &image, float *zbuffer, const Matrixf &viewPortMatrix);
   static void triangle(mat<4, 3, float> &pts, IShader &shader, TGAImage &image, float *zbuffer, int *segmentationMaskBuffer, const Matrixf &viewPortMatrix, int objectIndex);
   static void triangleClipped(mat<4, 3, float> &clippedPts, mat<4, 3, float> &pts, IShader &shader, TGAImage &image, float *zbuffer, const Matrixf &viewPortMatrix);
   static void triangleClipped(mat<4, 3, float> &clippedPts, mat<4, 3, float> &pts, IShader &shader, TGAImage &image, float *zbuffer, int *segmentationMaskBuffer, const Matrixf &viewPortMatrix, int objectIndex);
};

#endif  //__OUR_GL_H__
