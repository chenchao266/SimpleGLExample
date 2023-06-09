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

#ifndef OSG_POLYTOPE
#define OSG_POLYTOPE 1

#include <Bullet3Common/Plane.h>

 

/** A Polytope class for representing convex clipping volumes made up of a set of planes.
  * When adding planes, their normals should point inwards (into the volume) */
class   Polytope
{

    public:

        typedef unsigned int                    ClippingMask;
        typedef std::vector<Plane>              PlaneList;
        typedef std::vector<Vec3f>               VertexList;
        typedef std::vector<ClippingMask>   MaskStack;

        inline Polytope() {setupMask();}

        inline Polytope(const Polytope& cv) :
            _maskStack(cv._maskStack),
            _resultMask(cv._resultMask),
            _planeList(cv._planeList),
            _referenceVertexList(cv._referenceVertexList) {}

        inline Polytope(const PlaneList& pl) : _planeList(pl) {setupMask();}

        inline ~Polytope() {}

        inline void clear() { _planeList.clear(); setupMask(); }

        inline Polytope& operator = (const Polytope& cv)
        {
            if (&cv==this) return *this;
            _maskStack = cv._maskStack;
            _resultMask = cv._resultMask;
            _planeList = cv._planeList;
            _referenceVertexList = cv._referenceVertexList;
            return *this;
        }

        /** Create a Polytope which is a cube, centered at 0,0,0, with sides of 2 units.*/
        void setToUnitFrustum(bool withNear=true, bool withFar=true)
        {
            _planeList.clear();
            _planeList.push_back(Plane(1.0,0.0,0.0,1.0)); // left plane.
            _planeList.push_back(Plane(-1.0,0.0,0.0,1.0)); // right plane.
            _planeList.push_back(Plane(0.0,1.0,0.0,1.0)); // bottom plane.
            _planeList.push_back(Plane(0.0,-1.0,0.0,1.0)); // top plane.
            if (withNear) _planeList.push_back(Plane(0.0,0.0,1.0,1.0)); // near plane
            if (withFar) _planeList.push_back(Plane(0.0,0.0,-1.0,1.0)); // far plane
            setupMask();
        }

        /** Create a Polytope which is a equivalent to BoundingBox.*/
        void setToBoundingBox(const BoundingBox& bb)
        {
            _planeList.clear();
            _planeList.push_back(Plane(1.0,0.0,0.0,-bb.xMin())); // left plane.
            _planeList.push_back(Plane(-1.0,0.0,0.0,bb.xMax())); // right plane.
            _planeList.push_back(Plane(0.0,1.0,0.0,-bb.yMin())); // bottom plane.
            _planeList.push_back(Plane(0.0,-1.0,0.0,bb.yMax())); // top plane.
            _planeList.push_back(Plane(0.0,0.0,1.0,-bb.zMin())); // near plane
            _planeList.push_back(Plane(0.0,0.0,-1.0,bb.zMax())); // far plane
            setupMask();
        }

        inline void setAndTransformProvidingInverse(const Polytope& pt, const Matrixf& matrix)
        {
            _referenceVertexList = pt._referenceVertexList;

            unsigned int resultMask = pt._maskStack.back();
            if (resultMask==0)
            {
                _maskStack.back() = 0;
                _resultMask = 0;
                _planeList.clear();
                return;
            }

            ClippingMask selector_mask = 0x1;

            unsigned int numActivePlanes = 0;

            // count number of active planes.
            PlaneList::const_iterator itr;
            for(itr=pt._planeList.begin();
                itr!=pt._planeList.end();
                ++itr)
            {
                if (resultMask&selector_mask) ++numActivePlanes;
                selector_mask <<= 1;
            }

            _planeList.resize(numActivePlanes);
            _resultMask = 0;
            selector_mask = 0x1;
            unsigned int index = 0;
            for(itr=pt._planeList.begin();
                itr!=pt._planeList.end();
                ++itr)
            {
                if (resultMask&selector_mask)
                {
                     _planeList[index] = *itr;
                     _planeList[index++].transformProvidingInverse(matrix);
                    _resultMask = (_resultMask<<1) | 1;
                }
                selector_mask <<= 1;
            }

            _maskStack.back() = _resultMask;
        }

