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

#include "OceanScene.h"

#include "FFTOceanTechnique.h"
#include "OpenGLWindow/LoadShader.h"
#include "Component/Fog.h"
#include <iostream>
#include <fstream>

struct OceanSurfaceProgram :public ProgramBase
{
    GLint osgOcean_EyeUnderwater = -1;
    GLint osgOcean_Eye = -1;
    GLint osgOcean_EnableDOF = -1;
    GLint osgOcean_EnableGlare = -1;
    GLint osgOcean_EnableUnderwaterScattering = -1;
    GLint osgOcean_WaterHeight = -1;
    GLint osgOcean_UnderwaterFogColor = -1;
    GLint osgOcean_AboveWaterFogColor = -1;
    GLint osgOcean_UnderwaterFogDensity = -1;
    GLint osgOcean_AboveWaterFogDensity = -1;
    GLint osgOcean_UnderwaterDiffuse = -1;
    GLint osgOcean_UnderwaterAttenuation = -1;
};
struct OceanSceneProgram :public ProgramBase
{

    GLint  osgOcean_EnableReflections = -1;
    GLint  osgOcean_ReflectionMap = -1;
    GLint  osgOcean_EnableRefractions = -1;
    GLint  osgOcean_RefractionMap = -1;
    GLint  osgOcean_RefractionDepthMap = -1;
    GLint  osgOcean_EnableHeightmap = -1;
    GLint  osgOcean_Heightmap = -1;
    GLint  osgOcean_RefractionInverseTransformation = -1;
    GLint  osgOcean_ViewportDimensions = -1;
     
};


static const float OCEAN_CYLINDER_HEIGHT = 4000.f;
 
OceanScene::OceanScene( void )
    :_oceanSurface               ( 0 )
    ,_isDirty                    ( true )
  
    ,_enableUnderwaterScattering ( false ) 
    ,_reflectionTexSize          ( 512,512 )
    ,_refractionTexSize          ( 512,512 )
    ,_screenDims                 ( 1024,768 )
    ,_sunDirection               ( 0,0,-1 )
    ,_reflectionUnit             ( 1 )
    ,_refractionUnit             ( 2 )
    ,_refractionDepthUnit        ( 3 )
    ,_heightmapUnit              ( 7 )

    ,_lightID                    ( 0 )
    ,_dofNear                    ( 0.f )
    ,_dofFar                     ( 160.f )
    ,_dofFocus                   ( 30.f )
    ,_dofFarClamp                ( 1.f )
    ,_glareThreshold             ( 0.9f )
    ,_glareAttenuation           ( 0.75f )
    ,_underwaterFogColor         ( 0.2274509f, 0.4352941f, 0.7294117f, 1.f )
    ,_underwaterAttenuation      ( 0.015f, 0.0075f, 0.005f)
    ,_underwaterFogDensity       ( 0.01f )
    ,_aboveWaterFogDensity       ( 0.0012f )
    //,_eyeHeightReflectionCutoff  ( FLT_MAX )
    //,_eyeHeightRefractionCutoff  (-FLT_MAX )
    ,_surfaceHeight              ( 0.0f )
    ,_oceanTransform             ( new MultiGeometryShape )
    ,_oceanCylinder              ( new Cylinder(1900.f, OCEAN_CYLINDER_HEIGHT, 16, false, true) )
    //,_oceanCylinderMT            ( new MatrixTransform )
{
    //-----------------------------------------------------------------------
    // _oceanCylinder follows the camera underwater, so that the clear
    // color is not visible past the far plane - it will be the fog color.
    _oceanCylinder->setColor( _underwaterFogColor );
    glDisable(GL_LIGHTING);//_oceanCylinder
    glDisable(GL_FOG);//_oceanCylinder

    addDrawable( _oceanCylinder.get() );

    _oceanCylinder->setMatrix( Matrixf::translate(Vec3f(0, 0, -OCEAN_CYLINDER_HEIGHT)));
 
    //_oceanCylinder->setCullCallback(   &CameraTrack  );
    _oceanCylinder->setNodeMask(_normalSceneMask  | _refractionSceneMask );
  
    //-----------------------------------------------------------------------

    _oceanTransform->setNodeMask( _normalSceneMask | _surfaceMask );
    addDrawable( _oceanTransform.get() );
     

    //setNumChildrenRequiringUpdateTraversal(1);
    
    createDefaultSceneShader();
    //setGlobalDefinition("osgOcean_LightID", _lightID);
}

