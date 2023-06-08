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

#include "GodRays.h"
#include <Component/Texture.h>
#include <Component/TextureRectangle.h>
#include <Component/Texture2D.h>
#include <Component/CullVisitor.h>
#include <OpenGLWindow/LoadShader.h>
#include "stb_image/stb_image.h"
struct GodRayGlareProgram :public ProgramBase
{
    GLint  osgOcean_GlareTexture = -1;

};
struct GodRayProgram :public ProgramBase
{
    GLint osgOcean_Waves = -1;

    GLint osgOcean_Origin = -1;
    GLint osgOcean_Extinction_c = -1;
    GLint osgOcean_Eye = -1;
    GLint osgOcean_Spacing = -1;
    GLint osgOcean_SunDir = -1;

};
GodRays::GodRays(void)
    :_isDirty         (true)
    ,_isStateDirty    (true)
    ,_numOfRays       (10)
    ,_sunDirection    (0.f,0.f,-1.f)
    ,_extinction      (0.1f,0.1f,0.1f)
    ,_baseWaterHeight (0.f)
{
    setUserPointer( new GodRayDataType(*this) );
 
    //setCullingActive(false);
}

GodRays::GodRays(unsigned int numOfRays, const Vec3f& sunDir, float baseWaterHeight )
    :_isDirty         (true)
    ,_isStateDirty    (true)
    ,_numOfRays       (numOfRays)
    ,_sunDirection    (sunDir)
    ,_extinction      (0.1f,0.1f,0.1f)
    ,_baseWaterHeight (baseWaterHeight)
{
    setUserPointer( new GodRayDataType(*this) );
 
    //setCullingActive(false);
}

GodRays::GodRays(const GodRays& copy):
    MultiGeometryShape       (copy)
    ,_isDirty         (copy._isDirty)
    ,_isStateDirty    (copy._isStateDirty)
    ,_numOfRays       (copy._numOfRays)
    ,_sunDirection    (copy._sunDirection)
    ,_extinction      (copy._extinction)
    ,_baseWaterHeight (copy._baseWaterHeight)
    ,_stateSet        (copy._stateSet)
    ,_constants       (copy._constants)
    ,_trochoids       (copy._trochoids)
{
}


void GodRays::build(void)
{
    removeDrawables( 0, getNumDrawables() );
 
    addDrawable(createRayShafts());

    GeometryShape* glare = createGlareQuad();

    if( glare)
        addDrawable( glare );

    _isDirty = false;
}

void GodRays::buildStateSet(void)
{
    _constants.reset(new FloatArray());

    // reset, create and pack trochoids
    _trochoids = WaterTrochoids(0.05f, 0.25f, 18.f, 1.2f, 1.f, 0.2f );
    _trochoids.createWaves();
    _trochoids.packWaves( _constants.get() );

  
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    m_prog->setUniform(m_prog->osgOcean_Waves, &(*_constants.get())[0], _constants->size() );
     
    m_prog->setUniform(m_prog->osgOcean_Origin, Vec3f());
    m_prog->setUniform(m_prog->osgOcean_Extinction_c, _extinction);
    m_prog->setUniform(m_prog->osgOcean_Eye, Vec3f());
    m_prog->setUniform(m_prog->osgOcean_Spacing, 1.f);
    m_prog->setUniform(m_prog->osgOcean_SunDir, _sunDirection);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
     
    _isStateDirty=false;
}

void GodRays::setSunDirection(const Vec3f& dir)
{
    _sunDirection = dir;
     
    m_prog->setUniform(m_prog->osgOcean_SunDir, _sunDirection);
}

void GodRays::setExtinctionCoeff(const Vec3f& coeff)
{
    _extinction = coeff;

    m_prog->setUniform(m_prog->osgOcean_Extinction_c, _extinction);
}

