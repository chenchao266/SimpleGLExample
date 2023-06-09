﻿/*
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

#include "FFTOceanSurface.h"
#include "OpenGLWindow/LoadShader.h"
#include <iostream>
#include <sstream>
#include <fstream>
 
struct OceanSurfaceProgram :public ProgramBase
{
    GLint  osgOcean_EnvironmentMap = -1;
    GLint  osgOcean_EnableCrestFoam = -1;
    GLint  osgOcean_FoamCapBottom = -1;
    GLint  osgOcean_FoamCapTop = -1;
    GLint  osgOcean_FoamMap = -1;
    GLint  osgOcean_FoamScale = -1;
    GLint  osgOcean_NoiseMap = -1;
    GLint  osgOcean_NoiseCoords0 = -1;
    GLint  osgOcean_NoiseCoords1 = -1;
    GLint  osgOcean_WaveTop = -1;
    GLint  osgOcean_WaveBot = -1;
    GLint  osgOcean_FresnelMul = -1;
    GLint  osgOcean_FrameTime = -1;

};
FFTOceanSurface::FFTOceanSurface( unsigned int FFTGridSize,
                                  unsigned int resolution,
                                  unsigned int numTiles, 
                                  const Vec2f& windDirection,
                                  float windSpeed,
                                  float depth,
                                  float reflectionDamping,
                                  float waveScale,
                                  bool isChoppy,
                                  float choppyFactor,
                                  float animLoopTime,
                                  unsigned int numFrames )
    :FFTOceanTechnique( FFTGridSize, 
                        resolution, 
                        numTiles, 
                        windDirection, 
                        windSpeed, 
                        depth, 
                        reflectionDamping, 
                        waveScale, 
                        isChoppy, 
                        choppyFactor, 
                        animLoopTime, 
                        numFrames)
    ,_numVertices    ( 0 )
    ,_newNumVertices ( 0 )
    ,_activeVertices ( new Vec3Array )
    ,_activeNormals  ( new Vec3Array )
    ,_totalPoints    ( _tileSize * _numTiles + 1 )
{
    setUserPointer( new OceanDataType(*this, _NUMFRAMES, 25) );
    //setOceanAnimationCallback( new OceanAnimationCallback );
}

FFTOceanSurface::FFTOceanSurface( const FFTOceanSurface& copy):
    FFTOceanTechnique   ( copy )
    ,_numVertices       ( copy._numVertices )
    ,_newNumVertices    ( copy._newNumVertices )
    ,_mipmapGeom        ( copy._mipmapGeom )
    ,_mipmapData        ( copy._mipmapData )
    ,_totalPoints       ( copy._totalPoints )
{}

FFTOceanSurface::~FFTOceanSurface(void)
{
}

void FFTOceanSurface::build( void )
{
    std::cout << "FFTOceanSurface::build()" << std::endl;

    computeSea( _NUMFRAMES );
    createOceanTiles();
    computeVertices(0);
    computePrimitives();

    initStateSet();

    _isDirty =  false;
    _isStateDirty = false;

    std::cout << "FFTOceanSurface::build() Complete." << std::endl;
}

void FFTOceanSurface::initStateSet( void )
{
    std::cout << "FFTOceanSurface::initStateSet()" << std::endl;
 
    // Note that we will only set the textures in the state set if shaders are
    // enabled, otherwise the fixed pipeline will try to put the env map onto
    // the water surface, which has no texture coordinates, so the surface
    // will take the general color of the env map.
    createShader();
    // Environment map    
    m_prog->osgOcean_EnvironmentMap = m_prog->getLoc("osgOcean_EnvironmentMap"); //  
    m_prog->setUniform(m_prog->osgOcean_EnvironmentMap, ENV_MAP);

    //( ENV_MAP, _environmentMap.get() );
    
    // Foam
    m_prog->osgOcean_EnableCrestFoam = m_prog->getLoc("osgOcean_EnableCrestFoam"); //  
    m_prog->setUniform(m_prog->osgOcean_EnableCrestFoam, _useCrestFoam);

    m_prog->osgOcean_FoamCapBottom = m_prog->getLoc("osgOcean_FoamCapBottom"); //  
    m_prog->setUniform(m_prog->osgOcean_FoamCapBottom, _foamCapBottom);

    m_prog->osgOcean_FoamCapTop = m_prog->getLoc("osgOcean_FoamCapTop"); //  
    m_prog->setUniform(m_prog->osgOcean_FoamCapTop, _foamCapTop);

    m_prog->osgOcean_FoamMap = m_prog->getLoc("osgOcean_FoamMap"); //  
    m_prog->setUniform(m_prog->osgOcean_FoamMap, FOAM_MAP);

    m_prog->osgOcean_FoamScale = m_prog->getLoc("osgOcean_FoamScale"); //  
    m_prog->setUniform(m_prog->osgOcean_FoamScale, _tileResInv*30.f);
    if( _useCrestFoam )
    {
        Texture2D* foam_tex = createTexture("sea_foam.png", Texture::REPEAT );
        
            //( FOAM_MAP, foam_tex );
    }

    // Noise
    m_prog->osgOcean_NoiseMap = m_prog->getLoc("osgOcean_NoiseMap"); //  
    m_prog->setUniform(m_prog->osgOcean_NoiseMap, NORMAL_MAP);

    m_prog->osgOcean_NoiseCoords0 = m_prog->getLoc("osgOcean_NoiseCoords0"); //  
    m_prog->setUniform(m_prog->osgOcean_NoiseCoords0, computeNoiseCoords(32.f, Vec2f(2.f, 4.f), 2.f, 0.f));

    m_prog->osgOcean_NoiseCoords1 = m_prog->getLoc("osgOcean_NoiseCoords1"); //  
    m_prog->setUniform(m_prog->osgOcean_NoiseCoords1, computeNoiseCoords(8.f, Vec2f(-4.f, 2.f), 1.f, 0.f));

    std::shared_ptr<Texture2D> noiseMap 
        = createNoiseMap( _noiseTileSize, _noiseWindDir, _noiseWindSpeed, _noiseWaveScale, _noiseTileRes ); 

    //( NORMAL_MAP, noiseMap.get());

    // Colouring
    Vec4f waveTop = colorLerp(_lightColor, Vec4f(), Vec4f(_waveTopColor,1.f) );
    Vec4f waveBot = colorLerp(_lightColor, Vec4f(), Vec4f(_waveBottomColor,1.f) );

    m_prog->osgOcean_WaveTop = m_prog->getLoc("osgOcean_WaveTop"); //  
    m_prog->setUniform(m_prog->osgOcean_WaveTop, waveTop);

    m_prog->osgOcean_WaveBot = m_prog->getLoc("osgOcean_WaveBot"); //  
    m_prog->setUniform(m_prog->osgOcean_WaveBot, waveBot);

    m_prog->osgOcean_FresnelMul = m_prog->getLoc("osgOcean_FresnelMul"); //  
    m_prog->setUniform(m_prog->osgOcean_FresnelMul, _fresnelMul);

    m_prog->osgOcean_FrameTime = m_prog->getLoc("osgOcean_FrameTime"); //  
    m_prog->setUniform(m_prog->osgOcean_FrameTime, 0.0f);
      
    // If shaders are enabled, the final color will be determined by the 
    // shader so we need a white base color. But on the fixed pipeline the
    // material color will determine the ocean surface's color.
 

    _isStateDirty = false;

    std::cout << "FFTOceanSurface::initStateSet() Complete." << std::endl;
}

std::shared_ptr<Texture2D> FFTOceanSurface::createNoiseMap(unsigned int size, 
                                                             const Vec2f& windDir, 
                                                             float windSpeed,                                         
                                                             float waveScale,
                                                             float tileResolution )
{
    std::shared_ptr<FloatArray> heights ( new FloatArray);

    FFTSimulation noiseFFT(size, windDir, windSpeed, _depth, _reflDampFactor, waveScale, tileResolution, 10.f);
    noiseFFT.setTime(0.f);
    noiseFFT.computeHeights(heights.get());
        
    OceanTile oceanTile(heights.get(),size,tileResolution/size);

    return oceanTile.createNormalMap();
}

void FFTOceanSurface::computeSea( unsigned int totalFrames )
{
    std::cout << "FFTOceanSurface::computeSea("<<totalFrames<<")" << std::endl;
    std::cout << "Mipmap Levels: " << _numLevels << std::endl;
    std::cout << "Highest Resolution: " << _tileSize << std::endl;

    FFTSimulation FFTSim( _tileSize, _windDirection, _windSpeed, _depth, _reflDampFactor, _waveScale, _tileResolution, _cycleTime );

    // clear previous mipmaps (if any)
    _mipmapData.clear();
    _mipmapData.resize( totalFrames );

    _averageHeight = 0.f;
    _maxHeight = -FLT_MAX;

    for( unsigned int frame = 0; frame < totalFrames; ++frame )
    {
        std::shared_ptr<FloatArray> heights ( new FloatArray);
        std::shared_ptr<Vec2Array> displacements ( NULL);

        if (_isChoppy)
            displacements.reset(new Vec2Array);

        float time = _cycleTime * ( float(frame) / float(totalFrames) );

        FFTSim.setTime( time );
        FFTSim.computeHeights( heights.get() );

        if(_isChoppy)
            FFTSim.computeDisplacements( _choppyFactor, displacements.get() );

        _mipmapData[frame].resize( _numLevels );

        // Level 0
        _mipmapData[frame][0] = OceanTile( heights.get(), _tileSize, _pointSpacing, displacements.get() );

        _averageHeight += _mipmapData[frame][0].getAverageHeight();

        _maxHeight = b3Max(_maxHeight, _mipmapData[frame][0].getMaximumHeight());

        // Levels 1 -> Max Level
        for(unsigned int level = 1; level < _numLevels-1; ++level )
        {
            OceanTile& lastTile = _mipmapData[frame][level-1];

            _mipmapData[frame][level] = OceanTile( lastTile, _tileSize >> level, _tileSize/(_tileSize>>level)*_pointSpacing );
        }

        // Used for lowest resolution tile
        std::shared_ptr<FloatArray> zeroHeights ( new FloatArray(4));
        zeroHeights->at(0) = 0.f;
        zeroHeights->at(1) = 0.f;
        zeroHeights->at(2) = 0.f;
        zeroHeights->at(3) = 0.f;

        _mipmapData[frame][_numLevels-1] = OceanTile( zeroHeights.get(), 1, _tileSize/(_tileSize>>(_numLevels-1))*_pointSpacing );
    }

    _averageHeight /= (float)totalFrames;

    std::cout << "Average Height: " << _averageHeight << std::endl;
    std::cout << "FFTOceanSurface::computeSea() Complete." << std::endl;
}

void FFTOceanSurface::createOceanTiles( void )
{
    std::cout << "FFTOceanSurface::createOceanTiles()" << std::endl;
    std::cout << "Total tiles: " << _numTiles*_numTiles << std::endl;
    std::cout << "Init level: " << _numLevels-2 << std::endl;

    MipmapGeometry::BORDER_TYPE border = MipmapGeometry::BORDER_NONE;

    // Clear previous data if it exists
    _numVertices = 0;
    _newNumVertices = 0;
    _mipmapGeom.clear();
    _activeVertices->clear();
    _activeNormals->clear();
    _minDist.clear();

    if(getNumDrawables()>0)
        removeDrawables(0,getNumDrawables());

    _mipmapGeom.resize( _numTiles );

    std::shared_ptr<Vec4Array> colours ( new Vec4Array);
    colours->push_back( Vec4f(1.f, 1.f,1.f,1.f) );

    for(int y = 0; y < (int)_numTiles; ++y )
    {
        for(int x = 0; x < (int)_numTiles; ++x )
        {
            if(x == _numTiles-1 && y == _numTiles-1)
                border = MipmapGeometry::BORDER_XY;
            else if(x == _numTiles-1)        
                border = MipmapGeometry::BORDER_X;
            else if(y==_numTiles-1)
                border = MipmapGeometry::BORDER_Y;
            else 
                border = MipmapGeometry::BORDER_NONE;
            
            std::shared_ptr<MipmapGeometry> patch(new MipmapGeometry( _numLevels-2, _numLevels, 0, border ));

            //patch->setUseDisplayList( false );
            patch->setVertexArray( _activeVertices.get() );
            patch->setNormalArray( _activeNormals.get(), Array::BIND_PER_VERTEX );
            patch->setColorArray    ( colours.get(),  Array::BIND_OVERALL );
 
            //patch->setDataVariance( DYNAMIC );
            patch->setIdx( _numVertices );

            addDrawable( patch );

            _mipmapGeom[y].push_back( patch );

            unsigned int verts = 0;
            unsigned int s = 2;

            verts = s * s;

            if(x == _numTiles-1 )                       // If on right border add extra column
                verts += s;
            if(y == _numTiles-1 )                       // If on bottom border add extra row
                verts += s;
            if(x == _numTiles-1 && y == _numTiles-1)    // If corner piece add corner vertex
                verts += 1;

            _numVertices += verts;
        }
    }

    std::cout << "Vertices needed: " << _numVertices << std::endl;

    _activeVertices->resize( _numVertices );
    _activeNormals->resize( _numVertices );

// Correct dMin calculations for geomipmap distances. Not used at the moment
//    float T = (2.0f * TRESHOLD) / VRES;
//    float A = 1.0f / (float)tan(FOV / 2.0f);
//    float C = A / T;

    std::cout << "Minimum Distances: " << std::endl;

    for(unsigned int d = 0; d < _numLevels; ++d)
    {
        _minDist.push_back( d * (float(_tileResolution+1)) + ( float(_tileResolution+1.f)*0.5f ) );
        _minDist.back() *= _minDist.back();
        std::cout << d << ": " << sqrt(_minDist.back()) << std::endl;
    }

    std::cout << "FFTOceanSurface::createOceanTiles() Complete." << std::endl;
}

void FFTOceanSurface::computeVertices( unsigned int frame )
{
    // Only resize vertex/normal arrays if more are needed
    if(_newNumVertices > _numVertices )
    {
        std::cout << "Resizing vertex array from " << _numVertices << "to " << _newNumVertices << std::endl;
        _numVertices = _newNumVertices;
        _activeVertices->resize(_numVertices);
        _activeNormals->resize(_numVertices);
    }

    Vec3f tileOffset,vertexOffset,vertex;
    
    unsigned int ptr = 0;

    const std::vector<OceanTile>& curData = _mipmapData[frame];

    for(unsigned int y = 0; y < _numTiles; ++y )
    {    
        tileOffset.y = _startPos.y - y*_tileResolution;

        for(unsigned int x = 0; x < _numTiles; ++x )
        {
            tileOffset.x = _startPos.x + x*_tileResolution;

            MipmapGeometry* tile = getTile(x,y);
            const OceanTile& data = curData[ tile->getLevel() ];

            for(unsigned int row = 0; row < tile->getColLen(); ++row )
            {
                vertexOffset.y = data.getSpacing()*-float(row) + tileOffset.y;

                for(unsigned int col = 0; col < tile->getRowLen(); ++col )
                {
                    vertexOffset.x = data.getSpacing()*float(col) + tileOffset.x;

                    (*_activeVertices)[ptr] = data.getVertex(col,row) + vertexOffset;
                    (*_activeNormals) [ptr] = data.getNormal(col,row);
                    ++ptr;
                }
            }
        }
    }
}

void FFTOceanSurface::update( unsigned int frame, const double& dt, const Vec3f& eye )
{
    if(_isDirty)
        build();
    else if(_isStateDirty)
        initStateSet();

    if (_isAnimating)
    {
        static double time = 0.0;
        time += (dt * 0.001);      // dt is in milliseconds (see FFTOceanTechnique::OceanDataType::updateOcean() )

        m_prog->setUniform(m_prog->osgOcean_FrameTime, float(time));

        static double noiseTime = 0.0;
        noiseTime += (dt*0.0008);

        m_prog->setUniform(m_prog->osgOcean_NoiseCoords0, computeNoiseCoords(32.f, Vec2f(2.f, 4.f), 2.f, time));
        m_prog->setUniform(m_prog->osgOcean_NoiseCoords1, computeNoiseCoords(8.f, Vec2f(-4.f, 2.f), 1.f, time));

        if( updateMipmaps( eye, frame ) )
        {
            computeVertices( frame );
            computePrimitives();
        }
        else if( frame != _oldFrame )
        {
            computeVertices( frame );
        }
    }

    _oldFrame = frame;
}

float FFTOceanSurface::getSurfaceHeightAt(float x, float y, Vec3f* normal)
{
    if(_isDirty)
        build();

    // Initialize normal so it's in a "known" state if we can't calculate it later.
    if (normal != 0)
    {
        *normal = Vec3f(0, 0, 1);
    }

    // ocean surface coordinates
    float oceanX, oceanY;

    // translate x, y to oceanSurface origin coordinates
    oceanX = -_startPos.x + x;
    oceanY =  _startPos.y - y;

    // calculate the corresponding tile on the ocean surface
    unsigned int ix = oceanX/_tileResolution;
    unsigned int iy = oceanY/_tileResolution;

    unsigned int frame = _oldFrame;

    // Test if the tile is valid 
    if (ix < _numTiles && iy < _numTiles)
    {
        const OceanTile& data = _mipmapData[_oldFrame][0];

        float tile_x = oceanX - ix * _tileResolution;
        float tile_y = oceanY - iy * _tileResolution;

        if (normal != 0)
        {
            *normal = data.normalBiLinearInterp(tile_x, tile_y);
        }

        return data.biLinearInterp(tile_x, tile_y);
    }

    return 0.0f;
}

bool FFTOceanSurface::updateMipmaps( const Vec3f& eye, unsigned int frame )
{
    static unsigned int count = 0;

    bool updated = false;

    _newNumVertices = 0;

    int tileSize = _tileResolution+1;

    int x_offset = 0;
    int y_offset = 0;

    if(_isEndless)
    {
        float xMin = _startPos.x;
        float yMin = _startPos.y - (float)((_tileResolution+1)*_numTiles);

        x_offset = (int) ( (eye.x-xMin) / (float)_tileResolution );
        y_offset = (int) ( (eye.y-yMin) / (float)_tileResolution );

        x_offset -= _numTiles/2;
        y_offset -= _numTiles/2;

        _startPos.x += (float)(x_offset * tileSize); 
        _startPos.y += (float)(y_offset * tileSize); 
    }

    for( unsigned int y = 0; y < _numTiles; ++y)
    {
        for( unsigned int x = 0; x < _numTiles; ++x)
        {
            Vec3f newbound = getTile(x,y)->getBound().center();
            newbound.x += (float)(x_offset * tileSize);
            newbound.y += (float)(y_offset * tileSize);

            Vec3f distanceToTile = newbound - eye;
            
            unsigned int mipmapLevel = 0;

            for( unsigned int m = 0; m < _minDist.size(); ++m )
            {
                if( distanceToTile.norm2() > _minDist.at(m) )
                    mipmapLevel = m;
            }

            if( getTile(x,y)->getLevel() != mipmapLevel )
                updated = true;

            getTile(x,y)->setLevel( mipmapLevel );
            getTile(x,y)->setIdx( _newNumVertices );
            
            unsigned int verts = 0;
            unsigned int size = getTile(x,y)->getResolution();

            verts = size * size;

            if(x == _numTiles-1 )
                verts += size;
            if(y == _numTiles-1 )
                verts += size;
            if(x == _numTiles-1 && y == _numTiles-1)
                verts += 1;

            _newNumVertices += verts;
        }
    }

    return updated;    
}

void FFTOceanSurface::computePrimitives( void )
{
    int x1 = 0;
    int y1 = 0;
    int size = 0;

    std::cout << "FFTOceanSurface::computePrimitives()" << std::endl;

    //debugOut << std::endl;

    for(unsigned int y = 0; y < _numTiles; ++y)
    {
        //std::cout << std::endl;

        for(unsigned int x = 0; x < _numTiles; ++x )
        {
            std::cout <<getTile(x,y)->getLevel() << " ";
            
            x+1 > _numTiles-1 ? x1 = _numTiles-1 : x1 = x+1;
            y+1 > _numTiles-1 ? y1 = _numTiles-1 : y1 = y+1;

            MipmapGeometry* cTile  = getTile(x, y);    // Current tile
            MipmapGeometry* xTile  = getTile(x1,y);    // Right Tile
            MipmapGeometry* yTile  = getTile(x, y1);   // Bottom Tile
            MipmapGeometry* xyTile = getTile(x1,y1);   // Bottom right Tile

            // First clear old primitive sets
            cTile->removePrimitiveSet(0, cTile->getNumPrimitiveSets() );

            if(cTile->getResolution()!=1)
            {
                addMainBody(cTile);

                if( x < _numTiles-1 )
                    addRightBorder( cTile, xTile );

                if( y < _numTiles-1 )
                    addBottomBorder( cTile, yTile );

                addCornerPatch( cTile, xTile, yTile, xyTile );
            }
            else
            {
                if(cTile->getBorder() == MipmapGeometry::BORDER_NONE )
                    addMaxDistMainBody( cTile, xTile, yTile, xyTile );
                else
                    addMaxDistEdge(cTile,xTile,yTile);
            }
        }
    }

    // Make sure the bounds are updated now that we've changed the topology.
    dirtyBound();
}

void FFTOceanSurface::addMainBody( MipmapGeometry* cTile )
{
    unsigned int degenX = cTile->getRowLen()-1;
    unsigned int degenY = cTile->getColLen()-1;

    unsigned int numDegens = (cTile->getColLen()-1)*2-2;
    unsigned int stripSize = (cTile->getRowLen()*2)*(cTile->getColLen()-1) + numDegens;
    unsigned int i = 0;

    // Generate 1 tristrip using degen triangles
    DrawElementsUInt* strip = new DrawElementsUInt(DrawElements::TRIANGLE_STRIP, stripSize );

    for( unsigned int row = 0; row < cTile->getColLen()-1; ++row )
    {
        for( unsigned int col = 0; col < cTile->getRowLen(); ++col )
        {
            (*strip)[i]   = cTile->getIndex( col, row   );
            (*strip)[i+1] = cTile->getIndex( col, row+1 );
            i+=2;

            if( col == degenX && row+1 != degenY )
            {
                (*strip)[i]   = cTile->getIndex( col, row+1 );
                (*strip)[i+1] = cTile->getIndex( 0,   row+1 );
                i+=2;
            }
        }
    }
    cTile->addPrimitiveSet( strip );
}

void FFTOceanSurface::addMaxDistEdge(  MipmapGeometry* cTile, MipmapGeometry* xTile, MipmapGeometry* yTile )
{
    if( cTile->getBorder() == MipmapGeometry::BORDER_X )
    {
        DrawElementsUInt* strip = new DrawElementsUInt(DrawElements::TRIANGLE_STRIP, 4 );

        (*strip)[0] = cTile->getIndex ( 0, 0 );
        (*strip)[1] = yTile->getIndex ( 0, 0 );
        (*strip)[2] = cTile->getIndex ( 1, 0 );
        (*strip)[3] = yTile->getIndex ( 1, 0 );

        cTile->addPrimitiveSet( strip );
    }
    else if( cTile->getBorder() == MipmapGeometry::BORDER_Y )
    {
        DrawElementsUInt* strip = new DrawElementsUInt(DrawElements::TRIANGLE_STRIP, 4 );

        (*strip)[0] = cTile->getIndex ( 0, 0 );
        (*strip)[1] = cTile->getIndex ( 0, 1 );
        (*strip)[2] = xTile->getIndex ( 0, 0 );
        (*strip)[3] = xTile->getIndex ( 0, 1 );

        cTile->addPrimitiveSet( strip );
    }
    else if( cTile->getBorder() == MipmapGeometry::BORDER_XY )
    {
        DrawElementsUInt* strip = new DrawElementsUInt(DrawElements::TRIANGLE_STRIP, 4 );

        (*strip)[0] = cTile->getIndex ( 0, 0 );
        (*strip)[1] = cTile->getIndex ( 0, 1 );
        (*strip)[2] = cTile->getIndex ( 1, 0 );
        (*strip)[3] = cTile->getIndex ( 1, 1 );

        cTile->addPrimitiveSet( strip );
    }
}

void FFTOceanSurface::addMaxDistMainBody(  MipmapGeometry* cTile, MipmapGeometry* xTile, MipmapGeometry* yTile, MipmapGeometry* xyTile )
{
    int x_points = xTile->getResolution() / cTile->getResolution();
    int y_points = yTile->getResolution() / cTile->getResolution(); 

    // same res bottom and right
    if( x_points == 1 && y_points == 1)
    {
        DrawElementsUInt* strip = new DrawElementsUInt(DrawElements::TRIANGLE_STRIP, 4 );

        (*strip)[0] = cTile->getIndex ( 0, 0 );
        (*strip)[1] = yTile->getIndex ( 0, 0 );
        (*strip)[2] = xTile->getIndex ( 0, 0 );
        (*strip)[3] = xyTile->getIndex( 0, 0 );
        
        cTile->addPrimitiveSet( strip );
    }
    // high res below same res right
    else if( x_points == 1 && y_points == 2 )
    {
        DrawElementsUInt* fan = new DrawElementsUInt(DrawElements::TRIANGLE_FAN, 5 );

        (*fan)[0] = xTile->getIndex ( 0, 0 );
        (*fan)[1] = cTile->getIndex ( 0, 0 );
        (*fan)[2] = yTile->getIndex ( 0, 0 );
        (*fan)[3] = yTile->getIndex ( 1, 0 );
        (*fan)[4] = xyTile->getIndex( 0, 0 );

        cTile->addPrimitiveSet( fan );
    }
    // same res below high res below
    else if( x_points == 2 && y_points == 1 )
    {
        DrawElementsUInt* fan = new DrawElementsUInt(DrawElements::TRIANGLE_FAN, 5 );

        (*fan)[0] = cTile->getIndex ( 0, 0 );
        (*fan)[1] = yTile->getIndex ( 0, 0 );
        (*fan)[2] = xyTile->getIndex( 0, 0 );
        (*fan)[3] = xTile->getIndex ( 0, 1 );
        (*fan)[4] = xTile->getIndex ( 0, 0 );

        cTile->addPrimitiveSet( fan );
    }
    // high res below and right
    else if( x_points == 2 && y_points == 2 )
    {
        DrawElementsUInt* fan = new DrawElementsUInt( DrawElements::TRIANGLE_FAN, 6 );

        (*fan)[0] = cTile->getIndex ( 0, 0 );
        (*fan)[1] = yTile->getIndex ( 0, 0 );
        (*fan)[2] = yTile->getIndex ( 1, 0 );
        (*fan)[3] = xyTile->getIndex( 0, 0 );
        (*fan)[4] = xTile->getIndex ( 0, 1 );
        (*fan)[5] = xTile->getIndex ( 0, 0 );

        cTile->addPrimitiveSet( fan );
    }
}

void FFTOceanSurface::addRightBorder( MipmapGeometry* cTile, MipmapGeometry* xTile )
{
    unsigned int endCol = cTile->getRowLen() - 1;

    // Same level to the right
    if( cTile->getLevel() == xTile->getLevel() )
    {
        //  3   2
        //
        //  0   1

        for(unsigned int r = 0; r < cTile->getColLen()-1; ++r)    
        {
            DrawElementsUInt* fan = new DrawElementsUInt(DrawElements::TRIANGLE_FAN, 4);

            (*fan)[0] = cTile->getIndex( endCol, r+1 );        
            (*fan)[1] = xTile->getIndex( 0,      r+1 );        
            (*fan)[2] = xTile->getIndex( 0,      r   );        
            (*fan)[3] = cTile->getIndex( endCol, r   );        

            cTile->addPrimitiveSet( fan );
        }
    }
    // low res to the right
    else if( cTile->getLevel() < xTile->getLevel() )
    {
        unsigned int diff = cTile->getResolution() / xTile->getResolution(); 
        unsigned int cPts = diff + 1;        
        unsigned int start = 0;

        //  1   0
        //  2
        //  3   4
        
        for(unsigned int r = 0; r < xTile->getColLen()-1; ++r )
        {
            DrawElementsUInt* fan = new DrawElementsUInt(DrawElements::TRIANGLE_FAN, 0);
            fan->reserve( cPts+2 );    

            fan->push_back( xTile->getIndex( 0, r ) );

            start = r*diff;

            for(unsigned int i = 0; i < cPts; ++i)
            {
                fan->push_back( cTile->getIndex( endCol, start+i ) );
            }

            fan->push_back( xTile->getIndex( 0, r+1 ) );

            cTile->addPrimitiveSet( fan );
        }
    }
    // high res to the right
    else if( cTile->getLevel() > xTile->getLevel() )
    {
        unsigned int diff = xTile->getResolution() / cTile->getResolution(); 
        unsigned int xPts = diff + 1;    
        unsigned int start = 0;

        //  4       3
        //          2
        //  0       1

        for(unsigned int r = 0; r < cTile->getColLen()-1; ++r )
        {
            DrawElementsUInt* fan = new DrawElementsUInt(DrawElements::TRIANGLE_FAN, 0);
            fan->reserve( xPts+2 );    

            fan->push_back( cTile->getIndex( endCol, r+1 ) );

            start = (r+1)*diff;

            for(unsigned int i = 0; i < xPts; ++i )
            {
                fan->push_back( xTile->getIndex( 0, start-i ) );
            }

            fan->push_back( cTile->getIndex( endCol, r ) );

            cTile->addPrimitiveSet( fan );
        }
    }
}

void FFTOceanSurface::addBottomBorder( MipmapGeometry* cTile, MipmapGeometry* yTile )
{
    unsigned int endRow = cTile->getColLen() - 1;

    // Same res below
    if( cTile->getLevel() == yTile->getLevel() )
    {
        unsigned int i = 0; 

        DrawElementsUInt* fan = new DrawElementsUInt(DrawElements::TRIANGLE_STRIP, cTile->getRowLen()*2);

        for(unsigned int c = 0; c < cTile->getRowLen(); ++c)
        {
            (*fan)[i]   = cTile->getIndex( c, endRow );    // 0        2
            (*fan)[i+1] = yTile->getIndex( c, 0      );    // 1        3
            i+=2;
        }

        cTile->addPrimitiveSet( fan );
    }
    // lower res below
    else if( cTile->getLevel() < yTile->getLevel() )
    {
        unsigned int diff = cTile->getResolution() / yTile->getResolution(); 
        unsigned int cPts = diff + 1;
        unsigned int start = 0;

        // 4    3   2
        //
        // 0        1

        for(unsigned int c = 0; c < yTile->getRowLen()-1; ++c)
        {
            DrawElementsUInt* fan = new DrawElementsUInt(DrawElements::TRIANGLE_FAN, 0);
            fan->reserve( cPts+2 );

            fan->push_back( yTile->getIndex( c,   0 ) );
            fan->push_back( yTile->getIndex( c+1, 0 ) );

            start = (c+1)*diff;

            for( unsigned int i = 0; i < cPts; ++i )
            {
                fan->push_back( cTile->getIndex( start-i, endRow ) );
            }

            cTile->addPrimitiveSet( fan );
        }
    }
    // Higher res below
    else if( cTile->getLevel() > yTile->getLevel() )
    {
        unsigned int diff = yTile->getResolution() / cTile->getResolution(); 
        unsigned int yPts = diff + 1;        
        unsigned int start = 0;

        //  1       0
        //
        // 2    3   4

        for(unsigned int c = 0; c < cTile->getRowLen()-1; ++c)
        {
            DrawElementsUInt* fan = new DrawElementsUInt(DrawElements::TRIANGLE_FAN, 0);
            fan->reserve( yPts+2 );

            fan->push_back( cTile->getIndex( c+1, endRow ) );
            fan->push_back( cTile->getIndex( c,   endRow ) );

            start = c*diff;

            for( unsigned int i = 0; i < yPts; ++i )
            {
                fan->push_back( yTile->getIndex( start+i, 0 ) );
            }

            cTile->addPrimitiveSet( fan );
        }
    }
}

void FFTOceanSurface::addCornerPatch( MipmapGeometry* cTile, MipmapGeometry* xTile, MipmapGeometry* yTile, MipmapGeometry* xyTile )
{
    // CORNER PATCH
    // ------------

    int x_points = xTile->getResolution() / cTile->getResolution();
    int y_points = yTile->getResolution() / cTile->getResolution(); 

    unsigned int curSize   = cTile->getResolution()-1;
    unsigned int botSize   = yTile->getResolution()-1;
    unsigned int rightSize = xTile->getResolution()-1;

    if( cTile->getBorder() == MipmapGeometry::BORDER_NONE )
    {
        // Low res bottom
        if( y_points == 0 )
        {
            // Low res right
            if( x_points == 0 )
            {
                DrawElementsUInt* fan = new DrawElementsUInt(DrawElements::TRIANGLE_FAN, 6);

                (*fan)[0] = cTile->getIndex ( curSize,   curSize   ); // 5    4
                (*fan)[1] = cTile->getIndex ( curSize-1, curSize   ); //    
                (*fan)[2] = yTile->getIndex ( botSize,   0         ); // 1    0    
                (*fan)[3] = xyTile->getIndex( 0,         0         ); // 
                (*fan)[4] = xTile->getIndex ( 0,         rightSize ); // 2         3
                (*fan)[5] = cTile->getIndex ( curSize,   curSize-1 );    

                cTile->addPrimitiveSet( fan );
            }
            // same res right
            else if( x_points == 1 )
            {
                DrawElementsUInt* fan = new DrawElementsUInt(DrawElements::TRIANGLE_FAN, 5);

                (*fan)[0] = yTile->getIndex ( botSize,   0         );    //
                (*fan)[1] = xyTile->getIndex( 0,         0         );    //           4    3    2
                (*fan)[2] = xTile->getIndex ( 0,         rightSize );    //
                (*fan)[3] = cTile->getIndex ( curSize,   curSize   );    // 0         1
                (*fan)[4] = cTile->getIndex ( curSize-1, curSize   );    //

                cTile->addPrimitiveSet( fan );
            }
            // high res right
            else if( x_points == 2 )
            {
                DrawElementsUInt* fan = new DrawElementsUInt(DrawElements::TRIANGLE_FAN, 6);

                (*fan)[0] = yTile->getIndex    ( botSize,   0           );    // 5    4    3
                (*fan)[1] = xyTile->getIndex    ( 0,        0           );    //
                (*fan)[2] = xTile->getIndex    ( 0,         rightSize   );    //           2
                (*fan)[3] = xTile->getIndex    ( 0,         rightSize-1 );    //    
                (*fan)[4] = cTile->getIndex    ( curSize,   curSize     );    // 0         1
                (*fan)[5] = cTile->getIndex    ( curSize-1, curSize     );    

                cTile->addPrimitiveSet( fan );
            }
        }
        // same res bottom
        else if( y_points == 1 )
        {
            // Low res right
            if( x_points == 0 )
            {
                DrawElementsUInt* fan = new DrawElementsUInt(DrawElements::TRIANGLE_FAN, 7);

                (*fan)[0] = cTile->getIndex ( curSize,   curSize   ); //      6    5
                (*fan)[1] = cTile->getIndex ( curSize-1, curSize   ); //    
                (*fan)[2] = yTile->getIndex ( botSize-1, 0         ); // 1         0    
                (*fan)[3] = yTile->getIndex ( botSize,   0         ); // 
                (*fan)[4] = xyTile->getIndex( 0,         0         ); //           2    3    4
                (*fan)[5] = xTile->getIndex ( 0,         rightSize );
                (*fan)[6] = cTile->getIndex ( curSize,   curSize-1 );    

                cTile->addPrimitiveSet( fan );
            }
            // same res right
            else if( x_points == 1 )
            {
                DrawElementsUInt* strip = new DrawElementsUInt(DrawElements::TRIANGLE_STRIP, 6);

                (*strip)[0] = cTile->getIndex ( curSize-1, curSize   ); // 0    2    4
                (*strip)[1] = yTile->getIndex ( botSize-1, 0         ); //    
                (*strip)[2] = cTile->getIndex ( curSize,   curSize   ); //      1    3    5
                (*strip)[3] = yTile->getIndex ( botSize,   0         ); // 
                (*strip)[4] = xTile->getIndex ( 0,         rightSize );    
                (*strip)[5] = xyTile->getIndex( 0,         0         );

                cTile->addPrimitiveSet( strip );
            }
            // high res right
            else if( x_points == 2 )
            {
                DrawElementsUInt* fan = new DrawElementsUInt(DrawElements::TRIANGLE_FAN, 7);

                (*fan)[0] = cTile->getIndex ( curSize,   curSize     ); // 1    0    6
                (*fan)[1] = cTile->getIndex ( curSize-1, curSize     ); //    
                (*fan)[2] = yTile->getIndex ( botSize-1, 0           ); //                5
                (*fan)[3] = yTile->getIndex ( botSize,   0           ); //
                (*fan)[4] = xyTile->getIndex( 0,         0           ); //      2    3    4
                (*fan)[5] = xTile->getIndex ( 0,         rightSize   );
                (*fan)[6] = xTile->getIndex ( 0,         rightSize-1 );

                cTile->addPrimitiveSet( fan );
            }
        }
        // high res bottom
        else if( y_points == 2 )
        {
            // Low res right
            if( x_points == 0 )
            {
                DrawElementsUInt* fan = new DrawElementsUInt(DrawElements::TRIANGLE_FAN, 6);

                (*fan)[0] = xTile->getIndex( 0,         rightSize );    // 1         0
                (*fan)[1] = cTile->getIndex( curSize,   curSize-1 );    //
                (*fan)[2] = cTile->getIndex( curSize,   curSize   );    // 2            
                (*fan)[3] = yTile->getIndex( botSize-1, 0         );    //
                (*fan)[4] = yTile->getIndex( botSize,   0         );    // 3    4    5
                (*fan)[5] = xyTile->getIndex( 0,        0         );                    

                cTile->addPrimitiveSet( fan );
            }
            // same res right
            if( x_points == 1 )
            {
                DrawElementsUInt* fan = new DrawElementsUInt(DrawElements::TRIANGLE_FAN, 5);

                (*fan)[0] = xTile->getIndex ( 0,         rightSize );    // 1         0
                (*fan)[1] = cTile->getIndex ( curSize,   curSize   );    //    
                (*fan)[2] = yTile->getIndex ( botSize-1, 0         );    // 
                (*fan)[3] = yTile->getIndex ( botSize,   0         );    // 2    3    4
                (*fan)[4] = xyTile->getIndex( 0,         0         );                    

                cTile->addPrimitiveSet( fan );
            }
            // high res right
            if( x_points == 2 )
            {
                DrawElementsUInt* fan = new DrawElementsUInt(DrawElements::TRIANGLE_FAN, 6);

                (*fan)[0] = cTile->getIndex ( curSize,   curSize     );    //    
                (*fan)[1] = yTile->getIndex ( botSize-1, 0           );    // 0         5
                (*fan)[2] = yTile->getIndex ( botSize,   0           );    // 
                (*fan)[3] = xyTile->getIndex( 0,         0           );    //           4
                (*fan)[4] = xTile->getIndex ( 0,         rightSize   );    //
                (*fan)[5] = xTile->getIndex ( 0,         rightSize-1 );    // 1    2    3            

                cTile->addPrimitiveSet( fan );
            }
        }
    }
}

Vec3f FFTOceanSurface::computeNoiseCoords(float noiseSize, const Vec2f& movement, float speed, float time )
{
    float length = noiseSize*movement.norm();
    float totalTime = length / speed;    
    float tileScale = _tileResInv * noiseSize;

    Vec2f velocity = movement * speed / length;
    Vec2f pos = velocity * fmod( time, totalTime );

    return Vec3f( pos, tileScale );
}

#include <shaders/osgOcean_ocean_surface_vert.inl>
#include <shaders/osgOcean_ocean_surface_frag.inl>

void FFTOceanSurface::createShader(void)
{
    static const char osgOcean_ocean_surface_vert_file[] = "osgOcean_ocean_surface.vert";
    static const char osgOcean_ocean_surface_frag_file[] = "osgOcean_ocean_surface.frag";
    m_prog = new OceanSurfaceProgram;
    m_prog->create( osgOcean_ocean_surface_vert,      osgOcean_ocean_surface_frag);
     
}
 