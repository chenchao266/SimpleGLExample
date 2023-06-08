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

#include "FFTOceanSurfaceVBO.h"
#include "OpenGLWindow/LoadShader.h"
#include <iostream>
#include <sstream>
#include <fstream>
struct OceanSurfaceVBOProgram :public ProgramBase
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
#define USE_LOCAL_SHADERS 1

FFTOceanSurfaceVBO::FFTOceanSurfaceVBO( unsigned int FFTGridSize,
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
                                        unsigned int numFrames)
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
    ,_masterVertices ( new Vec3Array )
    ,_masterNormals  ( new Vec3Array )
{
    setUserPointer( new OceanDataType(*this, _NUMFRAMES, 25) );
    //setCullCallback( new OceanAnimationCallback );
    //setUpdateCallback( new OceanAnimationCallback );

    _minDist.clear();
    std::cout << "Minimum Distances: " << std::endl;

    for(unsigned int d = 0; d < _numLevels; ++d)
    {
        _minDist.push_back( d * (float(_tileResolution+1)) + ( float(_tileResolution+1.f)*0.5f ) );
        _minDist.back() *= _minDist.back();
        std::cout << d << ": " << sqrt(_minDist.back()) << std::endl;
    }

    std::cout << "FFTOceanSurfaceVBO::createOceanTiles() Complete." << std::endl;

}

FFTOceanSurfaceVBO::FFTOceanSurfaceVBO( const FFTOceanSurfaceVBO& copy )
    :FFTOceanTechnique ( copy )
    ,_masterVertices   ( copy._masterVertices )
    ,_masterNormals    ( copy._masterNormals )
    ,_mipmapGeom       ( copy._mipmapGeom )
    ,_mipmapData       ( copy._mipmapData )
{}

FFTOceanSurfaceVBO::~FFTOceanSurfaceVBO(void)
{
}

void FFTOceanSurfaceVBO::build( void )
{
    std::cout << "FFTOceanSurfaceVBO::build()" << std::endl;

    computeSea( _NUMFRAMES );
    createOceanTiles();
    updateLevels(Vec3f(0.0f, 0.0f, 0.0f));
    updateVertices(0);

    initStateSet();

    _isDirty =  false;
    _isStateDirty = false;

    std::cout << "FFTOceanSurfaceVBO::build() Complete." << std::endl;
}

void FFTOceanSurfaceVBO::initStateSet( void )
{
    std::cout << "FFTOceanSurfaceVBO::initStateSet()" << std::endl;
     

    createShader();
    // Note that we will only set the textures in the state set if shaders are
    // enabled, otherwise the fixed pipeline will try to put the env map onto
    // the water surface, which has no texture coordinates, so the surface
    // will take the general color of the env map..
    m_prog->osgOcean_EnvironmentMap = m_prog->getLoc("osgOcean_EnvironmentMap"); //  
    m_prog->setUniform(m_prog->osgOcean_EnvironmentMap, ENV_MAP);

    // Environment map    
 
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
    m_prog->setUniform(m_prog->osgOcean_NoiseCoords0, computeNoiseCoords(32.f, Vec2f(2.f, 4.f), 2.f, 0.f) );

    m_prog->osgOcean_NoiseCoords1 = m_prog->getLoc("osgOcean_NoiseCoords1"); //  
    m_prog->setUniform(m_prog->osgOcean_NoiseCoords1, computeNoiseCoords(8.f, Vec2f(-4.f, 2.f), 1.f, 0.f) );
     
    std::shared_ptr<Texture2D> noiseMap 
        = createNoiseMap( _noiseTileSize, _noiseWindDir, _noiseWindSpeed, _noiseWaveScale, _noiseTileRes ); 

 
    {
        //( NORMAL_MAP, noiseMap.get() );
    }

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

    std::cout << "FFTOceanSurfaceVBO::initStateSet() Complete." << std::endl;
}

std::shared_ptr<Texture2D> FFTOceanSurfaceVBO::createNoiseMap(unsigned int size, 
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

void FFTOceanSurfaceVBO::computeSea( unsigned int totalFrames )
{
    std::cout << "FFTOceanSurfaceVBO::computeSea("<<totalFrames<<")" << std::endl;
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

        // Level 0
        _mipmapData[frame] = OceanTile( heights.get(), _tileSize, _pointSpacing, displacements.get(), true );

        _averageHeight += _mipmapData[frame].getAverageHeight();

        _maxHeight = b3Max(_maxHeight, _mipmapData[frame].getMaximumHeight());
    }
    _averageHeight /= (float)totalFrames;

    std::cout << "Average Height: " << _averageHeight << std::endl;
    std::cout << "FFTOceanSurfaceVBO::computeSea() Complete." << std::endl;
}