OceanScene::OceanScene( OceanTechnique* technique, OceanTechnique* riverTechnique )
    :_oceanSurface               ( technique )
	,_riverSurface				( riverTechnique )
    ,_isDirty                    ( true )
  
    ,_enableUnderwaterScattering ( false ) 
    ,_reflectionTexSize          ( 512,512 )
    ,_refractionTexSize          ( 512,512 )
    ,_screenDims                 ( 1024,768 )
    ,_sunDirection               ( 0,0,-1 )
    ,_reflectionUnit             ( 1 )
    ,_refractionUnit             ( 2 )
    ,_refractionDepthUnit        ( 3 )
    ,_heightmapUnit              ( 7 )

    ,_lightID                    ( 0 )
    ,_dofNear                    ( 0.f )
    ,_dofFar                     ( 160.f )
    ,_dofFocus                   ( 30.f )
    ,_dofFarClamp                ( 1.f )
    ,_glareThreshold             ( 0.9f )
    ,_glareAttenuation           ( 0.75f )
    ,_underwaterFogColor         ( 0.2274509f, 0.4352941f, 0.7294117f, 1.f )
    ,_underwaterAttenuation      ( 0.015f, 0.0075f, 0.005f)
    ,_underwaterFogDensity       ( 0.01f )
    ,_aboveWaterFogDensity       ( 0.0012f )
    //,_eyeHeightReflectionCutoff  ( FLT_MAX)
    //,_eyeHeightRefractionCutoff  (-FLT_MAX)
    ,_surfaceHeight              ( 0.0f )
    ,_oceanTransform             ( new MultiGeometryShape)
    ,_oceanCylinder              ( new Cylinder(1900.f, OCEAN_CYLINDER_HEIGHT, 16, false, true) )
    //,_oceanCylinderMT            ( new MatrixTransform )
{
    //-----------------------------------------------------------------------
    // _oceanCylinder follows the camera underwater, so that the clear
    // color is not visible past the far plane - it will be the fog color.
    _oceanCylinder->setColor( _underwaterFogColor );
    glDisable(GL_LIGHTING);//_oceanCylinder
    glDisable(GL_FOG);//_oceanCylinder

    addDrawable( _oceanCylinder.get() );

    _oceanCylinder->setMatrix( Matrixf::translate(Vec3f(0, 0, -OCEAN_CYLINDER_HEIGHT)));
    //_oceanCylinder->setCullCallback(   &CameraTrack  );
    _oceanCylinder->setNodeMask( _normalSceneMask  | _refractionSceneMask );
  
    //-----------------------------------------------------------------------

    _oceanTransform->setNodeMask( _normalSceneMask | _surfaceMask );
    addDrawable( _oceanTransform.get() );

	if (_riverSurface != NULL)
	{
		_riverSurface->setNodeMask( _surfaceMask );
		_oceanTransform->addDrawable( _riverSurface.get() );
	} else {
		_oceanSurface->setNodeMask( _surfaceMask );
		_oceanTransform->addDrawable( _oceanSurface.get() );
	}

 

    //setNumChildrenRequiringUpdateTraversal(1);

    //setGlobalDefinition("osgOcean_LightID", _lightID);
    createDefaultSceneShader();
}
 
OceanScene::~OceanScene( void )
{

}

