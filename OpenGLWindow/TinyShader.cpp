#include <math.h>
#include <limits>
 
#include "TinyShader.h"
#include "tgaimage.h"
#include "model.h"
#include "Bullet3Common/b3MinMax.h"
#include "Bullet3Common/Color.h"

IShader::~IShader() {}

Matrixf TinyHelper::viewport(int x, int y, int w, int h)
{
	Matrixf Viewport;
	Viewport = Matrixf::identity();
	Viewport[0][3] = x + w / 2.f;
	Viewport[1][3] = y + h / 2.f;
	Viewport[2][3] = .5f;
	Viewport[0][0] = w / 2.f;
	Viewport[1][1] = h / 2.f;
	Viewport[2][2] = .5f;
	return Viewport;
}

Matrixf TinyHelper::projection(float coeff)
{
	Matrixf Projection;
	Projection = Matrixf::identity();
	Projection[3][2] = coeff;
	return Projection;
}

Matrixf TinyHelper::lookat(Vec3f eye, Vec3f center, Vec3f up)
{
	Vec3f f = (center - eye).normalized();
	Vec3f u = up.normalized();
	Vec3f s =  (f).cross(u).normalized();
	u =  (s).cross( f);

	Matrixf ModelView;
	ModelView[0][0] = s.x;
	ModelView[0][1] = s.y;
	ModelView[0][2] = s.z;

	ModelView[1][0] = u.x;
	ModelView[1][1] = u.y;
	ModelView[1][2] = u.z;

	ModelView[2][0] = -f.x;
	ModelView[2][1] = -f.y;
	ModelView[2][2] = -f.z;

	ModelView[3][0] = 0.f;
	ModelView[3][1] = 0.f;
	ModelView[3][2] = 0.f;

	ModelView[0][3] = -(s[0] * eye[0] + s[1] * eye[1] + s[2] * eye[2]);
	ModelView[1][3] = -(u[0] * eye[0] + u[1] * eye[1] + u[2] * eye[2]);
	ModelView[2][3] = f[0] * eye[0] + f[1] * eye[1] + f[2] * eye[2];
	ModelView[3][3] = 1.f;

	return ModelView;
}

Vec3f TinyHelper::barycentric(Vec2f A, Vec2f B, Vec2f C, Vec2f P)
{
	Vec3f s[2];
	for (int i = 2; i--;)
	{
		s[i][0] = C[i] - A[i];
		s[i][1] = B[i] - A[i];
		s[i][2] = A[i] - P[i];
	}
	Vec3f u =  (s[0]).cross(s[1]);
	if (std::abs(u[2]) > 1e-2)  // dont forget that u[2] is integer. If it is zero then triangle ABC is degenerate
		return Vec3f(1.f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z);
	return Vec3f(-1, 1, 1);  // in this case generate negative coordinates, it will be thrown away by the rasterizator
}

void TinyHelper::triangleClipped(mat<4, 3, float> &clipc, mat<4, 3, float> &orgClipc, IShader &shader, TGAImage &image, float *zbuffer, const Matrixf &viewPortMatrix)
{
	triangleClipped(clipc, orgClipc, shader, image, zbuffer, 0, viewPortMatrix, 0);
}

void TinyHelper::triangleClipped(mat<4, 3, float> &clipc, mat<4, 3, float> &orgClipc, IShader &shader, TGAImage &image, float *zbuffer, int *segmentationMaskBuffer, const Matrixf &viewPortMatrix, int objectAndLinkIndex)
{
	mat<3, 4, float> screenSpacePts = (viewPortMatrix * clipc).transpose();  // transposed to ease access to each of the points

	mat<3, 2, float> pts2;
	for (int i = 0; i < 3; i++)
	{
		pts2[i] = proj<2>(screenSpacePts[i] / screenSpacePts[i][3]);
	}

	Vec2f bboxmin(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
	Vec2f bboxmax(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());
	Vec2f clamp(image.get_width() - 1, image.get_height() - 1);

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			bboxmin[j] = b3Max(0.f, b3Min(bboxmin[j], pts2[i][j]));
			bboxmax[j] = b3Min(clamp[j], b3Max(bboxmax[j], pts2[i][j]));
		}
	}

	Vec2i P;
	Colorb color;

	mat<3, 4, float> orgScreenSpacePts = (viewPortMatrix * orgClipc).transpose();  // transposed to ease access to each of the points

	mat<3, 2, float> orgPts2;
	for (int i = 0; i < 3; i++)
	{
		orgPts2[i] = proj<2>(orgScreenSpacePts[i] / orgScreenSpacePts[i][3]);
	}

	for (P.x = bboxmin.x; P.x <= bboxmax.x; P.x++)
	{
		for (P.y = bboxmin.y; P.y <= bboxmax.y; P.y++)
		{
			float frag_depth = 0;
			{
				Vec3f bc_screen = barycentric(pts2[0], pts2[1], pts2[2], P);
				Vec3f bc_clip = Vec3f(bc_screen.x / screenSpacePts[0][3], bc_screen.y / screenSpacePts[1][3], bc_screen.z / screenSpacePts[2][3]);
				bc_clip = bc_clip / (bc_clip.x + bc_clip.y + bc_clip.z);
				frag_depth = -1 * (clipc[2]) .dot( bc_clip);

				if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0 ||
					zbuffer[P.x + P.y * image.get_width()] > frag_depth)
					continue;
			}

			Vec3f bc_screen2 = barycentric(orgPts2[0], orgPts2[1], orgPts2[2], P);
			Vec3f bc_clip2 = Vec3f(bc_screen2.x / orgScreenSpacePts[0][3], bc_screen2.y / orgScreenSpacePts[1][3], bc_screen2.z / orgScreenSpacePts[2][3]);
			bc_clip2 = bc_clip2 / (bc_clip2.x + bc_clip2.y + bc_clip2.z);
			float frag_depth2 = -1 * (orgClipc[2]) .dot( bc_clip2);

			bool discard = shader.fragment(bc_clip2, color);

			if (!discard)
			{
				zbuffer[P.x + P.y * image.get_width()] = frag_depth;
				if (segmentationMaskBuffer)
				{
					segmentationMaskBuffer[P.x + P.y * image.get_width()] = objectAndLinkIndex;
				}
				image.set(P.x, P.y, color);
			}
		}
	}
}