void FFTOceanSurfaceVBO::createOceanTiles( void )
{
    std::cout << "FFTOceanSurfaceVBO::createOceanTiles()" << std::endl;
    std::cout << "Total tiles: " << _numTiles*_numTiles << std::endl;

    // Clear previous data if it exists
    _mipmapGeom.clear();

    removeDrawables(0, getNumDrawables());

    // Setup Vertex buffer objects
  
    // reset (just in case)
    _masterVertices->clear();
    _masterNormals->clear();

    _masterVertices->resize( _mipmapData[0].getNumVertices() );
    _masterNormals->resize ( _mipmapData[0].getNumVertices() );
 
    // Setup mipmap geometry tiles
    // ------------------------------------------------------------

    for(int y = 0; y < (int)_numTiles; ++y )
    {
        std::vector< std::shared_ptr<MipmapGeometryVBO> > tileRow(_numTiles);
        for(int x = 0; x < (int)_numTiles; ++x )
        {
            int centreX = -((int)(_numTiles*(int)_tileResolution))/2;
            int centreY =  ((int)(_numTiles*(int)_tileResolution))/2;
            Vec3f offset( centreX+x*(int)_tileResolution, centreY-y*(int)_tileResolution, 0.f ); 

            MipmapGeometryVBO* tile = new MipmapGeometryVBO( _numLevels, _tileResolution );
            tile->setOffset( offset );

            BoundingBoxf bb;

            bb.xMin() = (int)offset.x;
            bb.xMax() = (int)offset.x+(int)_tileResolution;

            bb.yMin() = (int)offset.y-(int)_tileResolution;
            bb.yMax() = (int)offset.y;

            bb.zMin() = (int)-15.f;
            bb.zMax() = (int)15.f;
 
            tile->setInitialBound(bb);
            
            tileRow.at(x).reset(tile);

            // assign the master arrays to the tile geometry
            tile->initialiseArrays( _masterVertices.get(), _masterNormals.get() );

            addDrawable( tile );

        }
        _mipmapGeom.push_back(tileRow);
    }

    return;
}

void FFTOceanSurfaceVBO::setMinDistances( std::vector<float> &minDist )
{
    if (_numLevels != minDist.size())
    {
        std::cout << "FFTOceanSurface::setMinDistances() Incorrect Number of Levels." << std::endl;
        std::cout << "Found " << minDist.size() << " Expected " << _numLevels << std::endl;
        std::cout << "Ignoring Min Distances" << std::endl;
        return;
    }
    _minDist.clear();

    std::cout << "setting Minimum Distances: " << std::endl;

    for(unsigned int d = 0; d < _numLevels; ++d)
    {
        _minDist.push_back( minDist[d] * minDist[d] );
        std::cout << d << ": " << sqrt(_minDist.back()) << std::endl;
    }
}

static int count = 0;

void FFTOceanSurfaceVBO::updateVertices(unsigned int frame)
{
#ifdef OSGOCEAN_TIMING
    Timer_t startTime;
    Timer_t endTime;
    startTime = Timer::instance()->tick();
#endif /*OSTOCEAN_TIMING*/

    Vec3f tileOffset;

    const OceanTile& data = _mipmapData[frame];

    // copy the new data into the master arrays
    (*_masterVertices) = *data.getVertices();
    (*_masterNormals)  = *data.getNormals();

    // dirty the arrays so VBOs are resent.
    //_masterVertices->dirty();
    //_masterNormals->dirty();

#ifdef OSGOCEAN_TIMING
    endTime = Timer::instance()->tick();
    double dt = Timer::instance()->delta_m(startTime, endTime);
    fprintf(stderr, "updateVertices() time = %lfms\n", dt);
#endif /*OSGOCEAN_TIMING*/
}