void OceanScene::init( void )
{
    std::cout << "OceanScene::init()" << std::endl;
     
    if( _reflectionClipNode ){
        ///removeDrawable( _reflectionClipNode.get() );
        _reflectionClipNode = NULL;
    }
      
    if( _oceanSurface || _riverSurface)
    {
        const float LOG2E = 1.442695;

         
        // This is now a #define, added by the call to 
        // ShaderManager::setGlobalDefinition() in the constructors above. 
        // Note that since _lightID can change, we will need to change the
        // global definition and reload all the shaders that depend on its
        // value when it does. This is not implemented yet.
        //addUniform( ("osgOcean_LightID", _lightID ) );
        m_sfprog->osgOcean_EnableDOF = m_sfprog->getLoc("osgOcean_EnableDOF"); //  
        m_sfprog->setUniform(m_sfprog->osgOcean_EnableDOF, false);

        m_sfprog->osgOcean_EnableGlare = m_sfprog->getLoc("osgOcean_EnableGlare"); //  
        m_sfprog->setUniform(m_sfprog->osgOcean_EnableGlare, false);
 
        m_sfprog->osgOcean_EnableUnderwaterScattering = m_sfprog->getLoc("osgOcean_EnableUnderwaterScattering"); //  
        m_sfprog->setUniform(m_sfprog->osgOcean_EnableUnderwaterScattering, _enableUnderwaterScattering);

        m_sfprog->osgOcean_WaterHeight = m_sfprog->getLoc("osgOcean_WaterHeight"); //  
        m_sfprog->setUniform(m_sfprog->osgOcean_WaterHeight, float(getOceanSurfaceHeight()));
        
        m_sfprog->osgOcean_UnderwaterFogColor = m_sfprog->getLoc("osgOcean_UnderwaterFogColor"); //  
        m_sfprog->setUniform(m_sfprog->osgOcean_UnderwaterFogColor, _underwaterFogColor);
         
        m_sfprog->osgOcean_AboveWaterFogColor = m_sfprog->getLoc("osgOcean_AboveWaterFogColor"); //  
        m_sfprog->setUniform(m_sfprog->osgOcean_AboveWaterFogColor, _aboveWaterFogColor);
       
        m_sfprog->osgOcean_UnderwaterFogDensity = m_sfprog->getLoc("osgOcean_UnderwaterFogDensity"); //  
        m_sfprog->setUniform(m_sfprog->osgOcean_UnderwaterFogDensity, -_underwaterFogDensity * _underwaterFogDensity*LOG2E);

  
        m_sfprog->osgOcean_AboveWaterFogDensity = m_sfprog->getLoc("osgOcean_AboveWaterFogDensity"); //  
        m_sfprog->setUniform(m_sfprog->osgOcean_AboveWaterFogDensity, -_aboveWaterFogDensity * _aboveWaterFogDensity*LOG2E);

        m_sfprog->osgOcean_UnderwaterDiffuse = m_sfprog->getLoc("osgOcean_UnderwaterDiffuse"); //  
        m_sfprog->setUniform(m_sfprog->osgOcean_UnderwaterDiffuse, _underwaterDiffuse);

        m_sfprog->osgOcean_UnderwaterAttenuation = m_sfprog->getLoc("osgOcean_UnderwaterAttenuation"); //  
        m_sfprog->setUniform(m_sfprog->osgOcean_UnderwaterAttenuation, _underwaterAttenuation);
         
 
        {
            //( _defaultSceneShader.get(), ON );
        }
         
    }
    
    _isDirty = false;
}
// CameraTrackCallback used by osgOcean with the undersea cylinder.
// Note: only set on MatrixTransform.
void OceanScene::CameraTrack(CullVisitor* cv)
{

    bool follow = true;

    FFTOceanTechnique* oceanTechnique = dynamic_cast<FFTOceanTechnique*>(getOceanTechnique());
    if (oceanTechnique && !oceanTechnique->isEndlessOceanEnabled())
    {
        // Do not follow the eye when the ocean is constrained 
        // to an area.
        follow = false;

        // The ocean cylinder should not be visible if the ocean 
        // is constrained to an area, since the ocean will often 
        // be in a pool or a river which already has walls.
        _oceanCylinder->setNodeMask(0);
    }

    if (follow)
    {
        Vec3f centre, up, eye;
        // get MAIN camera eye,centre,up
        cv->getViewMatrixAsLookAt(eye, centre, up);

        bool eyeAboveWater = isEyeAboveWater(eye);
        float mult = 1.0;
        if (eyeAboveWater) mult = -1.0;

        // Translate the ocean cylinder down by the surface height 
        // if the eye went from below to above the surface (so the 
        // cylinder doesn't peek through the waves) and inversely
        // when the eye goes from above to below the surface (so
        // we don't see cracks between the cylinder's edge and the
        // waves).

        // Note, we set the ocean cylinder's own matrixTransform 
        // to identity, and push the relevant matrix onto the 
        // modelView matrix stack, because it is viewpoint 
        // dependent.
        _oceanCylinder->setMatrix(Matrixf::identity());

        double z = -getOceanCylinder()->getHeight() +                      // So the cylinder is underwater
            getOceanSurfaceHeight() +                              // Follow the ocean surface's height
            mult * getOceanTechnique()->getMaximumHeight();        // Offset either up or down by a bit.
        Matrixf cylinderMatrix = (Matrixf::translate(Vec3f(eye.x, eye.y, z)) * *cv->getViewMatrix());
        cv->pushModelViewMatrix(cylinderMatrix);
    }

    //traverse(node, cv);

    if (follow)
    {
        cv->popModelViewMatrix();
    }

};

