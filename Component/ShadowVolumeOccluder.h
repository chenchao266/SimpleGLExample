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

#ifndef OSG_SHADOWVOLUMEOCCLUDER
#define OSG_SHADOWVOLUMEOCCLUDER 1

#include <Bullet3Common/Polytope.h>
#include <ConvexPlanarOccluder.h>
#include <memory>
 

 
class CullVisitor;

/** ShadowVolumeOccluder is a helper class for implementing shadow occlusion culling. */
class   ShadowVolumeOccluder
{

    public:


        typedef std::vector<Polytope> HoleList;

        ShadowVolumeOccluder(const ShadowVolumeOccluder& svo):
            _volume(svo._volume),
             
            _projectionMatrix(svo._projectionMatrix),
            _occluderVolume(svo._occluderVolume),
            _holeList(svo._holeList) {}

        ShadowVolumeOccluder():
            _volume(0.0f) {}


        bool operator < (const ShadowVolumeOccluder& svo) const { return getVolume()>svo.getVolume(); } // not greater volume first.

        /** compute the shadow volume occluder. */
        bool computeOccluder( const ConvexPlanarOccluder& occluder,CullVisitor& cullStack,bool createDrawables=false);


        inline void disableResultMasks();

        inline void pushCurrentMask();
        inline void popCurrentMask();


        /** return true if the matrix passed in matches the projection matrix that this ShadowVolumeOccluder is
          * associated with.*/
        bool matchProjectionMatrix(const Matrixf& matrix) const
        {
            return matrix==_projectionMatrix;
            //else return false;
        }

 
        /** get the volume of the occluder minus its holes, in eye coords, the volume is normalized by dividing by
          * the volume of the view frustum in eye coords.*/
        float getVolume() const { return _volume; }

        /** return the occluder polytope.*/
        Polytope& getOccluder() { return _occluderVolume; }

        /** return the const occluder polytope.*/
        const Polytope& getOccluder() const { return _occluderVolume; }

        /** return the list of holes.*/
        HoleList& getHoleList() { return _holeList; }

        /** return the const list of holes.*/
        const HoleList& getHoleList() const { return _holeList; }


        /** return true if the specified vertex list is contained entirely
          * within this shadow occluder volume.*/
        bool contains(const std::vector<Vec3f>& vertices);

        /** return true if the specified bounding sphere is contained entirely
          * within this shadow occluder volume.*/
        bool contains(const BoundingSphere& bound);

        /** return true if the specified bounding box is contained entirely
          * within this shadow occluder volume.*/
        bool contains(const BoundingBox& bound);

        inline void transformProvidingInverse(const Matrixf& matrix)
        {
            _occluderVolume.transformProvidingInverse(matrix);
            for(HoleList::iterator itr=_holeList.begin();
                itr!=_holeList.end();
                ++itr)
            {
                itr->transformProvidingInverse(matrix);
            }
        }


    protected:

        float                       _volume;
        //NodePath                    _nodePath;
        Matrixf   _projectionMatrix;
        Polytope                    _occluderVolume;
        HoleList                    _holeList;
};


/** A list of ShadowVolumeOccluder, used by CollectOccluderVisitor and CullVistor's.*/
typedef std::vector<ShadowVolumeOccluder> ShadowVolumeOccluderList;


inline void ShadowVolumeOccluder::disableResultMasks()
{
    //std::cout<<"ShadowVolumeOccluder::disableResultMasks() - _occluderVolume.getMaskStack().size()="<<_occluderVolume.getMaskStack().size()<<"  "<<_occluderVolume.getCurrentMask()<<std::endl;
    _occluderVolume.setResultMask(0);
    for(HoleList::iterator itr=_holeList.begin();
        itr!=_holeList.end();
        ++itr)
    {
        itr->setResultMask(0);
    }
}

inline void ShadowVolumeOccluder::pushCurrentMask()
{
    //std::cout<<"ShadowVolumeOccluder::pushCurrentMasks() - _occluderVolume.getMaskStack().size()="<<_occluderVolume.getMaskStack().size()<<"  "<<_occluderVolume.getCurrentMask()<<std::endl;
    _occluderVolume.pushCurrentMask();
    if (!_holeList.empty())
    {
        for(HoleList::iterator itr=_holeList.begin();
            itr!=_holeList.end();
            ++itr)
        {
            itr->pushCurrentMask();
        }
    }
}

inline void ShadowVolumeOccluder::popCurrentMask()
{
    _occluderVolume.popCurrentMask();
    if (!_holeList.empty())
    {
        for(HoleList::iterator itr=_holeList.begin();
            itr!=_holeList.end();
            ++itr)
        {
            itr->popCurrentMask();
        }
    }
    //std::cout<<"ShadowVolumeOccluder::popCurrentMasks() - _occluderVolume.getMaskStack().size()="<<_occluderVolume.getMaskStack().size()<<"  "<<_occluderVolume.getCurrentMask()<<std::endl;
}

 
#endif