        inline void set(const PlaneList& pl) { _planeList = pl; setupMask(); }


        inline void add(const Plane& pl) { _planeList.push_back(pl); setupMask(); }

        /** flip/reverse the orientation of all the planes.*/
        inline void flip()
        {
            for(PlaneList::iterator itr=_planeList.begin();
                itr!=_planeList.end();
                ++itr)
            {
                itr->flip();
            }
        }

        inline bool empty() const { return _planeList.empty(); }

        inline PlaneList& getPlaneList() { return _planeList; }

        inline const PlaneList& getPlaneList() const { return _planeList; }


        inline void setReferenceVertexList(VertexList& vertices) { _referenceVertexList=vertices; }

        inline VertexList& getReferenceVertexList() { return _referenceVertexList; }

        inline const VertexList& getReferenceVertexList() const { return _referenceVertexList; }


        inline void setupMask()
        {
            _resultMask = 0;
            for(unsigned int i=0;i<_planeList.size();++i)
            {
                _resultMask = (_resultMask<<1) | 1;
            }
            _maskStack.push_back(_resultMask);
        }

        inline ClippingMask& getCurrentMask() { return _maskStack.back(); }

        inline ClippingMask getCurrentMask() const { return _maskStack.back(); }

        inline void setResultMask(ClippingMask mask) { _resultMask=mask; }

        inline ClippingMask getResultMask() const { return _resultMask; }

        MaskStack& getMaskStack() { return _maskStack; }

        const MaskStack& getMaskStack() const { return _maskStack; }


        inline void pushCurrentMask()
        {
            _maskStack.push_back(_resultMask);
        }

        inline void popCurrentMask()
        {
            _maskStack.pop_back();
        }

        /** Check whether a vertex is contained within clipping set.*/
        inline bool contains(const Vec3f& v) const
        {
            if (!_maskStack.back()) return true;

            unsigned int selector_mask = 0x1;
            for(PlaneList::const_iterator itr=_planeList.begin();
                itr!=_planeList.end();
                ++itr)
            {
                if ((_maskStack.back()&selector_mask) && (itr->distance(v)<0.0f)) return false;
                selector_mask <<= 1;
            }
            return true;
        }

        /** Check whether any part of vertex list is contained within clipping set.*/
        inline bool contains(const std::vector<Vec3f>& vertices)
        {
            if (!_maskStack.back()) return true;

            _resultMask = _maskStack.back();

            for(std::vector<Vec3f>::const_iterator vitr = vertices.begin();
                vitr != vertices.end();
                ++vitr)
            {
                const Vec3f& v = *vitr;
                bool outside = false;
                ClippingMask selector_mask = 0x1;
                for(PlaneList::const_iterator itr=_planeList.begin();
                    itr!=_planeList.end() && !outside;
                    ++itr)
                {
                    if ((_maskStack.back()&selector_mask) && (itr->distance(v)<0.0f)) outside = true;
                    selector_mask <<= 1;
                }

                if (!outside) return true;
            }
            return false;
        }

        /** Check whether any part of a bounding sphere is contained within clipping set.
            Using a mask to determine which planes should be used for the check, and
            modifying the mask to turn off planes which wouldn't contribute to clipping
            of any internal objects.  This feature is used in CullVisitor
            to prevent redundant plane checking.*/
        inline bool contains(const BoundingSphere& bs)
        {
            if (!_maskStack.back()) return true;

            _resultMask = _maskStack.back();
            ClippingMask selector_mask = 0x1;

            for(PlaneList::const_iterator itr=_planeList.begin();
                itr!=_planeList.end();
                ++itr)
            {
                if (_resultMask&selector_mask)
                {
                    int res=itr->intersect(bs);
                    if (res<0) return false; // outside clipping set.
                    else if (res>0) _resultMask ^= selector_mask; // subsequent checks against this plane not required.
                }
                selector_mask <<= 1;
            }
            return true;
        }