void TinyHelper::triangle(mat<4, 3, float> &clipc, IShader &shader, TGAImage &image, float *zbuffer, const Matrixf &viewPortMatrix)
{
	triangle(clipc, shader, image, zbuffer, 0, viewPortMatrix, 0);
}

void TinyHelper::triangle(mat<4, 3, float> &clipc, IShader &shader, TGAImage &image, float *zbuffer, int *segmentationMaskBuffer, const Matrixf &viewPortMatrix, int objectAndLinkIndex)
{
	mat<3, 4, float> pts = (viewPortMatrix * clipc).transpose();  // transposed to ease access to each of the points

	mat<3, 2, float> pts2;
	for (int i = 0; i < 3; i++) pts2[i] = proj<2>(pts[i] / pts[i][3]);

	Vec2f bboxmin(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
	Vec2f bboxmax(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());
	Vec2f clamp(image.get_width() - 1, image.get_height() - 1);

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			bboxmin[j] = b3Max(0.f, b3Min(bboxmin[j], pts2[i][j]));
			bboxmax[j] = b3Min(clamp[j], b3Max(bboxmax[j], pts2[i][j]));
		}
	}

	Vec2i P;
	Colorb color;
	for (P.x = bboxmin.x; P.x <= bboxmax.x; P.x++)
	{
		for (P.y = bboxmin.y; P.y <= bboxmax.y; P.y++)
		{
			Vec3f bc_screen = barycentric(pts2[0], pts2[1], pts2[2], P);
			Vec3f bc_clip = Vec3f(bc_screen.x / pts[0][3], bc_screen.y / pts[1][3], bc_screen.z / pts[2][3]);
			bc_clip = bc_clip / (bc_clip.x + bc_clip.y + bc_clip.z);
			float frag_depth = -1 * (clipc[2]).dot( bc_clip);
			if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0 ||
				zbuffer[P.x + P.y * image.get_width()] > frag_depth)
				continue;
			bool discard = shader.fragment(bc_clip, color);
			if (frag_depth < -shader.m_farPlane)
				discard = true;
			if (frag_depth > shader.m_nearPlane)
				discard = true;

			if (!discard)
			{
				zbuffer[P.x + P.y * image.get_width()] = frag_depth;
				if (segmentationMaskBuffer)
				{
					segmentationMaskBuffer[P.x + P.y * image.get_width()] = objectAndLinkIndex;
				}
				image.set(P.x, P.y, color);
			}
		}
	}
}

Shader::Shader(Model* model, Vec3f light_dir_local, Vec3f light_color, 
    Matrixf& modelView, Matrixf& lightModelView, Matrixf& projectionMat, Matrixf& modelMat, Matrixf& viewportMat, 
    Vec3f localScaling, const Colorf& colorRGBA, int width, int height, b3AlignedObjectArray<float>* shadowBuffer, 
    float ambient_coefficient /*= 0.6*/, float diffuse_coefficient /*= 0.35*/, float specular_coefficient /*= 0.05*/)
    : m_model(model),
    m_light_dir_local(light_dir_local),
    m_light_color(light_color),
    m_modelMat(modelMat),
    m_modelView1(modelView),
    m_projectionMat(projectionMat),
    m_localScaling(localScaling),
    m_lightModelView(lightModelView),
    m_colorRGBA(colorRGBA),
    m_viewportMat(viewportMat),
    m_ambient_coefficient(ambient_coefficient),
    m_diffuse_coefficient(diffuse_coefficient),
    m_specular_coefficient(specular_coefficient),

    m_shadowBuffer(shadowBuffer),
    m_width(width),
    m_height(height)
{
    m_nearPlane = m_projectionMat.col(3)[2] / (m_projectionMat.col(2)[2] - 1);
    m_farPlane = m_projectionMat.col(3)[2] / (m_projectionMat.col(2)[2] + 1);
    //printf("near=%f, far=%f\n", m_nearPlane, m_farPlane);
    m_invModelMat = m_modelMat.invert_transpose();
    m_projectionModelViewMat = m_projectionMat * m_modelView1;
    m_projectionLightViewMat = m_projectionMat * m_lightModelView;
}