GeometryShape* GodRays::createRayShafts(void)
{
    GeometryShape* geom = new GeometryShape();

    int gridSize = _numOfRays;

    Vec3Array* vertices = new Vec3Array(gridSize*gridSize*2);
    Vec2Array* texcoords = new Vec2Array(gridSize*gridSize*2);

    int rowLen = gridSize*2;
    float disp = ((float)gridSize-1.f)/2.f;

    // The two sets of vertices are set side to side.
    // columns 0-9 upper set
    // columns 10-19 lower set
    // Assign length of ray to the tex coord for use in the vertex shader.
    for(int r = 0; r < gridSize; r++)
    {
        for(int c = 0; c < gridSize; c++)
        {
            float pos_x = (float)c-disp;
            float pos_y = (float)r-disp;

            int i_0 = idx(c,r,rowLen);

            (*vertices) [i_0] = Vec3f( pos_x, pos_y, 0.f );
            (*texcoords)[i_0] = Vec2f( 0.f, 0.f );

            int i_1 = idx(c+gridSize,r,rowLen);

            (*vertices) [i_1] = Vec3f( pos_x, pos_y, 0.f );
            (*texcoords)[i_1] = Vec2f( 40.f, 40.f );
        }
    }

    geom->setVertexArray( vertices );
    geom->setTexCoordArray(0, texcoords, Array::BIND_PER_VERTEX);

    Vec4Array* colors = new Vec4Array();
    colors->push_back( Vec4f(1.0,1.0,1.0,1.0) );
    geom->setColorArray( colors ,Array::BIND_OVERALL );

    for(int r = 0; r < gridSize-1; r+=2)
    {
        for(int c = 0; c < gridSize-1; c+=2)
        {
            DrawElementsUInt* shaft = 
                new DrawElementsUInt(DrawElements::TRIANGLE_STRIP, 0);

            shaft->push_back( idx(c,            r+1, rowLen) );
            shaft->push_back( idx(c+gridSize,   r+1, rowLen) );

            shaft->push_back( idx(c+1,          r+1, rowLen) );
            shaft->push_back( idx(c+gridSize+1, r+1, rowLen) );

            shaft->push_back( idx(c+1,          r,   rowLen) );
            shaft->push_back( idx(c+gridSize+1, r,   rowLen) );

            geom->addPrimitiveSet( shaft );
        }
    }
     
    createGodRayProgram();
    m_prog->use();
    m_prog->osgOcean_Waves = m_prog->getLoc("osgOcean_Waves");// 

    m_prog->osgOcean_Origin = m_prog->getLoc("osgOcean_Origin");// 
    m_prog->osgOcean_Extinction_c = m_prog->getLoc("osgOcean_Extinction_c");// 
    m_prog->osgOcean_Eye = m_prog->getLoc("osgOcean_Eye");// 
    m_prog->osgOcean_Spacing = m_prog->getLoc("osgOcean_Spacing");// 
    m_prog->osgOcean_SunDir = m_prog->getLoc("osgOcean_SunDir");// 
    // set bounding box as the vertices are displaced in the vertex shader
    // HACK: bounds are set ridiculously big, but could still get culled
    // when we don't want it to.
    BoundingBox box(-2000.f, -2000.f, -2000.f, 2000.f, 2000.f, 0.f);
    geom->setInitialBound(box);
 
    return geom;
}

GeometryShape* GodRays::createGlareQuad(void)
{
    std::shared_ptr<Image> glareImage;
    int x, y, comp, req_comp = 3;//??
    stbi_load("sun_glare.png", &x, &y, &comp, req_comp);

    if( !glareImage )
        return NULL;

    Texture2D* glareTexture = new Texture2D(glareImage.get());
    glareTexture->setInternalFormat(GL_RGB);
    glareTexture->setFilter(Texture2D::MIN_FILTER,Texture2D::LINEAR);
    glareTexture->setFilter(Texture2D::MAG_FILTER,Texture2D::LINEAR);
    glareTexture->setWrap(Texture::WRAP_S, Texture::CLAMP );
    glareTexture->setWrap(Texture::WRAP_T, Texture::CLAMP );

    GeometryShape* geom = new GeometryShape;

    Vec3Array* vertices = new Vec3Array;

    float size = 15.f;

    vertices->push_back( Vec3f(-15.f, -15.f, 0.f) );
    vertices->push_back( Vec3f(-15.f,  15.f, 0.f) );
    vertices->push_back( Vec3f( 15.f,  15.f, 0.f) );
    vertices->push_back( Vec3f( 15.f, -15.f, 0.f) );

    Vec2Array* texCoords = new Vec2Array;

    texCoords->push_back( Vec2f(0.f,0.f) );
    texCoords->push_back( Vec2f(0.f,1.f) );
    texCoords->push_back( Vec2f(1.f,1.f) );
    texCoords->push_back( Vec2f(1.f,0.f) );

    Vec3Array* normals = new Vec3Array;

    normals->push_back( Vec3f(0.f, 0.f, -1.f) );

    Vec4Array* colors = new Vec4Array;

    colors->push_back( Vec4f(1.f,1.f,1.f,1.f) );

    DrawElementsUInt* prim = new DrawElementsUInt(DrawElements::QUADS);

    prim->push_back(0);
    prim->push_back(1);
    prim->push_back(2);
    prim->push_back(3);

    createGodRayGlareProgram();
    ///( 0, glareTexture );
    m_progGlare->osgOcean_GlareTexture = m_progGlare->getLoc("osgOcean_GlareTexture"); //,0 );
    m_progGlare->use();
    m_progGlare->setUniform(m_progGlare->osgOcean_GlareTexture, 0);
    // set bounding box as the vertices are displaced in the vertex shader
    // HACK: bounds are set ridiculously big, but could still get culled
    // when we don't want it to.
    BoundingBox box(-2000.f, -2000.f, -30.f, 2000.f, 2000.f, 0.f);
    geom->setInitialBound(box);
 
    geom->setVertexArray(vertices);
    geom->setTexCoordArray(0,texCoords, Array::BIND_PER_VERTEX);
    geom->setNormalArray(normals, Array::BIND_OVERALL);
    geom->setColorArray(colors, Array::BIND_OVERALL);
    geom->addPrimitiveSet(prim);
     

    return geom;
}

