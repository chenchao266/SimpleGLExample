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

#ifndef OSG_CLIPPLANE
#define OSG_CLIPPLANE 1

#include <Bullet3Common/geometry.h>
#include <Bullet3Common/Plane.h>
 

 
class State;
/** Encapsulates OpenGL glClipPlane().
*/
class   ClipPlane  
{
    public :

        ClipPlane();
        inline ClipPlane(unsigned int no):_clipPlaneNum(no) {}
        inline ClipPlane(unsigned int no,const Vec4d& plane):_clipPlaneNum(no) { setClipPlane(plane); }
        inline ClipPlane(unsigned int no,const Plane& plane):_clipPlaneNum(no) { setClipPlane(plane); }
        inline ClipPlane(unsigned int no,double a,double b,double c,double d): _clipPlaneNum(no) { setClipPlane(a,b,c,d); }

        /** Copy constructor using CopyOp to manage deep vs shallow copy. */
        ClipPlane(const ClipPlane& cp)
        {
            _clipPlane[0]=cp._clipPlane[0];
            _clipPlane[1]=cp._clipPlane[1];
            _clipPlane[2]=cp._clipPlane[2];
            _clipPlane[3]=cp._clipPlane[3];
            _clipPlaneNum=cp._clipPlaneNum;
        }
           

        virtual unsigned int getMember() const { return _clipPlaneNum; }
          
        /** Set the clip plane with the given Plane. */
        void setClipPlane(const Plane& plane)
        {
           _clipPlane = Vec4d(plane[0],plane[1],plane[2],plane[3]);
        }

        /** Defines the plane as [ a b c d ]. */
        void setClipPlane(double a,double b,double c,double d)
        {
            _clipPlane = Vec4d(a,b,c,d);
        }

        /** Set the clip plane with the given Vec4. */
        inline void setClipPlane(const Vec4d& plane) { _clipPlane = plane; }

        /** Gets the clip plane as a Vec4d. */
        const Vec4d& getClipPlane() const { return _clipPlane; }


        /** Sets the clip plane number. */
        void setClipPlaneNum(unsigned int num);

        /** Gets the clip plane number. */
        unsigned int getClipPlaneNum() const;

        /** Applies the clip plane's state to the OpenGL state machine. */
        virtual void apply(State& state) const;


        virtual ~ClipPlane();
protected:

        Vec4d               _clipPlane;
        unsigned int        _clipPlaneNum;

};

 

#endif