Vec4f Shader::vertex(int iface, int nthvert)
{
    //B3_PROFILE("vertex");
    Vec2f uv = m_model->uv(iface, nthvert);
    varying_uv.set_col(nthvert, uv);
    varying_nrm.set_col(nthvert, proj<3>(m_invModelMat * embed<4>(m_model->normal(iface, nthvert), 0.f)));
    Vec3f unScaledVert = m_model->vert(iface, nthvert);
    Vec3f scaledVert = Vec3f(unScaledVert[0] * m_localScaling[0],
        unScaledVert[1] * m_localScaling[1],
        unScaledVert[2] * m_localScaling[2]);
    Vec4f gl_Vertex = m_projectionModelViewMat * embed<4>(scaledVert);
    varying_tri.set_col(nthvert, gl_Vertex);
    Vec4f world_Vertex = m_modelMat * embed<4>(scaledVert);
    world_tri.set_col(nthvert, world_Vertex);
    Vec4f gl_VertexLightView = m_projectionLightViewMat * embed<4>(scaledVert);
    varying_tri_light_view.set_col(nthvert, gl_VertexLightView);
    return gl_Vertex;
}

bool Shader::fragment(Vec3f bar, Colorb& color)
{
    //B3_PROFILE("fragment");
    Vec4f p = m_viewportMat * (varying_tri_light_view * bar);
    float depth = p[2];
    p = p / p[3];

    float index_x = b3Max(float(0.0), b3Min(float(m_width - 1), p[0]));
    float index_y = b3Max(float(0.0), b3Min(float(m_height - 1), p[1]));
    int idx = int(index_x) + int(index_y) * m_width;                       // index in the shadowbuffer array
    float shadow = 1.0;
    if (m_shadowBuffer && idx >= 0 && idx < m_shadowBuffer->size())
    {
        shadow = 0.8 + 0.2 * (m_shadowBuffer->at(idx) < -depth + 0.05);  // magic coeff to avoid z-fighting
    }
    Vec3f bn = (varying_nrm * bar).normalized();
    Vec2f uv = varying_uv * bar;

    Vec3f reflection_direction = (bn * (bn * m_light_dir_local * 2.f) - m_light_dir_local).normalized();
    float specular = std::pow(b3Max(reflection_direction.z, 0.f),
        m_model->specular(uv));
    float diffuse = b3Max(0.f, bn.dot( m_light_dir_local));

    color = m_model->diffuse(uv);
    color[0] *= m_colorRGBA[0];
    color[1] *= m_colorRGBA[1];
    color[2] *= m_colorRGBA[2];
    color[3] *= m_colorRGBA[3];

    for (int i = 0; i < 3; ++i)
    {
        int orgColor = 0;
        float floatColor = (m_ambient_coefficient * color[i] + shadow * (m_diffuse_coefficient * diffuse + m_specular_coefficient * specular) * color[i] * m_light_color[i]);
        if (floatColor == floatColor)
        {
            orgColor = int(floatColor);
        }
        color[i] = b3Min(orgColor, 255);
    }

    return false;
}

DepthShader::DepthShader(Model* model, Matrixf& lightModelView, Matrixf& projectionMat, Matrixf& modelMat, Vec3f localScaling, float lightDistance) : m_model(model),
m_modelMat(modelMat),
m_projectionMat(projectionMat),
m_localScaling(localScaling),
m_lightModelView(lightModelView),
m_lightDistance(lightDistance)
{
    m_nearPlane = m_projectionMat.col(3)[2] / (m_projectionMat.col(2)[2] - 1);
    m_farPlane = m_projectionMat.col(3)[2] / (m_projectionMat.col(2)[2] + 1);

    m_invModelMat = m_modelMat.invert_transpose();
}

Vec4f DepthShader::vertex(int iface, int nthvert)
{
    Vec2f uv = m_model->uv(iface, nthvert);
    varying_uv.set_col(nthvert, uv);
    varying_nrm.set_col(nthvert, proj<3>(m_invModelMat * embed<4>(m_model->normal(iface, nthvert), 0.f)));
    Vec3f unScaledVert = m_model->vert(iface, nthvert);
    Vec3f scaledVert = Vec3f(unScaledVert[0] * m_localScaling[0],
        unScaledVert[1] * m_localScaling[1],
        unScaledVert[2] * m_localScaling[2]);
    Vec4f gl_Vertex = m_projectionMat * m_lightModelView * embed<4>(scaledVert);
    varying_tri.set_col(nthvert, gl_Vertex);
    return gl_Vertex;
}

bool DepthShader::fragment(Vec3f bar, Colorb& color)
{
    Vec4f p = varying_tri * bar;
    float s = p[2] / m_lightDistance;
    b3Clamp(s, 0.0f, 1.0f);
    color = Colorb(255, 255, 255) * s;
    return false;
}