bool FFTOceanSurfaceVBO::updateLevels(const Vec3f& eye)
{
   int x_offset = 0;
   int y_offset = 0;

   if(_isEndless)
   {
      float xMin = _startPos.x;
      float yMin = _startPos.y-(_tileResolution*_numTiles);
      
      x_offset = (int) ( (eye.x-xMin) / _tileResolution );
      y_offset = (int) ( (eye.y-yMin) / _tileResolution );
      
      x_offset -= ((int)_numTiles)/2;
      y_offset -= ((int)_numTiles)/2;
//      std::cerr <<  "Offset: " << x_offset << "," << y_offset << std::endl;
//      std::cerr <<  "Start: " << _startPos.x << "," << _startPos.y << std::endl;
      
      if(x_offset != 0 || y_offset != 0)
      {
         //std::cerr << "Surface Move." << std::endl;
         
         while ((x_offset != 0) || (y_offset != 0))
         {
            if(x_offset < 0)
            {
               Vec3f offset;
               _startPos.x -= (int)_tileResolution;
               
               for(int r = 0; r < (int)_numTiles; ++r)
               {
                  std::vector< std::shared_ptr<MipmapGeometryVBO> >& row = _mipmapGeom.at(r);
                  
                  offset.x = _startPos.x;
                  offset.y = _startPos.y-r*(int)_tileResolution;
                  offset.z = 0;
                  
                  row.insert( row.begin(), row.back() );   // insert the 
                  row.pop_back(); 
                  row.front()->setOffset( offset );         // change offset
               }
               ++x_offset;
            }
            else if (x_offset > 0)
            {
               Vec3f offset;
               _startPos.x += (int)_tileResolution;
               
               for(int r = 0; r < (int)_numTiles; ++r)
               {
                  std::vector< std::shared_ptr<MipmapGeometryVBO> >& row = _mipmapGeom.at(r);
                  
                  offset.x = _startPos.x + ( (_numTiles-1)*(int)_tileResolution );
                  offset.y = _startPos.y-r*(int)_tileResolution;
                  offset.z = 0;
                  
                  row.insert( row.end(), row.front() );
                  row.erase( row.begin() );
                  row.back()->setOffset( offset );
               } 
               --x_offset;                  
            }
            
            if(y_offset < 0)
            {
               _startPos.y -= (int)_tileResolution;

               _mipmapGeom.insert( _mipmapGeom.end(), _mipmapGeom.front() );
               _mipmapGeom.erase( _mipmapGeom.begin() );
               
               Vec3f offset;
               
               for(int c = 0; c < (int)_numTiles; c++ )
               {
                  offset.x = _startPos.x + c *(int) _tileResolution;
                  offset.y = _startPos.y-( (_numTiles-1)*(int)_tileResolution );
                  offset.z = 0;
                  
                  _mipmapGeom.back().at(c)->setOffset(offset);
               }
               ++y_offset;
            }
            else if(y_offset > 0)
            {
               _startPos.y += (int)_tileResolution;

               _mipmapGeom.insert( _mipmapGeom.begin(), _mipmapGeom.back() );
               _mipmapGeom.pop_back();
               
               Vec3f offset;
               
               for(int c = 0; c < (int)_numTiles; c++ )
               {
                  offset.x = _startPos.x + c * (int)_tileResolution;
                  offset.y = _startPos.y;
                  offset.z = 0;
                  
                  _mipmapGeom.front().at(c)->setOffset(offset);
               }
               --y_offset;
            }
         }
      }
   }
   
   unsigned updates=0;
   
   for(int r = _numTiles-1; r>=0; --r )
   {
      for(int c = _numTiles-1; c>=0; --c )
      {
         MipmapGeometryVBO* curGeom = _mipmapGeom.at(r).at(c).get();
         Vec3f centre = curGeom->getBound().center();
         
         float distanceToTile2 = (centre-eye).norm2();
         
         unsigned mipmapLevel = 0;
         unsigned rightLevel  = 0;
         unsigned belowLevel  = 0;
         
         for( unsigned int m = 0; m < _minDist.size(); ++m )
         {
            if( distanceToTile2 > _minDist.at(m) )
               mipmapLevel = m;
         }
         
         if( c != _numTiles-1 && r != _numTiles-1 ){
            MipmapGeometryVBO* rightGeom = _mipmapGeom.at(r).at(c+1).get();
            MipmapGeometryVBO* belowGeom = _mipmapGeom.at(r+1).at(c).get();
            rightLevel = rightGeom->getLevel();
            belowLevel = belowGeom->getLevel();
         }
         else 
         {
            if( c != _numTiles-1 ){
               MipmapGeometryVBO* rightGeom = _mipmapGeom.at(r).at(c+1).get();
               rightLevel = rightGeom->getLevel();
            }
            else{
               rightLevel = mipmapLevel;
            }
            
            if( r != _numTiles-1 ){
               MipmapGeometryVBO* belowGeom = _mipmapGeom.at(r+1).at(c).get();
               belowLevel = belowGeom->getLevel();
            }
            else{
               belowLevel = mipmapLevel;
            }
         }

         if( curGeom->updatePrimitives(mipmapLevel,rightLevel,belowLevel) )
            updates++;
      }
   }

#ifdef OSGOCEAN_MIPMAP
   if (updates > 0)
   {
        std::cerr <<  "Updates: " << updates << std::endl;
        for(int r = _numTiles-1; r>=0; --r )
        {
           for(int c = _numTiles-1; c>=0; --c )
           {
              fprintf(stderr, "%d", _mipmapGeom.at(r).at(c)->getLevel());
           }
           fprintf(stderr, "\n");
        }
   }
#endif /*OSGOCEAN_MIPMAP*/

   return updates > 0;
}


