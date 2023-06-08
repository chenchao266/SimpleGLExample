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

#include "FFTOceanTechnique.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <Component/Timer.h>
  
FFTOceanTechnique::FFTOceanTechnique( unsigned int FFTGridSize,
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
    :_tileSize       ( FFTGridSize )
    ,_noiseTileSize  ( FFTGridSize )
    ,_tileResolution ( resolution )
    ,_tileResInv     ( 1.f / float(resolution) )
    ,_noiseTileRes   ( resolution )
    ,_numTiles       ( numTiles )
    ,_pointSpacing   ( _tileResolution / _tileSize )
    ,_windDirection  ( windDirection )
    ,_noiseWindDir   ( windDirection )
    ,_windSpeed      ( windSpeed )
    ,_noiseWindSpeed ( windSpeed )
    ,_waveScale      ( waveScale )
    ,_noiseWaveScale ( waveScale )
    ,_depth          ( depth )
    ,_reflDampFactor ( reflectionDamping )
    ,_cycleTime      ( animLoopTime )
    ,_choppyFactor   ( choppyFactor )
    ,_isChoppy       ( isChoppy )
    ,_isEndless      ( false )
    ,_oldFrame       ( 0 )
    ,_fresnelMul     ( 0.7 )
    ,_numLevels      ( (unsigned int) ( log( (float)_tileSize) / log(2.f) )+1)
    ,_startPos       ( -float( (_tileResolution+1)*_numTiles) * 0.5f, float( (_tileResolution+1)*_numTiles) * 0.5f )
    ,_THRESHOLD      ( 3.f )
    ,_VRES           ( 1024 )
    ,_NUMFRAMES      ( numFrames )
    ,_waveTopColor   ( 0.192156862f, 0.32549019f, 0.36862745098f )
    ,_waveBottomColor( 0.11372549019f, 0.219607843f, 0.3568627450f )
    ,_useCrestFoam   ( false )
    ,_foamCapBottom  ( 2.2f )
    ,_foamCapTop     ( 3.0f )
    ,_isStateDirty   ( true )
    ,_averageHeight  ( 0.f )
    ,_lightColor     ( 0.411764705f, 0.54117647f, 0.6823529f, 1.f )
{
 
 
    setUserPointer( new OceanDataType(*this, _NUMFRAMES, 25) );
    //setOceanAnimationCallback( new OceanAnimationCallback );
}

FFTOceanTechnique::FFTOceanTechnique( const FFTOceanTechnique& copy  ):
    OceanTechnique   ( copy )
    ,_tileSize       ( copy._tileSize )
    ,_noiseTileSize  ( copy._noiseTileSize )
    ,_tileResolution ( copy._tileResolution )
    ,_tileResInv     ( copy._tileResInv )
    ,_noiseTileRes   ( copy._noiseTileRes )
    ,_numTiles       ( copy._numTiles )
    ,_pointSpacing   ( copy._pointSpacing )
    ,_windDirection  ( copy._windDirection )
    ,_noiseWindDir   ( copy._noiseWindDir )
    ,_windSpeed      ( copy._windSpeed )
    ,_noiseWindSpeed ( copy._noiseWindSpeed )
    ,_waveScale      ( copy._waveScale )
    ,_noiseWaveScale ( copy._noiseWaveScale )
    ,_depth          ( copy._depth )
    ,_cycleTime      ( copy._cycleTime )
    ,_choppyFactor   ( copy._choppyFactor )
    ,_isChoppy       ( copy._isChoppy )
    ,_isEndless      ( copy._isEndless )
    ,_oldFrame       ( copy._oldFrame )
    ,_fresnelMul     ( copy._fresnelMul )
    ,_numLevels      ( copy._numLevels )
    ,_startPos       ( copy._startPos )
    ,_THRESHOLD      ( copy._THRESHOLD )
    ,_VRES           ( copy._VRES )
    ,_NUMFRAMES      ( copy._NUMFRAMES )
    ,_minDist        ( copy._minDist )
    ,_environmentMap ( copy._environmentMap )
    ,_waveTopColor   ( copy._waveTopColor )
    ,_waveBottomColor( copy._waveBottomColor )
    ,_useCrestFoam   ( copy._useCrestFoam )
    ,_foamCapBottom  ( copy._foamCapBottom )
    ,_foamCapTop     ( copy._foamCapTop )
    ,_isStateDirty   ( copy._isStateDirty )
    ,_averageHeight  ( copy._averageHeight )
    ,_lightColor     ( copy._lightColor )
{}

FFTOceanTechnique::~FFTOceanTechnique(void)
{
}

Texture2D* FFTOceanTechnique::createTexture(const std::string& name, Texture::WrapMode wrap)
{
    Texture2D* tex = new Texture2D();

    tex->setFilter(Texture::MIN_FILTER, Texture::LINEAR_MIPMAP_LINEAR);
    tex->setFilter(Texture::MAG_FILTER, Texture::LINEAR);
    tex->setWrap  (Texture::WRAP_S,     wrap);
    tex->setWrap  (Texture::WRAP_T,     wrap);
    Image* img = new Image; img->setFileName(name);
    tex->setImage (img);

    return tex;
}

float FFTOceanTechnique::getSurfaceHeightAt(float x, float y, Vec3f* normal)
{
    std::cout << "getSurfaceHeightAt() not implemented." << std::endl;
    return 0.f;
}
 

// --------------------------------------------------------
//  OceanDataType implementation
// --------------------------------------------------------

FFTOceanTechnique::OceanDataType::OceanDataType( FFTOceanTechnique& ocean, 
                                                 unsigned int numFrames, 
                                                 unsigned int fps )
    :_oceanSurface  ( ocean )
    ,_NUMFRAMES     ( numFrames )
    ,_time          ( 0.0 )
    ,_FPS           ( fps )
    ,_msPerFrame    ( 1000.0/(double)fps )
    ,_frame         ( 0 )
    ,_oldTime       ( 0 )
    ,_newTime       ( 0 )
{}

FFTOceanTechnique::OceanDataType::OceanDataType( const OceanDataType& copy )
    :_oceanSurface  ( copy._oceanSurface )
    ,_NUMFRAMES     ( copy._NUMFRAMES )
    ,_eye           ( copy._eye )
    ,_time          ( copy._time )
    ,_FPS           ( copy._FPS )
    ,_msPerFrame    ( copy._msPerFrame )
    ,_frame         ( copy._frame )
    ,_oldTime       ( copy._oldTime )
    ,_newTime       ( copy._newTime )
{}

void FFTOceanTechnique::OceanDataType::updateOcean( double simulationTime )
{
    _oldTime = _newTime;

    if (simulationTime < 0.0)
    {
        _newTime = Timer::instance()->tick();
    }
    else
    {
        // simulationTime is passed in seconds.
        _newTime = simulationTime / Timer::instance()->getSecondsPerTick();
    }

    double dt = Timer::instance()->delta_m(_oldTime, _newTime);
    _time += dt;

    if( _time >= _msPerFrame )
    {
        _frame += ( _time / _msPerFrame );

        if( _frame >= _NUMFRAMES ) 
            _frame = _frame%_NUMFRAMES; 

        _time = fmod( _time, (double)_msPerFrame );
    }

    _oceanSurface.update( _frame, dt, _eye );
}

// --------------------------------------------------------
//  EventHandler implementation
// --------------------------------------------------------
 

bool FFTOceanTechnique::handle(int key)
{
    // Call parent class's handle().
    OceanTechnique::handle(key);
  
    bool _autoDirty(true);
    // Now we can handle this class's events.
 
    {
   
        {
            // Downcast to the concrete class we're interested in.
            
            // Crest foam
            if (key == 'f' )
            {
                enableCrestFoam(!isCrestFoamEnabled());
                std::cout << "Crest foam " << (isCrestFoamEnabled()? "enabled" : "disabled") << std::endl;
                return true;
            }
            // isChoppy
            if( key == 'p' )
            {
                setIsChoppy(!isChoppy(), _autoDirty);
                std::cout << "Choppy waves " << (isChoppy()? "enabled" : "disabled") << std::endl;
                return true;
            }
            // Wind speed + 0.5
            if (key == 'W')
            {
                setWindSpeed(getWindSpeed() + 0.5, _autoDirty);
                std::cout << "Wind speed now " << getWindSpeed() << std::endl;
                return true;
            }
            // Wind speed - 0.5
            if (key == 'w')
            {
                setWindSpeed(getWindSpeed() - 0.5, _autoDirty);
                std::cout << "Wind speed now " << getWindSpeed() << std::endl;
                return true;
            }
            // Scale factor + 1e-9
            if(key == 'K' )
            {
                float waveScale = getWaveScaleFactor();
                setWaveScaleFactor(waveScale+(1e-9), _autoDirty);
                std::cout << "Wave scale factor now " << getWaveScaleFactor() << std::endl;
                return true;
            }
            // Scale factor - 1e-9
            if(key == 'k' )
            {
                float waveScale = getWaveScaleFactor();
                setWaveScaleFactor(waveScale-(1e-9), _autoDirty);
                std::cout << "Wave scale factor now " << getWaveScaleFactor() << std::endl;
                return true;
            }
            // Dirty geometry
            if (key == 'd')
            {
                std::cout << "Dirtying ocean geometry" << std::endl;
                dirty();
                return true;
            }
            // Toggle autoDirty, if off then individual changes will be 
            // instantaneous but the user will get no feedback until they 
            // dirty manually, if on each change will dirty automatically.
            if (key == 'D')
            {
                _autoDirty = !_autoDirty;
                std::cout << "AutoDirty " << (_autoDirty? "enabled" : "disabled") << std::endl;
                return true;
            }
            // Print out all current settings to the console.
            if (key == 'P')
            {
                //std::cout << "Current FFTOceanTechnique settings are:" << std::endl;
                //std::cout << "  Endless ocean " << (isEndlessOceanEnabled()? "enabled" : "disabled") << std::endl;
                //std::cout << "  Crest foam " << (isCrestFoamEnabled()? "enabled" : "disabled") << std::endl;
                //std::cout << "  Choppy waves " << (isChoppy()? "enabled" : "disabled") << std::endl;
                //std::cout << "  Choppy factor " << getChoppyFactor() << std::endl;
                //std::cout << "  Wind direction " << getWindDirection() << std::endl;
                //std::cout << "  Wind speed " << getWindSpeed() << std::endl;
                //std::cout << "  Wave scale factor " << getWaveScaleFactor() << std::endl;
                return true;
            }
         
        }
 
    }

    return false;
}
 
// --------------------------------------------------------
//  OceanAnimationCallback implementation
// --------------------------------------------------------

void FFTOceanTechnique::cull(CullVisitor* nv)
{
    CullVisitor* cv = static_cast<CullVisitor*>(nv);
  
    if (cv->isShadowMap()) //   
    {
    }
    else
    {
        OceanDataType* oceanData = static_cast<OceanDataType*> (getUserPointer());

        oceanData->setEye(cv->getEyePoint());
    }
     
}

void FFTOceanTechnique::update1(double simulationTime)
{
    OceanDataType* oceanData = static_cast<OceanDataType*> ( getUserPointer() );

    if( oceanData )
    {
        // If cull visitor update the current eye position
   
        ///update(-1.0);
     
        oceanData->updateOcean(simulationTime);
    
    }
 
}