OceanScene::ViewData* OceanScene::getViewDependentData( CullVisitor * cv )
{   //scoped_lock c++17
   std::unique_lock<std::mutex> lock(_viewDataMapMutex);
    return _viewDataMap[ cv ].get();
}

void OceanScene::setViewDependentData( CullVisitor * cv, ViewData * data )
{   //scoped_lock c++17
   std::unique_lock<std::mutex> lock(_viewDataMapMutex);
    _viewDataMap[ cv ] = std::shared_ptr<ViewData>(data);
}

OceanScene::ViewData * OceanScene::initViewDependentData( CullVisitor *cv, OceanScene::ViewData * vd )
{
    ViewData* viewData = vd;
    if (!viewData) viewData = new ViewData;
    viewData->init( this, cv );
    return viewData;
}

void OceanScene::ViewData::dirty( bool flag )
{
   std::unique_lock<std::mutex> lock(_mutex);
    _dirty = flag;
}
void OceanScene::initProg( )
{
    m_sfprog->osgOcean_EyeUnderwater = m_sfprog->getLoc("osgOcean_EyeUnderwater"); //  
    m_sfprog->setUniform(m_sfprog->osgOcean_EyeUnderwater, false);

    m_sfprog->osgOcean_Eye = m_sfprog->getLoc("osgOcean_Eye"); //  
    m_sfprog->setUniform(m_sfprog->osgOcean_Eye, Vec3f());
    //_surfaceStateSet

    m_prog->osgOcean_EnableReflections = m_prog->getLoc("osgOcean_EnableReflections"); //  
    m_prog->setUniform(m_prog->osgOcean_EnableReflections, false);

    m_prog->osgOcean_ReflectionMap = m_prog->getLoc("osgOcean_ReflectionMap"); //  
    m_prog->setUniform(m_prog->osgOcean_ReflectionMap, _reflectionUnit);

    m_prog->osgOcean_EnableRefractions = m_prog->getLoc("osgOcean_EnableRefractions"); //  
    m_prog->setUniform(m_prog->osgOcean_EnableRefractions, false);

    m_prog->osgOcean_RefractionMap = m_prog->getLoc("osgOcean_RefractionMap"); //  
    m_prog->setUniform(m_prog->osgOcean_RefractionMap, _refractionUnit);

    m_prog->osgOcean_RefractionDepthMap = m_prog->getLoc("osgOcean_RefractionDepthMap"); //  
    m_prog->setUniform(m_prog->osgOcean_RefractionDepthMap, _refractionDepthUnit);

    m_prog->osgOcean_EnableHeightmap = m_prog->getLoc("osgOcean_EnableHeightmap"); //  
    m_prog->setUniform(m_prog->osgOcean_EnableHeightmap, false);

    m_prog->osgOcean_Heightmap = m_prog->getLoc("osgOcean_Heightmap"); //  
    m_prog->setUniform(m_prog->osgOcean_Heightmap, _heightmapUnit);

    m_prog->osgOcean_RefractionInverseTransformation = m_prog->getLoc("osgOcean_RefractionInverseTransformation"); //  
    m_prog->setUniformMatrix4(m_prog->osgOcean_RefractionInverseTransformation, Matrixf::identity());

    m_prog->osgOcean_ViewportDimensions = m_prog->getLoc("osgOcean_ViewportDimensions"); //  
    m_prog->setUniform(m_prog->osgOcean_ViewportDimensions, Vec2f(_screenDims.x, _screenDims.y));
}
 
