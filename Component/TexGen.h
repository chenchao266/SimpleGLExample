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

#ifndef OSG_TEXGEN
#define OSG_TEXGEN 1

#include <Bullet3Common/Plane.h>
#include <OpenGLWindow/OpenGLInclude.h>
 
class State;
/** TexGen encapsulates the OpenGL glTexGen (texture coordinate generation)
  * state.*/
class   TexGen  
{
    public :

        TexGen();

        /** Copy constructor using CopyOp to manage deep vs shallow copy. */
        TexGen(const TexGen& texgen):
            //StateAttribute(texgen),
            _mode(texgen._mode),
            _plane_s(texgen._plane_s),
            _plane_t(texgen._plane_t),
            _plane_r(texgen._plane_r),
            _plane_q(texgen._plane_q) {}

        virtual void apply(State& state) const;

        enum Mode {
            OBJECT_LINEAR  = GL_OBJECT_LINEAR,
            EYE_LINEAR     = GL_EYE_LINEAR,
            SPHERE_MAP     = GL_SPHERE_MAP,
            NORMAL_MAP     = GL_NORMAL_MAP,
            REFLECTION_MAP = GL_REFLECTION_MAP
        };

        inline void setMode( Mode mode ) { _mode = mode; }

        Mode getMode() const { return _mode; }

        enum Coord {
          S, T, R, Q
        };

        void setPlane(Coord which, const Plane& plane);

        Plane& getPlane(Coord which);

        const Plane& getPlane(Coord which) const;

        /** Set the tex gen planes from specified matrix.
          * Typical usage would be to pass in a projection
          * matrix to set up projective texturing.
          */
        void setPlanesFromMatrix(const Matrixd& matrix);
        virtual ~TexGen(void);

    protected :

    
        Mode _mode;

        /** Additional texgen coefficients for GL_OBJECT_PLANE or
          * GL_EYE_PLANE, */
        Plane _plane_s, _plane_t, _plane_r, _plane_q;

};



#endif
