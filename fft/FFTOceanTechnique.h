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

#pragma once
 
#include "OceanTechnique.h"
#include "FFTSimulation.h"
#include "OceanTile.h"

#include <Component/Texture2D.h>
#include <Component/TextureCubeMap.h>

#include <vector>

 
    class   FFTOceanTechnique : public OceanTechnique
    {
    protected:
        unsigned int _tileSize;             /**< Size of FFT grid 2^n ie 128,64,32 etc. */
        unsigned int _noiseTileSize;        /**< Size of Noise FFT grid */
        unsigned int _tileResolution;       /**< Size of tile in world width/height. */
        float        _tileResInv;           /**< 1 / _tileResoltion; */
        unsigned int _noiseTileRes;         /**< Size of noise tile in world width/height. */
        unsigned int _numTiles;             /**< Number of tiles on width/height. */
        float        _pointSpacing;         /**< Spacing between points in real world. */
        Vec2f   _windDirection;        /**< Direction of wind. */
        Vec2f   _noiseWindDir;         /**< Direction of wind for noise tile. */
        float        _windSpeed;            /**< Wind speed m/s */
        float        _noiseWindSpeed;       /**< Wind speed for noise tile m/s */
        float        _waveScale;            /**< Wave scale modifier. */
        float        _noiseWaveScale;       /**< Wave scale modifier for noise tile. */
        float        _depth;                /**< Depth (m). */
        float        _reflDampFactor;       /**< Dampen waves going against the wind */
        float        _cycleTime;            /**< Time before the ocean tiles loop. */
        float        _choppyFactor;         /**< Amount of chop to add. */
        bool         _isChoppy;             /**< Enable choppy waves generation. */
        bool         _isEndless;            /**< Set whether the ocean is of fixed size. */

        Vec2f   _startPos;             /**< Start position of the surface ( -half width, half height ). */

        const float  _THRESHOLD;            /**< Pixel threshold. */
        const float  _VRES;                 /**< Vertical resolution. */
        float        _C;                    /**< C constant. */

        unsigned int _numLevels;            /**< Number of mipmap levels. */
        unsigned int _oldFrame;             /**< Last ocean frame number. */

        const unsigned int _NUMFRAMES;      /**< Number of frames in the animation cycle */

        Vec4f  _lightColor;            /**< Color of the sun */
        Vec3f  _waveTopColor;          /**< Color for the upwelling shading. */
        Vec3f  _waveBottomColor;       /**< Color for the upwelling shading. */
        bool        _useCrestFoam;          /**< Crest foam flag. */
        float       _foamCapTop;            /**< Maximum height for foam caps. */
        float       _foamCapBottom;         /**< Minimum height for foam caps. */
        float       _averageHeight;         /**< Average height over the total tiles. */
        float       _maxHeight;             /**< Maximum height over the total tiles. */
        float       _fresnelMul;            /**< Fresnel multiplier uniform, typical values: (0.5-0.8). */
        //1. 反射公式: fresnel = fresnel基础值 + fresnel缩放量*pow( 1 - dot( N, V ), 5 )
        bool        _isStateDirty;

        std::vector<float> _minDist;        /**< Minimum distances used for mipmap selection */

        std::shared_ptr<TextureCubeMap> _environmentMap;  /**< Cubemap used for refractions/reflections */

        enum TEXTURE_UNITS{ ENV_MAP=0,REFLECT_MAP=1,REFRACT_MAP=2,REFRACTDEPTH_MAP=3,NORMAL_MAP=4,FOG_MAP=5,FOAM_MAP=6 };

    public:
        FFTOceanTechnique(unsigned int FFTGridSize,
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
            unsigned int numFrames );

        FFTOceanTechnique( const FFTOceanTechnique& copy );
          
        virtual ~FFTOceanTechnique(void);

    public:
        /** 
        * Returns the height at the given point (in local space). If the 
        * address of a Vec3f is passed in the normal argument, the normal at
        * that position will be calculated and stored in it.
        */
        virtual float getSurfaceHeightAt(float x, float y, Vec3f* normal = NULL);

        /**
        * Checks for mipmap or frame changes and updates the geometry accordingly.
        * Will rebuild state or geometry if found to be dirty.
        */
        virtual void update( unsigned int frame, const double& dt, const Vec3f& eye )=0;

    protected:
        /** 
        * Convenience method for creating a Texture2D based on an image file path. 
        */
        Texture2D* createTexture( const std::string& path, Texture::WrapMode wrap );

    // -------------------------------------------------------------
    // inline accessors/mutators
    // -------------------------------------------------------------

    public:
        inline void setEnvironmentMap( TextureCubeMap* environmentMap ){
            _environmentMap .reset( environmentMap);
            _isStateDirty = true;
        }

        inline void setWaveTopColor( const Vec3f& color ){
            _waveTopColor = color;
            _isStateDirty = true;
        }

        inline const Vec3f& getWaveTopColor( void ) const {
            return _waveTopColor;
        }

        inline void setWaveBottomColor( const Vec3f& color ){
            _waveBottomColor = color;
            _isStateDirty = true;
        }

        inline const Vec3f& getWaveBottomColor( void ) const {
            return _waveTopColor;
        }

        inline void setLightColor( const Vec4f& color ){
            _lightColor = color;
            _isStateDirty = true;
        }

        inline Vec4f getLightColor() const{
            return _lightColor;
        }

        inline void enableCrestFoam( bool enable ){
            _useCrestFoam = enable;
            _isStateDirty = true;
        }

        inline bool isCrestFoamEnabled() const{
            return _useCrestFoam;
        }

        inline void setFoamBottomHeight( float height ){
            _foamCapBottom = height;
            _isStateDirty = true;
        }

        inline void setFoamTopHeight( float height ){
            _foamCapTop = height;
            _isStateDirty = true;
        }

        inline void setFresnelMultiplier( float mul ){
            _fresnelMul = mul;
            _isStateDirty = true;
        }

        /**
        * Enable/Disable endless ocean.
        * Dirties geometry by default, pass dirty=false to dirty yourself later.
        */
        inline void enableEndlessOcean( bool enable, bool dirty = true ){
            _isEndless = enable;
            if (dirty) _isDirty = true;
        }

        inline bool isEndlessOceanEnabled() const{
            return _isEndless;
        }

        /** Returns the average height over the whole surface (in local space)*/
        inline float getSurfaceHeight( void ) const {
            return _averageHeight;
        }

        /** Returns the maximum height over the whole surface (in local space)*/
        inline float getMaximumHeight( void ) const {
            return _maxHeight;
        }

        /**
        * Enable/Disable choppy wave geometry.
        * Dirties geometry by default, pass dirty=false to dirty yourself later.
        */
        inline void setIsChoppy(bool isChoppy, bool dirty = true){
            _isChoppy = isChoppy;
            if (dirty) _isDirty = true;
        }

        inline bool isChoppy(void) const{
            return _isChoppy;
        }

        /**
        * Change the choppy factor.
        * Dirties geometry by default, pass dirty=false to dirty yourself later.
        */
        inline void setChoppyFactor(float choppyFactor, bool dirty = true){
            _choppyFactor = choppyFactor;
            if (dirty) _isDirty = true;
        }

        inline float getChoppyFactor() const{
            return _choppyFactor;
        }

        /**
        * Tweak the wave scale factor.
        * Typically a very small value: ~1e-8.
        * Dirties geometry by default, pass dirty=false to dirty yourself later.
        */
        inline void setWaveScaleFactor( float scale, bool dirty = true ){
            _waveScale = scale;
            if (dirty) _isDirty = true;
        }

        inline float getWaveScaleFactor( void ) const {
            return _waveScale;
        }

        /**
        * Change the wind direction.
        * Dirties geometry by default, pass dirty=false to dirty yourself later.
        */
        inline void setWindDirection(const Vec2f& windDir, bool dirty = true){
            _windDirection = windDir;
            if (dirty) _isDirty = true;
        }

        inline Vec2f getWindDirection() const{
            return _windDirection;
        }

        /**
        * Change the wind speed.
        * Dirties geometry by default, pass dirty=false to dirty yourself later.
        */
        inline void setWindSpeed(const float windSpeed, bool dirty = true){
            _windSpeed = windSpeed;
            if (dirty) _isDirty = true;
        }

        inline float getWindSpeed() const{
            return _windSpeed;
        }

        inline void setDepth(const float depth, bool dirty = true){
            _depth = depth;
            if (dirty) _isDirty = true;
        }

        inline float getDepth() const{
            return _depth;
        }

        /**
        * Sets the parameters for a custom noise map for use in the fragment shader.
        * @param FFTSize is the size of the FFT grid that will be used and thus the size of the resulting texture. Values must be 2^n.
        * Dirties the stateset.
        */
        inline void setNoiseMapParams( unsigned int FFTSize, 
            const Vec2f& windDir, 
            float windSpeed, 
            float waveScale,
            float tileResolution )
        {
            _noiseTileSize = FFTSize;
            _noiseWindDir = windDir;
            _noiseWindSpeed = windSpeed;
            _noiseWaveScale = waveScale;
            _noiseTileRes = tileResolution;

            _isStateDirty = true;
        }

        /**
        * Linear interpolation of 3 RGBA colors.
        * @return interpolated color (vec4)
        */
        inline Vec4f colorLerp (const Vec4f& c0, const Vec4f& c1, const Vec4f& c2) const 
        {
            return Vec4f(
                c1[0]*(1-c0[0]) + c2[0]*c0[0],
                c1[1]*(1-c0[1]) + c2[1]*c0[1],
                c1[2]*(1-c0[2]) + c2[2]*c0[2],
                c1[3]*(1-c0[3]) + c2[3]*c0[3]
            );
        }

    protected:
        // --------------------------------------------------------
        //  OceanDataType 
        // --------------------------------------------------------

        /**
        * Data structure to store data needed for animation callback.
        */
        class  OceanDataType//: public Referenced
        {
        private:
            FFTOceanTechnique& _oceanSurface;
            const unsigned int _NUMFRAMES;
            Vec3f _eye;
            double _time;
            const unsigned int _FPS;
            double _msPerFrame;
            unsigned int _frame;
            double _oldTime;
            double _newTime;

        public:
            OceanDataType( FFTOceanTechnique& ocean, unsigned int numFrames, unsigned int fps );
            OceanDataType( const OceanDataType& copy);

            inline void setEye( const Vec3f& eye ){ _eye = eye; }
            void updateOcean( double simulationTime );
        };

    public:
        // --------------------------------------------------------
        //  OceanAnimationCallback 
        // --------------------------------------------------------

        /**
        * Cull/Update Animation callback. Override its operator() to customize
        * how the ocean is updated.
        */
   
            virtual void cull(CullVisitor* nv);
 
            virtual void update1(double simulationTime = -1.0);
    
   
            virtual bool handle(int key);
 
            bool _autoDirty;
         
    };
 