void OceanScene::ViewData::init( OceanScene *oceanScene, CullVisitor * cv )
{
    _cv = cv;
    _oceanScene = oceanScene;
    //_globalStateSet
    _oceanScene->initProg();
    _fog.reset( new Fog);
    _fog->setMode(Fog::EXP2);
    _fog->setDensity(_oceanScene->_aboveWaterFogDensity);
    _fog->setColor(_oceanScene->_aboveWaterFogColor);
    //_globalStateSet (_fog.get(), ON);
     
    dirty( false );
}

void OceanScene::ViewData::updateStateSet( bool eyeAboveWater )
{ 
    //_globalStateSet
 
    _oceanScene->m_sfprog->setUniform(_oceanScene->m_sfprog->osgOcean_EyeUnderwater, !eyeAboveWater);
     
    _oceanScene->m_sfprog->setUniform(_oceanScene->m_sfprog->osgOcean_Eye, _cv->getEyePoint());
  
    // Switch the fog state from underwater to above water or vice versa if necessary.
    float requiredFogDensity = eyeAboveWater ? _oceanScene->_aboveWaterFogDensity : _oceanScene->_underwaterFogDensity;
    Vec4f requiredFogColor = eyeAboveWater ? _oceanScene->_aboveWaterFogColor   : _oceanScene->_underwaterFogColor;
    if (requiredFogDensity != _fog->getDensity() || requiredFogColor != _fog->getColor())
    {
        _fog->setDensity(requiredFogDensity);
        _fog->setColor  (requiredFogColor);
    }

    // Update viewport dimensions
    //_surfaceStateSet
    Vec4f viewport = _cv->getViewport();
    _oceanScene->m_prog->setUniform(_oceanScene->m_prog->osgOcean_ViewportDimensions, Vec2f(viewport[2], viewport[3]) );

    // Check if the RTT passes should be enabled for this view.
    bool enabled = true;//(_oceanScene->_viewsWithRTTEffectsDisabled.find(cv->getView()) == _oceanScene->_viewsWithRTTEffectsDisabled.end());

    bool reflectionEnabled = false && eyeAboveWater && enabled &&
        false;//( _cv->getEyePoint().z < _oceanScene->_eyeHeightReflectionCutoff - _oceanScene->getOceanSurfaceHeight() );
    _oceanScene->m_prog->setUniform(_oceanScene->m_prog->osgOcean_EnableReflections, reflectionEnabled);

    if (reflectionEnabled)
    {
        // Update the reflection matrix's translation to take into account
        // the ocean surface height. The translation we need is 2*h.
        // See http://www.gamedev.net/columns/hardcore/rnerwater1/page3.asp
        float m[4][4] = { {1,  0,  0,  0},
                    {0,  1,  0,  0},
                    {0,  0, -1,  0},
                    {0,  0,  2 * _oceanScene->getOceanSurfaceHeight(),  1} };
        _reflectionMatrix = Matrixf(m);
    }

    // Refractions need to be calculated even when the eye is above water 
    // for the shoreline foam effect and translucency.
    bool refractionEnabled = false && enabled;
    _oceanScene->m_prog->setUniform(_oceanScene->m_prog->osgOcean_EnableRefractions, refractionEnabled);

    bool heightmapEnabled = false && eyeAboveWater && enabled;
    _oceanScene->m_prog->setUniform(_oceanScene->m_prog->osgOcean_EnableHeightmap, heightmapEnabled);
}

