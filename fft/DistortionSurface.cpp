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

#include "DistortionSurface.h"
#include <OpenGLWindow/LoadShader.h>
struct DistortionSurfaceProgram :public ProgramBase
{
 
    GLint  osgOcean_FrameBuffer = -1;
    GLint  osgOcean_Frequency = -1;
    GLint  osgOcean_Offset = -1;
    GLint  osgOcean_Speed = -1;
    GLint  osgOcean_ScreenRes = -1;
 

};
DistortionSurface::DistortionSurface( void )
    :_angle(0.f)
{
 
}

DistortionSurface::DistortionSurface( const Vec3f& corner, const Vec2f& dims, TextureRectangle* texture )
    :_angle(0.f)
{
    build(corner,dims,texture);
  
}

DistortionSurface::DistortionSurface( const DistortionSurface &copy):
    MultiGeometryShape(copy)
    ,_angle(copy._angle)
{
}

void DistortionSurface::build( const Vec3f& corner, const Vec2f& dims, TextureRectangle* texture )
{
    //std::cout << "DistortionSurface::build()"<< std::endl;

    _angle = 0.f;

    removeDrawables( 0, getNumDrawables() );

    GeometryShape* geom = 
        createTexturedQuadGeometry( corner, Vec3f(dims.x,0.f,0.f), 
                                         Vec3f(0.f,dims.y,0.f), 
                                         texture->getTextureWidth(), texture->getTextureHeight() );
    addDrawable(geom);
     
     createShader();

     m_prog->use();
    //texture->apply
     m_prog->osgOcean_FrameBuffer = m_prog->getLoc("osgOcean_FrameBuffer"); //  
     m_prog->setUniform(m_prog->osgOcean_FrameBuffer, 0);

     m_prog->osgOcean_Frequency = m_prog->getLoc("osgOcean_Frequency"); //  
     m_prog->setUniform(m_prog->osgOcean_Frequency, 2.f);

     m_prog->osgOcean_Offset = m_prog->getLoc("osgOcean_Offset"); //  
     m_prog->setUniform(m_prog->osgOcean_Offset, 0.f);

     m_prog->osgOcean_Speed = m_prog->getLoc("osgOcean_Speed"); //  
     m_prog->setUniform(m_prog->osgOcean_Speed, 1.f);
 
     m_prog->osgOcean_ScreenRes = m_prog->getLoc("osgOcean_ScreenRes"); //  
     m_prog->setUniform(m_prog->osgOcean_ScreenRes, dims);
      
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
     
    setUserPointer( new DistortionDataType(*this) );
 
}

#include <shaders/osgOcean_water_distortion_vert.inl>
#include <shaders/osgOcean_water_distortion_frag.inl>

void DistortionSurface::createShader(void)
{
    static const char osgOcean_water_distortion_vert_file[] = "osgOcean_water_distortion.vert";
    static const char osgOcean_water_distortion_frag_file[] = "osgOcean_water_distortion.frag";
    m_prog = new DistortionSurfaceProgram;
    m_prog->create( osgOcean_water_distortion_vert, osgOcean_water_distortion_frag);
}

void DistortionSurface::update( const double& dt )
{
    const float inc = 1.39624444f; //(2PI/4.5f;)

    _angle += inc * dt; 

    if(_angle >= 6.2831f) 
        _angle = 0.f;
    m_prog->setUniform(m_prog->osgOcean_Offset, _angle);
     
}
 

// --------------------------------------------
//          Callback implementations
// --------------------------------------------

DistortionSurface::DistortionDataType::DistortionDataType(DistortionSurface& surface)
    :_surface( surface )
    ,_oldTime(0.0)
    ,_newTime(0.0)
{}

void DistortionSurface::DistortionDataType::update( const double& time )
{
    _oldTime = _newTime;
    _newTime = time;

    _surface.update(_newTime-_oldTime);
}

void DistortionSurface::update (unsigned frameNum, double currentTime)
{
    DistortionSurface::DistortionDataType* data 
        =  (DistortionSurface::DistortionDataType*) (getUserPointer() );

    if(data)
    {
    
        {
            data->update( currentTime );
        }
    }
     
}