void FFTOceanSurfaceVBO::update( unsigned int frame, const double& dt, const Vec3f& eye )
{
#ifdef OSGOCEAN_TIMING
    Timer_t startTime;
    Timer_t endTime;
    startTime = Timer::instance()->tick();
#endif /*OSTOCEAN_TIMING*/

    if(_isDirty)
        build();
    else if(_isStateDirty)
        initStateSet();

    if (_isAnimating)
    {
        static double time = 0.0;
        time += (dt * 0.001);      // dt is in milliseconds (see FFTOceanTechnique::OceanDataType::updateOcean() )

        m_prog->setUniform(m_prog->osgOcean_FrameTime,  float(time) );

        static double noiseTime = 0.0;
        noiseTime += (dt*0.0008);

        m_prog->setUniform(m_prog->osgOcean_NoiseCoords0, computeNoiseCoords( 32.f, Vec2f( 2.f, 4.f), 2.f, time ) );
        m_prog->setUniform(m_prog->osgOcean_NoiseCoords1, computeNoiseCoords( 8.f,  Vec2f(-4.f, 2.f), 1.f, time ) );

        if( updateLevels(eye) || frame != _oldFrame )
        {
            updateVertices(frame);
        } 
    }

    _oldFrame = frame;

#ifdef OSGOCEAN_TIMING
    endTime = Timer::instance()->tick();
    double deltaTime = Timer::instance()->delta_m(startTime, endTime);
    fprintf(stderr, "FFTOceanSurfaceVBO::update() time = %lfms\n", deltaTime);
#endif /*OSGOCEAN_TIMING*/
}

float FFTOceanSurfaceVBO::getSurfaceHeightAt(float x, float y, Vec3f* normal)
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
        const OceanTile& data = _mipmapData[_oldFrame];

        float tile_x = oceanX - ix * (int) _tileResolution;
        float tile_y = oceanY - iy * (int) _tileResolution;

        if (normal != 0)
        {
            *normal = data.normalBiLinearInterp(tile_x, tile_y);
        }

        return data.biLinearInterp(tile_x, tile_y);
    }

    return 0.0f;
}


Vec3f FFTOceanSurfaceVBO::computeNoiseCoords(float noiseSize, const Vec2f& movement, float speed, double time )
{
    float length = noiseSize*movement.norm();
    double totalTime = length / speed;    
    float tileScale = _tileResInv * noiseSize;

    Vec2f velocity = movement * speed / length;
    Vec2f pos = velocity * fmod( time, totalTime );

    return Vec3f( pos, tileScale );
}

#include <shaders/osgOcean_ocean_surface_vbo_vert.inl>
#include <shaders/osgOcean_ocean_surface_frag.inl>

void FFTOceanSurfaceVBO::createShader(void)
{
    static const char osgOcean_ocean_surface_vert_file[] = "osgOcean_ocean_surface_vbo.vert";
    static const char osgOcean_ocean_surface_frag_file[] = "osgOcean_ocean_surface.frag";
 
    m_prog = new OceanSurfaceVBOProgram;
    m_prog->create( osgOcean_ocean_surface_vbo_vert,  osgOcean_ocean_surface_frag);
     
}
 