void OceanScene::ViewData::cull( bool eyeAboveWater, bool surfaceVisible )
{
    // Assume _stateSet has been pushed before we get here.
       
}
 
void OceanScene::cull(CullVisitor* cv)
{ 
   
        if (cv) 
        { 
            if (cv->isShadowMap()) // MinimalDrawBoundsShadowMap 
            {
                // Do not do reflections and everything if we're in a shadow pass.
                //traverse(nv);
            }
            else
            {
                bool eyeAboveWater  = isEyeAboveWater(cv->getEyePoint());

                // Push the stateset for view-dependent RTT effects.
                ViewData * vd = getViewDependentData( cv );

                if ( !vd || vd->_dirty || vd->_cv != cv || vd->_oceanScene != this )
                {
                    vd = initViewDependentData( cv, vd );
                    setViewDependentData( cv, vd );
                }

                if (vd)
                {
                    vd->updateStateSet(eyeAboveWater);
                    //cv->pushStateSet( );
                }

                bool surfaceVisible = _oceanSurface->isVisible(*cv, eyeAboveWater);
 
                _oceanSurface->cull(cv);

				if (_riverSurface != NULL)
					_riverSurface->cull(cv);
 

                preRenderCull(*cv, eyeAboveWater, surfaceVisible);     // reflections/refractions
                
                // Above water
                if( eyeAboveWater )
                {
                   
                        cull(*cv, eyeAboveWater, surfaceVisible);        // normal scene render
                }
                // Below water passes
                else 
                {
                   
                        cull(*cv, eyeAboveWater, surfaceVisible);        // normal scene render
                }

                postRenderCull(*cv, eyeAboveWater, surfaceVisible);    // god rays/dof/glare

                if (vd)
                {
                    //cv->popStateSet();
                }
            }
        }
 
   
 
}

void OceanScene::update(unsigned frameNum, double currentTime)
{
 
}

void OceanScene::preRenderCull( CullVisitor& cv, bool eyeAboveWater, bool surfaceVisible )
{ 
    // Render all view-dependent RTT effects.
    ViewData * vd = getViewDependentData( &cv );

    if( vd )
    {
        std::unique_lock<std::mutex> lock(vd->_mutex);
        vd->cull(eyeAboveWater, surfaceVisible);
    }

    // Now render other effects.

    // Above water
    if( eyeAboveWater )
    {
  
    }
    // Below water
    else
    {
   
    }
}

void OceanScene::postRenderCull( CullVisitor& cv, bool eyeAboveWater, bool surfaceVisible )
{
    if( eyeAboveWater )
    {
 
    }
    else
    {
 
    }
}