void GodRays::update(float time, const Vec3f& eye, const double& fov)
{
    if(_isDirty)
        build();

    if(_isStateDirty)
        buildStateSet();

    // if eye is below water surface do the updates
    if( eye.z < _baseWaterHeight )
    {
        float tanFOVOver2 = tan(DegreesToRadians( fov / 2.f ) );
        float depth = -eye.z * 2.f;

        float spacing = 0.2 * ( ( depth * tanFOVOver2 ) / (float)_numOfRays );

        Vec3f refracted = refract( 0.75f /* 1/1.333 */, _sunDirection, Vec3f(0.f, 0.f, 1.f) );
        refracted.normalize();
        Vec3f sunPos = eye + refracted * ( _baseWaterHeight-eye.z ) / refracted.z;

        m_prog->setUniform(m_prog->osgOcean_Eye, eye);
        m_prog->setUniform(m_prog->osgOcean_Spacing, spacing);
        m_prog->setUniform(m_prog->osgOcean_Origin, sunPos);

        _trochoids.updateWaves( time/2.0 );
        _trochoids.packWaves( _constants.get() );

        m_prog->setUniform(m_prog->osgOcean_Waves, &(*_constants.get())[0], _constants->size());

        // If the eye isn't contained withing the god ray volume, 
        // we need to recompute the bounds or they get clipped.
        if(!getDrawable(0)->getBound().contains( eye )){
            getDrawable(0)->dirtyBound();
            getDrawable(1)->dirtyBound();
        }
    }
}

Vec3f GodRays::refract( const float ratio, const Vec3f& I, const Vec3f& N )
{
    float n = ratio;
    float n_2 = n*n;

    Vec3f nI = I * n;

    float nIN = nI*N;

    float IN_2 = I*N;
    IN_2 = IN_2 * IN_2;

    return ( N * ( -nIN - sqrt( 1.f - ( n_2*(1.f-IN_2) )  ) ) ) + nI;
}

#include <shaders/osgOcean_godrays_vert.inl>
#include <shaders/osgOcean_godrays_frag.inl>

void GodRays::createGodRayProgram( void )
{
	static const char osgOcean_godrays_vert_file[] = "osgOcean_godrays.vert";
	static const char osgOcean_godrays_frag_file[] = "osgOcean_godrays.frag";
    m_prog = new GodRayProgram;
    m_prog->create(osgOcean_godrays_vert, osgOcean_godrays_frag);
}

#include <shaders/osgOcean_godray_glare_vert.inl>
#include <shaders/osgOcean_godray_glare_frag.inl>

void GodRays::createGodRayGlareProgram( void )
{
	static const char osgOcean_godray_glare_vert_file[] = "osgOcean_godray_glare.vert";
	static const char osgOcean_godray_glare_frag_file[] = "osgOcean_godray_glare.frag";
    m_progGlare = new GodRayGlareProgram;
    m_progGlare->create(osgOcean_godray_glare_vert, osgOcean_godray_glare_frag);
}

// --------------------------------------------
//          Callback implementations
// --------------------------------------------

GodRays::GodRayDataType::GodRayDataType(GodRays& godRays)
    :_godRays( godRays )
    ,_fov    ( 0.0 )
{}

GodRays::GodRayDataType::GodRayDataType( const GodRayDataType& copy)
    :_godRays (copy._godRays)
    ,_eye     (copy._eye)
    ,_fov     (copy._fov)
{}

void GodRays::GodRayDataType::update( float time )
{
    _godRays.update(time, _eye, _fov);
}


void GodRays::cull(CullVisitor* cv)
{
    GodRayDataType* data = (GodRayDataType*) (getUserPointer());

    if (data)
    {
        // If cull visitor update the current eye position
    
        {
            Vec3f eye, centre, up;
        
            cv->getViewMatrixAsLookAt(eye, centre, up);
            data->setEye(eye);

            double fov, aspectRatio, near, far;
            cv->getProjectionMatrixAsPerspective(fov, aspectRatio, near, far);
            data->setFOV(fov);
        }
   
    }

}
void GodRays::update(unsigned frameNum, double currentTime)
{
    GodRayDataType* data =  (GodRayDataType*) (getUserPointer() );

    if(data)
    {
        // If cull visitor update the current eye position
   
            data->update( currentTime );
      
    }
     
}

BoundingBox GodRays::computeBoundingBox() const
{
    GodRays::GodRayDataType* data = static_cast<GodRays::GodRayDataType*> (getUserPointer() );

    BoundingBox bb = getInitialBoundingBox();

    bb.xMin() += data->getEye().x;
    bb.xMax() += data->getEye().x;

    bb.yMin() += data->getEye().y;
    bb.yMax() += data->getEye().y;

    return bb;
}
