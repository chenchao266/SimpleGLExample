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

#ifndef OSG_MULTIGEOMETRY
#define OSG_MULTIGEOMETRY 1

#include <Bullet3Common/geometry.h>
#include <Component/Array.h>
#include <Component/DrawElements.h>
#include <Bullet3Common/btCollisionShape.h>
#include <Component/GeometryShape.h>
typedef std::vector< std::shared_ptr<GeometryShape> > ShapeList;
class MultiGeometryShape : public GeometryShape
{
public:

    MultiGeometryShape() {};

    /** Copy constructor using CopyOp to manage deep vs shallow copy. */
    MultiGeometryShape(const MultiGeometryShape& rh) {
        for (ShapeList::const_iterator itr = rh._children.begin();
            itr != rh._children.end();
            ++itr)
        {
            GeometryShape* child =  (*itr).get();
            if (child) addDrawable(child);
        }
    };

    bool empty() const;
    virtual bool addDrawable(GeometryShape *drawable) { _children.push_back(std::shared_ptr<GeometryShape>(drawable)); return true; };

    template<class T> bool addDrawable(const std::shared_ptr<T>& drawable) { return addDrawable(drawable.get()); }
    virtual bool removeDrawable(GeometryShape *drawable) {
        return removeDrawables(getDrawableIndex(drawable), 1);
    };

    template<class T> bool removeDrawable(const std::shared_ptr<T>& drawable) { return removeDrawable(drawable.get()); }
    virtual bool removeDrawables(unsigned int pos, unsigned int numDrawablesToRemove = 1)
    { 
        _children.erase(_children.begin() + pos, _children.begin() + pos + numDrawablesToRemove); return true;
    }
    inline unsigned int getNumDrawables() const {  return static_cast<unsigned int>(_children.size()); }

    /** Return the \c Drawable at position \c i.*/
    inline GeometryShape* getDrawable(unsigned int i) { return _children[i].get(); }

    /** Return the \c Drawable at position \c i.*/
    inline const GeometryShape* getDrawable(unsigned int i) const { return _children[i].get(); }
    inline bool containsDrawable(const GeometryShape* drawable) const
    {
        for (ShapeList::const_iterator itr = _children.begin(); itr != _children.end(); ++itr)
        {
            if (itr->get() == drawable) return true;
        }
        return false;
    }

    template<class T> bool containsDrawable(const std::shared_ptr<T>& drawable) const { return containsDrawable(drawable.get()); }
    inline unsigned int getDrawableIndex(const GeometryShape* drawable) const
    {
        for (unsigned int childNum = 0; childNum < _children.size(); ++childNum)
        {
            if (_children[childNum].get() == drawable) return childNum;
        }
        return static_cast<unsigned int>(_children.size()); // node not found.
    }

    template<class T> unsigned int getDrawableIndex(const std::shared_ptr<T>& drawable) const { return getDrawableIndex(drawable.get()); }
    virtual ~MultiGeometryShape() {};
    inline const BoundingBox& getBoundingBox() const
    {
        getBound();
        return _bbox;
    }

    virtual BoundingSphere computeBound() const {
        BoundingSphere bsphere;

        _bbox.init();

        for (ShapeList::const_iterator itr = _children.begin();
            itr != _children.end();
            ++itr)
        {
            if (*itr)
            {
                const GeometryShape* drawable = (*itr).get();
          
                {
                    _bbox.expandBy(drawable->getBoundingBox());
                }
              
            }
        }

        if (_bbox.valid())
        {
            bsphere.expandBy(_bbox);
        }
        return bsphere;
    };
    void  getAabb(const b3Transform &, b3Vector3 &, b3Vector3 &) const
    {

    }
protected:

    ShapeList _children;

    mutable BoundingBox        _bbox;

};
 
#endif