void OceanScene::cull(CullVisitor& cv, bool eyeAboveWater, bool surfaceVisible)
{
    unsigned int mask = cv.getTraversalMask();

    //cv.pushStateSet( );

    if ( _oceanSurface  && _oceanSurface->getNodeMask() != 0 && surfaceVisible )
    {
        // HACK: Make sure masks are set correctly on children.. This 
        // assumes that the ocean surface is the only child that should have
        // the _surfaceMask bit set, and the silt node is the only child that
        // should have the _siltMask bit set. Otherwise other children will be
        // rendered twice.
        for (unsigned int i = 0; i < getNumDrawables(); ++i)
        {
            GeometryShape* child = getDrawable(i);
            if (child->getNodeMask() != 0 && child != _oceanTransform.get() )
                child->setNodeMask((child->getNodeMask() & ~_surfaceMask & ~_siltMask) | _normalSceneMask | _reflectionSceneMask | _refractionSceneMask);
        }

        // Push the view-dependent surface stateset.
        ViewData * vd = getViewDependentData( &cv );
        if (vd)
        {
            //cv.pushStateSet(  );
        }

        cv.setTraversalMask( mask & _surfaceMask );
        //traverse(cv);

        if (vd)
        {
            //cv.popStateSet();
        }
    }

    // render rest of scene
    cv.setTraversalMask( mask & _normalSceneMask );
    //traverse(cv);

    // pop globalStateSet
    //cv.popStateSet(); 

    if( !eyeAboveWater )
    {
 
    }

    // put original mask back
    cv.setTraversalMask( mask );
}

bool OceanScene::isEyeAboveWater( const Vec3f& eye )
{
    return (eye.z >= getOceanSurfaceHeight());
}


void OceanScene::setDOFNear(float dofNear)
{
    _dofNear = dofNear;
    //_dofStateSet
    //m_prog->setUniform(m_prog->osgOcean_DOF_Near, _dofNear);
}


void OceanScene::setDOFFar(float dofFar)
{
    _dofFar = dofFar;
    //_dofStateSet
    //m_prog->setUniform(m_prog->osgOcean_DOF_Far, _dofFar);
}


void OceanScene::setDOFFarClamp(float farClamp)
{
    _dofFarClamp = farClamp;

    //_dofStateSet
    //m_prog->setUniform(m_prog->osgOcean_DOF_Clamp, _dofFarClamp);
}


void OceanScene::setDOFFocalDistance(float focus)
{
    _dofFocus = focus;
    //_dofStateSet
    //m_prog->setUniform(m_prog->osgOcean_DOF_Focus, _dofFocus);
}

void OceanScene::setAboveWaterFog(float density, const Vec4f& color)
{
    _aboveWaterFogDensity = density;
    _aboveWaterFogColor = color;

    const float LOG2E = 1.442695;
    //if( _globalStateSet.valid() )
    {
        m_sfprog->setUniform(m_sfprog->osgOcean_AboveWaterFogDensity, -_aboveWaterFogDensity * _aboveWaterFogDensity*LOG2E);
        m_sfprog->setUniform(m_sfprog->osgOcean_AboveWaterFogColor, color);
    }

    _isDirty = true;
}


void OceanScene::setUnderwaterFog(float density, const Vec4f& color)
{
    _underwaterFogDensity = density;
    _underwaterFogColor = color;
    _oceanCylinder->setColor(_underwaterFogColor);

    const float LOG2E = 1.442695;
    //if( _globalStateSet.valid() )
    {
        m_sfprog->setUniform(m_sfprog->osgOcean_UnderwaterFogDensity, -_underwaterFogDensity * _underwaterFogDensity*LOG2E);
        m_sfprog->setUniform(m_sfprog->osgOcean_UnderwaterFogColor, _underwaterFogColor);
    }

    _isDirty = true;
}
 
 
Texture2D* OceanScene::createTexture2D( const Vec2s& size, GLint format )
{
    Texture2D* texture = new Texture2D;
    texture->setTextureSize(size.x, size.y);
    texture->setInternalFormat(format);
    texture->setFilter(Texture2D::MIN_FILTER, format == GL_DEPTH_COMPONENT ? Texture2D::NEAREST : Texture2D::LINEAR );
    texture->setFilter(Texture2D::MAG_FILTER, format == GL_DEPTH_COMPONENT ? Texture2D::NEAREST : Texture2D::LINEAR );
    texture->setWrap(Texture::WRAP_S, Texture::CLAMP_TO_EDGE );
    texture->setWrap(Texture::WRAP_T, Texture::CLAMP_TO_EDGE );
    //texture->setDataVariance(DYNAMIC);
    return texture;
}

