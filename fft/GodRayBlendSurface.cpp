/*
* This source file is part of the osgOcean library
* 
* Copyright (C) 2009 Kim Bale
* Copyright (C) 2009 The University of Hull, UK
* 
* This program is free software; you can redistribute it and/or modify it under
* the terms of the GNU Lesser General Public License as published by the Free Software
* Foundation; either version 3 of the License, or (at your option) any later
* version.

* This program is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
* FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.
* http://www.gnu.org/copyleft/lesser.txt.
*/

#include "GodRayBlendSurface.h"
#include <Component/CullVisitor.h>
#include <OpenGLWindow/LoadShader.h>
 
struct GodRayBlendProgram :public ProgramBase
{
    GLint  osgOcean_GodRayTexture = -1;
    GLint  osgOcean_Eye = -1;
    GLint  osgOcean_ViewerDir = -1;
    GLint  osgOcean_SunDir = -1;
    GLint  osgOcean_HGg = -1;
    GLint  osgOcean_Intensity = -1;
 
};
GodRayBlendSurface::GodRayBlendSurface( void ):
    _HGg          (1.f-(.2f*.2f), 1.f+(.2f*.2f), 2.f*.2f),
    _intensity    (0.2f),
    _sunDir       (0.f,0.f,-1.f)
{
    
}

GodRayBlendSurface::GodRayBlendSurface( const Vec3f& corner, const Vec2f& dims, TextureRectangle* texture ):
    _HGg        (1.f-(.2f*.2f), 1.f+(.2f*.2f), 2.f*.2f),
    _intensity  (0.2f),
    _sunDir     (0.f,0.f,-1.f)
{
    build(corner,dims,texture);
}

GodRayBlendSurface::GodRayBlendSurface( const GodRayBlendSurface &copy):
    MultiGeometryShape(copy),
    _HGg              (copy._HGg),
    _intensity        (copy._intensity),
    _sunDir           (copy._sunDir),
    _stateset         (copy._stateset),
    _normalArray      (copy._normalArray )
{

}

void GodRayBlendSurface::build( const Vec3f& corner, const Vec2f& dims, TextureRectangle* texture )
{
    removeDrawables( 0, getNumDrawables() );

    GeometryShape* geom = new ScreenAlignedQuad(corner,dims,texture);
     
    _normalArray.reset(new Vec3Array(4));
    geom->setNormalArray(_normalArray.get(), Array::BIND_PER_VERTEX );

    addDrawable(geom);

    createShader();
    m_prog->use();
 
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    ///( 0, texture,);
    m_prog->osgOcean_GodRayTexture = m_prog->getLoc( "osgOcean_GodRayTexture"); //  
    m_prog->setUniform(m_prog->osgOcean_GodRayTexture, 0);
    m_prog->osgOcean_Eye = m_prog->getLoc( "osgOcean_Eye" );          // 
    m_prog->setUniform(m_prog->osgOcean_Eye, Vec3f(0.f, 0.f, 0.f));
    m_prog->osgOcean_ViewerDir = m_prog->getLoc( "osgOcean_ViewerDir" );    //,  
    m_prog->setUniform(m_prog->osgOcean_ViewerDir, Vec3f(0.f, 1.f, 0.f));
    m_prog->osgOcean_SunDir = m_prog->getLoc( "osgOcean_SunDir" );       //, 
    m_prog->setUniform(m_prog->osgOcean_SunDir, Vec3f(0.f, 0.f, -1.f));

    m_prog->osgOcean_HGg = m_prog->getLoc( "osgOcean_HGg");           //,
    m_prog->setUniform(m_prog->osgOcean_HGg, _HGg);
    m_prog->osgOcean_Intensity = m_prog->getLoc( "osgOcean_Intensity");     //  
    m_prog->setUniform(m_prog->osgOcean_Intensity, _intensity);
 
    setUserPointer( new GodRayBlendDataType(*this) );
 
}

#include <shaders/osgOcean_godray_screen_blend_vert.inl>
#include <shaders/osgOcean_godray_screen_blend_frag.inl>

void GodRayBlendSurface::createShader(void)
{
    static const char osgOcean_godray_screen_blend_vert_file[] = "osgOcean_godray_screen_blend.vert";
    static const char osgOcean_godray_screen_blend_frag_file[] = "osgOcean_godray_screen_blend.frag";
    m_prog = new GodRayBlendProgram;
    m_prog->create( osgOcean_godray_screen_blend_vert,      osgOcean_godray_screen_blend_frag );
}

void GodRayBlendSurface::update( const Matrixf& view, const Matrixf& proj )
{
    // Get the corners of the far clipping plane.
    double far     = proj(3,2) / (1.0+proj(2,2));

    double fLeft   = far * (proj(2,0)-1.0) / proj(0,0);
    double fRight  = far * (1.0+proj(2,0)) / proj(0,0);
    double fTop    = far * (1.0+proj(2,1)) / proj(1,1);
    double fBottom = far * (proj(2,1)-1.0) / proj(1,1);

    // multiply plane coords by inverse view matrix and add 
    // to the normal array. These will be used as the ray 
    // vectors for the view direction at the corners of the 
    // screen aligned quad
    Matrixf inv_view = (view).invert();

    (*_normalArray)[0] = (inv_view * Vec4f( fLeft,  fTop,    -far, 1)).head3();
    (*_normalArray)[1] = (inv_view * Vec4f( fLeft,  fBottom, -far, 1)).head3();
    (*_normalArray)[2] = (inv_view * Vec4f( fRight, fBottom, -far, 1)).head3();
    (*_normalArray)[3] = (inv_view * Vec4f( fRight, fTop,    -far, 1)).head3();
}

void GodRayBlendSurface::setEccentricity(float g)
{
    _HGg = Vec3f(1.f - (g*g), 1.f + (g*g), 2.f*g);

     m_prog->setUniform(m_prog->osgOcean_HGg, _HGg);
}

void GodRayBlendSurface::setIntensity(float i)
{
    _intensity = i;

    m_prog->setUniform(m_prog->osgOcean_Intensity, _intensity);
}

void GodRayBlendSurface::setSunDirection(const Vec3f& sunDir)
{
    _sunDir = sunDir;


    m_prog->setUniform(m_prog->osgOcean_SunDir, _sunDir.u, 3);
}

// --------------------------------------------
//          Callback implementations
// --------------------------------------------

GodRayBlendSurface::GodRayBlendDataType::GodRayBlendDataType(GodRayBlendSurface& blendSurface):
    _blendSurface( blendSurface )
{}

GodRayBlendSurface::GodRayBlendDataType::GodRayBlendDataType(const GodRayBlendDataType& copy  ):
    _blendSurface (copy._blendSurface),
    _view         (copy._view),
    _projection   (copy._projection)
{}

void GodRayBlendSurface::GodRayBlendDataType::update(void)
{
    _blendSurface.update(_view,_projection);
}

void GodRayBlendSurface::cull(CullVisitor* cv)
{
    GodRayBlendDataType* data = (GodRayBlendDataType*) ( getUserPointer() );

    if(data)
    {
 
        { 
            // get view and projection matrices for the main view camera
            data->setViewMatrix(*cv->getViewMatrix());
            data->setProjMatrix(*cv->getProjectionMatrix());
        }
 
    }

   
}
void GodRayBlendSurface::update(unsigned frameNum, double currentTime)
{
    GodRayBlendDataType* data =(GodRayBlendDataType*) (getUserPointer());

    if (data)
    {
        {
            data->update();
        }
    }
     
}
