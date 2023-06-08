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

#pragma once
 
#include "OceanTechnique.h"
//#include "GodRayBlendSurface.h"
#include "DistortionSurface.h"
//#include "GodRays.h"
//#include <SiltEffect.h>
#include <Component/Cylinder.h>
 
 
 
#include <Component/Texture2D.h>
#include <Component/TextureRectangle.h>
 
#include <Component/CullVisitor.h>
 
//#include <Component/ClipNode.h>
#include <Component/ClipPlane.h>
 

#include <map>
#include <mutex>
class Camera;
class Fog;
struct ProgramBase;
struct OceanSceneProgram;
struct OceanSurfaceProgram;
    /**
    * Controls the render passes required for the ocean scene effects.
    * Uses a series of traversal masks to control which passes a child is subjected to.
    * @note Requires an \c OceanTechnique to be added.
    */
    class  OceanScene : public MultiGeometryShape
    {
    public:
 
        enum MaskE
        {
            _reflectionSceneMask = 0x1,  // 1
            _refractionSceneMask = 0x2,  // 2
            _normalSceneMask = 0x4,  // 4
            _surfaceMask = 0x8,  // 8
            _siltMask = 0x10, // 16
            _heightmapMask = 0x20, // 32
        };
    private:
        std::shared_ptr<OceanTechnique> _oceanSurface;
		std::shared_ptr<OceanTechnique> _riverSurface;

        bool _isDirty;

 
        bool _enableUnderwaterScattering;
  
        Vec2s _reflectionTexSize;
        Vec2s _refractionTexSize;
        Vec2s _screenDims;

        int _reflectionUnit;
        int _refractionUnit;
        int _refractionDepthUnit;
        int _heightmapUnit;

        float      _aboveWaterFogDensity;
        Vec4f _aboveWaterFogColor;

        float      _underwaterFogDensity;
        Vec4f _underwaterFogColor;
        Vec4f _underwaterDiffuse;
        Vec3f _underwaterAttenuation;

        Vec3f _sunDirection;
          
        //std::shared_ptr<GodRayBlendSurface> _godRayBlendSurface;
        //std::shared_ptr<DistortionSurface> _distortionSurface;
        //std::shared_ptr<GodRays> _godrays;
 
        std::shared_ptr<ClipPlane> _reflectionClipNode;
         

        unsigned int _lightID;

        float _dofNear;
        float _dofFar;
        float _dofFarClamp;
        float _dofFocus;
        float _glareThreshold;
        float _glareAttenuation;

        //float _eyeHeightReflectionCutoff;
        //float _eyeHeightRefractionCutoff;

        float _surfaceHeight;
        std::shared_ptr<MultiGeometryShape>  _oceanTransform;
        //std::shared_ptr<MatrixTransform>  _oceanCylinderMT;
        std::shared_ptr<Cylinder>              _oceanCylinder;
 
        struct ViewData //: public Referenced
        {
            /// Simple constructor zeroing all variables.
            ViewData()
                : _dirty( true )
                , _cv( NULL )
                , _oceanScene( NULL )
 
                , _fog(NULL)
                , _eyeAboveWaterPreviousFrame(true)
                //, _globalStateSet(NULL)
                //, _surfaceStateSet(NULL)
            { };

            /// Method called upon ViewData instance to initialize internal variables
            virtual void init( OceanScene* oceanScene, CullVisitor* cv );

            virtual void updateStateSet( bool eyeAboveWater );

            /// Method called by OceanScene to allow ViewData 
            /// do the hard work computing reflections/refractions for its associated view
            virtual void cull( bool eyeAboveWater, bool surfaceVisible );

            /// Dirty is called by parent OceanScene to force 
            /// update of resources after some of them were modified in parent scene
            virtual void dirty( bool flag );

            /// View's CullVisitor associated with this ViewData instance 
             CullVisitor * _cv;

            /// Parent OceanScene 
             OceanScene * _oceanScene;

            /// Mutex used to guard _dirty flag from override in case when parent technique calls 
            /// dirty() simultaneously with ViewData while it is updating resources inside init method.
            std::mutex _mutex;

            /// Dirty flag tells this instance to update its resources 
            bool _dirty;

            Matrixf _reflectionMatrix;
 
            std::shared_ptr<Fog> _fog;
            bool _eyeAboveWaterPreviousFrame;

            //std::shared_ptr<StateSet> _globalStateSet;
            //std::shared_ptr<StateSet> _surfaceStateSet;

            friend class OceanScene;
        };

        /// Map of view dependent data per view cull visitor (CVs are used as indices) 
        /// ViewDependentShadowTechnique uses this map to find VieData for each cull vitior
        typedef std::map< CullVisitor *, std::shared_ptr< ViewData > > ViewDataMap;

        ViewDataMap     _viewDataMap;


        /// Mutex used to serialize accesses to ViewDataMap
        std::mutex      _viewDataMapMutex;

        /// Return view dependent data for the cull visitor
        OceanScene::ViewData * getViewDependentData( CullVisitor * cv );

        /// Define view dependent data for the cull visitor
        void setViewDependentData( CullVisitor * cv, OceanScene::ViewData * data );

        ViewData * initViewDependentData( CullVisitor *cv, OceanScene::ViewData * vd );
        void initProg();
        OceanSceneProgram* m_prog;
        OceanSurfaceProgram* m_sfprog;


        // NOTE: Remember to add new variables to the copy constructor.
    public:
  
        OceanScene(void);
        OceanScene( OceanTechnique* technique, OceanTechnique* riverTechnique );
        virtual ~OceanScene(void);
         
        /// Sets up statesets and render passes based on current settings
        /// Called in the update traversal if dirty flag is set.
        void init( void );

        /// Check if the eye is above water or not.
        bool isEyeAboveWater( const Vec3f& eye );

        /// Set ocean surface height in world space (default is 0.0)
        void setOceanSurfaceHeight(float height){
            _surfaceHeight = height;
            _oceanTransform->setMatrix(Matrixf::translate(Vec3f(0,0,_surfaceHeight - _oceanSurface->getSurfaceHeight())));
            _isDirty = true;
        }

        /// Get ocean surface average height in world space.
        double getOceanSurfaceHeight() const{
            return _surfaceHeight + _oceanSurface->getSurfaceHeight();
        }

        /// Get height of given (x,y) point in world space. Optionally returns the normal.
        float getOceanSurfaceHeightAt(float x, float y, Vec3f* normal = 0)
        {
            return _surfaceHeight + 
                   _oceanSurface->getSurfaceHeightAt(x, y, normal);
        }

        /// Set the ocean surface world-space position. Note that the (x,y) 
        /// components of the translation are of no consequence if the ocean
        /// surface is infinite, since the surface will follow the eye.
        void setOceanSurfaceTransform(const Matrixf& transform)
        {
            _surfaceHeight = transform.getTranslate().z;
            _oceanTransform->setMatrix(Matrixf::translate(transform.getTranslate() - Vec3f(0,0,_oceanSurface->getSurfaceHeight())));
            _isDirty = true;
        }

        /// Returns the world-space position of the ocean surface. Note that 
        /// the (x,y) components of the translation are of no consequence if 
        /// the ocean surface is infinite, since the surface will follow the 
        /// eye.
        Matrixf getOceanSurfaceTransform() const
        {
            return Matrixf::translate(Vec3f(_oceanTransform->getMatrix().getTranslate().x,
                                          _oceanTransform->getMatrix().getTranslate().y,
                                          _surfaceHeight + _oceanSurface->getSurfaceHeight()));
        }

        /// Set whether the ocean surface is visible or not.
        void setOceanVisible(bool visible){
            _oceanTransform->setNodeMask( visible ? _normalSceneMask | _surfaceMask : 0 );
        }

        /// Check whether the ocean surface is visible or not.
        bool isOceanVisible() const { return _oceanTransform->getNodeMask() != 0; }

        /// Get the ocean cylinder.
        Cylinder* getOceanCylinder() const{
            return _oceanCylinder.get();
        }

        /// Get the ocean cylinder's transform node.
 

        /// Set the size of _oceanCylinder which follows the camera underwater, so that the clear
        /// color is not visible past the far plane - it will be the fog color.
        /// Height is a positive number which represents depth.
        /// Default values are Radius: 1900 and Height: 4000
        inline void setCylinderSize( float radius, float height ){
            _oceanCylinder->build( radius, height, 16, false, true );
            //_oceanCylinderMT->setMatrix( Matrixf::translate(Vec3f(0.f,0.f,-height-0.2f)) );
        }

        /// Get the radius of the ocean cylinder.
        inline const float getCylinderRadius( void ) const{
            return _oceanCylinder->getRadius();
        }

        /// Get the height of the ocean cylinder.
        inline const float getCylinderHeight( void ) const{
            return _oceanCylinder->getHeight();
        }
         
         
        /// Set reflection texture size (must be 2^n)
        inline void setReflectionTextureSize( const Vec2s& size ){
            if( size.x != size.y )
                return;
            _reflectionTexSize = size;
            _isDirty = true;
        }
           
        /// Set refraction texture size (must be 2^n)
        inline void setRefractionTextureSize( const Vec2s& size){
            if( size.x != size.y )
                return;
            _refractionTexSize = size;
            _isDirty = true;
        }
          
        /// Sets the current screen size, needed to initialise the God Ray 
        /// and DOF frame buffers. Default is 1024x768.
        inline void setScreenDims( Vec2s size ){
            _screenDims = size;
            _isDirty = true;
        }

        /// Set sun direction.
        inline void setSunDirection( const Vec3f& sunDir ){
            _sunDirection = sunDir;
            _isDirty = true;
        }

        /// Get sun direction.
        inline Vec3f getSunDirection() const{
            return _sunDirection;
        }
 

        /// Set near DOF blur distance.
        void setDOFNear( float dofNear );

        /// Get near DOF blur distance.
        inline float getDOFNear() const{
            return _dofNear;
        }

        /// Set far DOF blur distance
        void setDOFFar( float dofFar );

        /// Get far DOF blur distance.
        inline float getDOFFar() const{
            return _dofFar;
        }

        /// Set far clamp value.
        void setDOFFarClamp( float farClamp );

        /// Get far clamp value.
        inline float getDOFFarClamp() const{
            return _dofFarClamp;
        }

        /// Set DOF focal distance.
        void setDOFFocalDistance( float focus );

        /// Get DOF focal distance.
        inline float getDOFFocalDistance() const{
            return _dofFocus;
        }
  

        /// Set the luminance threshold for glare.
        /// Luminance value at which glare appears.
        /// Typical range: 0.75 < threshold < 1.0
        inline void setGlareThreshold( float threshold )
        {
            _glareThreshold = threshold;
            _isDirty = true;
        }

        /// Get the luminance threshold for glare.
        inline float getGlareThreshold() const{
            return _glareThreshold;
        }

        /// Set the glare attenuation.
        /// Controls the rate at which the glare drops off.
        /// Typical range: 0.75 < attenuation < 0.95
        inline void setGlareAttenuation( float attenuation )
        {
            _glareAttenuation = attenuation;
            _isDirty = true;
        }

        /// Get the glare attenuation.
        inline float getGlareAttenuation() const{
            return _glareAttenuation;
        }
          
        /// Enable underwater scattering.
        inline void enableUnderwaterScattering( bool flag )
        {
            _enableUnderwaterScattering = flag;
            _isDirty = true;
        }

        /// Check whether underwater scattering is enabled.
        inline bool isUnderwaterScatteringEnabled() const{
            return _enableUnderwaterScattering;
        }

        /// Set the ocean technique.
        inline void setOceanTechnique( OceanTechnique* surface ){

            if( _oceanSurface )
                _oceanTransform->removeDrawable( _oceanSurface.get() );

            _oceanSurface.reset( surface);

            if (_oceanSurface)
            {
                _oceanSurface->setNodeMask( _surfaceMask );

                _oceanTransform->addDrawable( _oceanSurface.get() );
            }

            _isDirty = true;
        }

        /// Get the current ocean technique.
        inline OceanTechnique* getOceanTechnique( void ) {
            return _oceanSurface.get();
        }
  
  
        /// Set the ID of the light source that should be used to light the ocean.
        inline void setLightID( unsigned int id ){
            _lightID = id;
            _isDirty = true;
        }

        /// Sets the fogging params for the above water scene.
        /// EXP fog
        void setAboveWaterFog( float density, const Vec4f& color );

        /// Get the above water fog density.
        inline float getAboveWaterFogDensity() const
        {
            return _aboveWaterFogDensity;
        }

        /// Get the above water fog color.
        inline Vec4f getAboveWaterFogColor() const
        {
            return _aboveWaterFogColor;
        }

        /// Sets the fogging params for the underwater scene.
        /// EXP2 fog
        void setUnderwaterFog( float density, const Vec4f& color );

        /// Get the underwater fog density.
        inline float getUnderwaterFogDensity() const
        {
            return _underwaterFogDensity;
        }

        /// Get the underwater fog color.
        inline Vec4f getUnderwaterFogColor() const
        {
            return _underwaterFogColor;
        }

        /// Changes the color of diffuse light used underwater.
        /// @note Should be computing this from physical calculations. For 
        /// the moment this is tweaked by hand.
        /// see: http://citeseer.ist.psu.edu/cache/papers/cs/26265/http:zSzzSzwww.cs.sunysb.eduzSz~ashzSzwaterCGF.pdf/
        void setUnderwaterDiffuse( const Vec4f& diffuse )
        {
            _underwaterDiffuse = diffuse;
            _isDirty = true;
        }

        /// Get the color of the diffuse light underwater.
        Vec4f getUnderwaterDiffuse() const
        {
            return _underwaterDiffuse;
        }

        /// Change the attenuation of light underwater.
        /// @note Should be computing this from physical calculations. For 
        ///the moment this is tweaked by hand.
        /// see: http://citeseer.ist.psu.edu/cache/papers/cs/26265/http:zSzzSzwww.cs.sunysb.eduzSz~ashzSzwaterCGF.pdf/
        void setUnderwaterAttenuation( const Vec3f& attenuation )
        {
            _underwaterAttenuation = attenuation;
            _isDirty = true;
        }

        /// Get the attenuation of light underwater.
        Vec3f getUnderwaterAttenuation() const
        {
            return _underwaterAttenuation;
        }
          
        /// Base class for the OceanScene event handler. Subclasses of
        /// OceanScene can subclass this to provide support for
        /// manipulating their particular properties, calling the base class
        /// handle() to inherit the base class's events (or not as desired).
        /// If subclasses subclass this handler, they need to override
        /// getEventHandler() in order for it to return the right concrete
        /// event handler instance.
 
            virtual bool handle(int key);
            void CameraTrack(CullVisitor* cv);

    private:
        Texture2D* createTexture2D( const Vec2s& size, GLint format );
        TextureRectangle* createTextureRectangle( const Vec2s& size, GLint format );

        /// Override OSG traversal function in order to do custom rendering.
        void traverse( );

        /// Main cull traversal.
        /// Renders main scene, surface, silt.
        void cull( CullVisitor& cv, bool eyeAboveWater, bool surfaceVisible );

        /// Render to texture passes for reflection/refractions/height map/god rays.
        void preRenderCull( CullVisitor& cv, bool eyeAboveWater, bool surfaceVisible );

        /// Post render passes for DOF/god rays.
        void postRenderCull( CullVisitor& cv, bool eyeAboveWater, bool surfaceVisible );
         
        /// Create geometry for a screen aligned quad.
        GeometryShape* createScreenQuad( const Vec2s& dims, const Vec2s& texSize );

        void createDefaultSceneShader( void );

        void CameraCull(CullVisitor* cv);
        void PrerenderCameraCull(CullVisitor* cv);
        /// Add the default resource paths to osgDB::Registry.
        /// Checks if already present.
        /// paths: resources/textures and resources/shaders.
    
    protected:
 

        virtual void cull(CullVisitor* cv);
        virtual void update(unsigned frameNum, double currentTime);

      
    };
 