        /** Check whether any part of a bounding box is contained within clipping set.
            Using a mask to determine which planes should be used for the check, and
            modifying the mask to turn off planes which wouldn't contribute to clipping
            of any internal objects.  This feature is used in CullVisitor
            to prevent redundant plane checking.*/
        inline bool contains(const BoundingBox& bb)
        {
            if (!_maskStack.back()) return true;

            _resultMask = _maskStack.back();
            ClippingMask selector_mask = 0x1;

            for(PlaneList::const_iterator itr=_planeList.begin();
                itr!=_planeList.end();
                ++itr)
            {
                if (_resultMask&selector_mask)
                {
                    int res=itr->intersect(bb);
                    if (res<0) return false; // outside clipping set.
                    else if (res>0) _resultMask ^= selector_mask; // subsequent checks against this plane not required.
                }
                selector_mask <<= 1;
            }
            return true;
        }

        /** Check whether all of vertex list is contained with clipping set.*/
        inline bool containsAllOf(const std::vector<Vec3f>& vertices)
        {
            if (!_maskStack.back()) return false;

            _resultMask = _maskStack.back();
            ClippingMask selector_mask = 0x1;

            for(PlaneList::const_iterator itr=_planeList.begin();
                itr!=_planeList.end();
                ++itr)
            {
                if (_resultMask&selector_mask)
                {
                    int res=itr->intersect(vertices);
                    if (res<1) return false;  // intersects, or is below plane.
                    _resultMask ^= selector_mask; // subsequent checks against this plane not required.
                }
                selector_mask <<= 1;
            }
            return true;
        }

        /** Check whether the entire bounding sphere is contained within clipping set.*/
        inline bool containsAllOf(const BoundingSphere& bs)
        {
            if (!_maskStack.back()) return false;

            _resultMask = _maskStack.back();
            ClippingMask selector_mask = 0x1;

            for(PlaneList::const_iterator itr=_planeList.begin();
                itr!=_planeList.end();
                ++itr)
            {
                if (_resultMask&selector_mask)
                {
                    int res=itr->intersect(bs);
                    if (res<1) return false;  // intersects, or is below plane.
                    _resultMask ^= selector_mask; // subsequent checks against this plane not required.
                }
                selector_mask <<= 1;
            }
            return true;
        }

        /** Check whether the entire bounding box is contained within clipping set.*/
        inline bool containsAllOf(const BoundingBox& bb)
        {
            if (!_maskStack.back()) return false;

            _resultMask = _maskStack.back();
            ClippingMask selector_mask = 0x1;

            for(PlaneList::const_iterator itr=_planeList.begin();
                itr!=_planeList.end();
                ++itr)
            {
                if (_resultMask&selector_mask)
                {
                    int res=itr->intersect(bb);
                    if (res<1) return false;  // intersects, or is below plane.
                    _resultMask ^= selector_mask; // subsequent checks against this plane not required.
                }
                selector_mask <<= 1;
            }
            return true;
        }

        /** Check whether any part of a triangle is contained within the polytope.*/
        bool contains(const Vec3f& v0, const Vec3f& v1, const Vec3f& v2) const;


        /** Transform the clipping set by matrix.  Note, this operations carries out
          * the calculation of the inverse of the matrix since a plane must
          * be multiplied by the inverse transposed to transform it. This
          * makes this operation expensive.  If the inverse has been already
          * calculated elsewhere then use transformProvidingInverse() instead.
          * See http://www.worldserver.com/turk/computergraphics/NormalTransformations.pdf*/
        inline void transform(const Matrixf& matrix)
        {
            Matrixf inverse = (matrix.invert());
            transformProvidingInverse(inverse);
        }

        /** Transform the clipping set by provide a pre inverted matrix.
          * see transform for details. */
        inline void transformProvidingInverse(const Matrixf& matrix)
        {
            if (!_maskStack.back()) return;

            _resultMask = _maskStack.back();
            ClippingMask selector_mask = 0x1;
            for(PlaneList::iterator itr=_planeList.begin();
                itr!=_planeList.end();
                ++itr)
            {
                if (_resultMask&selector_mask)
                {
                    itr->transformProvidingInverse(matrix);
                }
                selector_mask <<= 1;
            }
        }

    protected:


        MaskStack                           _maskStack;
        ClippingMask                        _resultMask;
        PlaneList                           _planeList;
        VertexList                          _referenceVertexList;

};

 
#endif