TextureRectangle* OceanScene::createTextureRectangle( const Vec2s& size, GLint format )
{
    TextureRectangle* texture = new TextureRectangle();
    texture->setTextureSize(size.x, size.y);
    texture->setInternalFormat(format);
    texture->setFilter(Texture2D::MIN_FILTER,Texture2D::LINEAR);
    texture->setFilter(Texture2D::MAG_FILTER,Texture2D::LINEAR);
    texture->setWrap(Texture::WRAP_S, Texture::CLAMP_TO_EDGE );
    texture->setWrap(Texture::WRAP_T, Texture::CLAMP_TO_EDGE );
    //texture->setDataVariance(DYNAMIC);
    return texture;
}

GeometryShape* OceanScene::createScreenQuad( const Vec2s& dims, const Vec2s& texSize )
{ 
    GeometryShape* quad = createTexturedQuadGeometry(
        Vec3f(0.f,0.f,0.f), 
        Vec3f(dims.x, 0.f, 0.f), 
        Vec3f( 0.f,dims.y, 0.0 ),
        (float)texSize.x,
        (float)texSize.y );
     
    return quad;
}

#include <shaders/osgOcean_ocean_scene_vert.inl>
#include <shaders/osgOcean_ocean_scene_frag.inl>

void OceanScene::createDefaultSceneShader(void)
{
    static const char osgOcean_ocean_scene_vert_file[] = "osgOcean_ocean_scene.vert";
    static const char osgOcean_ocean_scene_frag_file[] = "osgOcean_ocean_scene.frag";
    m_prog = new OceanSceneProgram;
    m_sfprog = new OceanSurfaceProgram;
    m_prog->create( osgOcean_ocean_scene_vert,      osgOcean_ocean_scene_frag);
}
 
// -------------------------------
//     Callback implementations
// -------------------------------
 
void OceanScene::CameraCull(CullVisitor* cv)
{
 
}

void OceanScene::PrerenderCameraCull(CullVisitor* cv)
{  
    if(cv)
    {
        bool eyeAboveWater  = isEyeAboveWater(cv->getEyePoint());
        bool surfaceVisible = getOceanTechnique()->isVisible(*cv, eyeAboveWater);
        cull(*cv, eyeAboveWater, surfaceVisible);
    }
}



bool OceanScene::handle(int key)
{
        //(KEYUP):
        {
            // Reflections
            if (key == 'r')
            { 
                return true;
            }
            // Refractions
            if (key == 'R')
            { 
                return true;
            }
            // DOF
            if (key == 'o')
            { 
                return true;
            }
            // Glare
            if (key == 'g')
            { 
                return true;
            }
            // God rays
            if (key == 'G')
            { 
                return true;
            }
            // Silt
            if (key == 't')
            { 
                return true;
            }
            // Underwater scattering
            if (key == 'T')
            {
                enableUnderwaterScattering(!isUnderwaterScatteringEnabled());
                std::cout << "Underwater scattering " << (isUnderwaterScatteringEnabled()? "enabled" : "disabled") << std::endl;
                return true;
            }
            // Height map
            if (key == 'H')
            { 
                return true;
            }
            // Ocean surface height
            if (key == '+')
            {
                setOceanSurfaceHeight(getOceanSurfaceHeight() + 1.0);
                std::cout << "Ocean surface is now at z = " << getOceanSurfaceHeight() << std::endl;
                return true;
            }
            if (key == '-')
            {
                setOceanSurfaceHeight(getOceanSurfaceHeight() - 1.0);
                std::cout << "Ocean surface is now at z = " << getOceanSurfaceHeight() << std::endl;
                return true;
            }
             
        }
 

    return false;
}
  