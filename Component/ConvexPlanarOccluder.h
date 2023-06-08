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

#ifndef OSG_CONVEXPLANAROCCLUDER
#define OSG_CONVEXPLANAROCCLUDER 1

#include <Bullet3Common/geometry.h>
class   ConvexPlanarPolygon
{

    public:

        ConvexPlanarPolygon();

        typedef std::vector<Vec3f> VertexList;

        void add(const Vec3f& v) { _vertexList.push_back(v); }

        void setVertexList(const VertexList& vertexList) { _vertexList=vertexList; }

        VertexList& getVertexList() { return _vertexList; }

        const VertexList& getVertexList() const { return _vertexList; }

    protected:

        VertexList _vertexList;

};

 
/** A class for representing convex clipping volumes made up of several ConvexPlanarPolygon. */
class   ConvexPlanarOccluder 
{

    public:

        ConvexPlanarOccluder() {}
        ConvexPlanarOccluder(const ConvexPlanarOccluder& cpo):
     
            _occluder(cpo._occluder),
            _holeList(cpo._holeList) {}
 
        void setOccluder(const ConvexPlanarPolygon& cpp) { _occluder = cpp; }

        ConvexPlanarPolygon& getOccluder() { return _occluder; }

        const ConvexPlanarPolygon& getOccluder() const { return _occluder; }



        typedef std::vector<ConvexPlanarPolygon> HoleList;

        void addHole(const ConvexPlanarPolygon& cpp) { _holeList.push_back(cpp); }

        void setHoleList(const HoleList& holeList) { _holeList=holeList; }

        HoleList& getHoleList() { return _holeList; }

        const HoleList& getHoleList() const { return _holeList; }
        ~ConvexPlanarOccluder() {};

    protected:

    
        ConvexPlanarPolygon _occluder;
        HoleList _holeList;

};
 
#endif
