/* -*-c++-*- OpenSceneGraph - Copyright (C) 1998-2006 Robert Osfield
 *
 * This library is open source and may be redistributed and/or modified under
 * the terms of the OpenSceneGraph Public License (OSGPL) version 0.0 or
 * (at your option) any later version.  The full license is in LICENSE file
 * included with this distribution, and on the openscenegraph.org website.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * OpenSceneGraph Public License for more details.
*/

#ifndef OSG_FOG
#define OSG_FOG 1

#include <Bullet3Common/geometry.h>
#include <OpenGLWindow/OpenGLInclude.h>


#ifndef GL_FOG_DISTANCE_MODE_NV
    #define GL_FOG_DISTANCE_MODE_NV 0x855A
#endif
#ifndef GL_EYE_PLANE_ABSOLUTE_NV
    #define GL_EYE_PLANE_ABSOLUTE_NV 0x855C
#endif
#ifndef GL_EYE_RADIAL_NV
    #define GL_EYE_RADIAL_NV 0x855B
#endif
  
 
class State;
/** Fog - encapsulates OpenGL fog state. */
class   Fog //: public StateAttribute
{
    public :

        Fog();

        /** Copy constructor using CopyOp to manage deep vs shallow copy.*/
        Fog(const Fog& fog):
            //StateAttribute(fog,copyop),
            _mode(fog._mode),
            _density(fog._density),
            _start(fog._start),
            _end(fog._end),
            _color(fog._color),
            _fogCoordinateSource(fog._fogCoordinateSource),
            _useRadialFog(fog._useRadialFog)    {}
   
        enum Mode {
            LINEAR = GL_LINEAR,
            EXP    = GL_EXP,
            EXP2   = GL_EXP2
        };

        inline void  setMode( Mode mode )         { _mode = mode; }
        inline Mode getMode() const                     { return _mode; }

        inline void  setDensity( float density )  { _density = density; }
        inline float getDensity() const           { return _density; }

        inline void  setStart( float start )      { _start = start; }
        inline float getStart() const             { return _start; }

        inline void  setEnd( float end )          { _end = end; }
        inline float getEnd() const               { return _end; }

        inline void  setColor( const Vec4f &color ) { _color = color; }
        inline const Vec4f& getColor() const        { return _color; }

        inline void setUseRadialFog( bool useRadialFog ) { _useRadialFog = useRadialFog; }
        inline bool getUseRadialFog() const              { return _useRadialFog; }

        enum FogCoordinateSource
        {
            FOG_COORDINATE = GL_FOG_COORDINATE,
            FRAGMENT_DEPTH = GL_FRAGMENT_DEPTH
        };

        inline void setFogCoordinateSource(int source) { _fogCoordinateSource = source; }
        inline int getFogCoordinateSource() const { return _fogCoordinateSource; }

        virtual void apply(State& state) const;
        virtual ~Fog();

    protected :

  
        Mode    _mode;
        float   _density;
        float   _start;
        float   _end;
        Vec4f    _color;
        int   _fogCoordinateSource;
        bool    _useRadialFog;
};

 

#endif
