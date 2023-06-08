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

#ifndef OSG_GEOMETRY
#define OSG_GEOMETRY 1

#include <Bullet3Common/geometry.h>
#include <Component/Array.h>
#include <Component/DrawElements.h>
#include <Bullet3Common/btCollisionShape.h>
 
class   GeometryShape : public btCollisionShape
{
public:

    GeometryShape();

    /** Copy constructor using CopyOp to manage deep vs shallow copy. */
    GeometryShape(const GeometryShape& GeometryShape);
 
    bool empty() const;

    typedef std::vector< std::shared_ptr<Array> >  ArrayList;

    void setVertexArray(Array* array);
    Array* getVertexArray() { return _vertexArray.get(); }
    const Array* getVertexArray() const { return _vertexArray.get(); }


    void setNormalArray(Array* array) { setNormalArray(array, Array::BIND_UNDEFINED); }
    void setNormalArray(Array* array, Array::Binding binding);
    Array* getNormalArray() { return _normalArray.get(); }
    const Array* getNormalArray() const { return _normalArray.get(); }




    void setColorArray(Array* array, Array::Binding binding);
    Array* getColorArray() { return _colorArray.get(); }
    const Array* getColorArray() const { return _colorArray.get(); }


    void setSecondaryColorArray(Array* array, Array::Binding binding);
    Array* getSecondaryColorArray() { return _secondaryColorArray.get(); }
    const Array* getSecondaryColorArray() const { return _secondaryColorArray.get(); }



    void setFogCoordArray(Array* array, Array::Binding binding);
    Array* getFogCoordArray() { return _fogCoordArray.get(); }
    const Array* getFogCoordArray() const { return _fogCoordArray.get(); }


    void setTexCoordArray(unsigned int unit, Array* array, Array::Binding binding);
    Array* getTexCoordArray(unsigned int unit);
    const Array* getTexCoordArray(unsigned int unit) const;

    unsigned int getNumTexCoordArrays() const { return static_cast<unsigned int>(_texCoordList.size()); }

    ArrayList& getTexCoordArrayList() { return _texCoordList; }
    const ArrayList& getTexCoordArrayList() const { return _texCoordList; }

    void setVertexAttribArray(unsigned int index, Array* array, Array::Binding binding);
    Array *getVertexAttribArray(unsigned int index);
    const Array *getVertexAttribArray(unsigned int index) const;


    unsigned int getNumVertexAttribArrays() const { return static_cast<unsigned int>(_vertexAttribList.size()); }
    ArrayList& getVertexAttribArrayList() { return _vertexAttribList; }
    const ArrayList& getVertexAttribArrayList() const { return _vertexAttribList; }


    typedef std::vector< std::shared_ptr<DrawElements> > PrimitiveSetList;

    PrimitiveSetList& getPrimitiveSetList() { return _primitives; }
    const PrimitiveSetList& getPrimitiveSetList() const { return _primitives; }

    unsigned int getNumPrimitiveSets() const { return static_cast<unsigned int>(_primitives.size()); }
    DrawElements* getPrimitiveSet(unsigned int pos) { return _primitives[pos].get(); }
    const DrawElements* getPrimitiveSet(unsigned int pos) const { return _primitives[pos].get(); }

    /** Add a primitive set to the GeometryShape. */
    bool addPrimitiveSet(DrawElements* DrawElements);

    /** Set a primitive set to the specified position in GeometryShape's primitive set list. */
    bool setPrimitiveSet(unsigned int i, DrawElements* DrawElements);

    /** Insert a primitive set to the specified position in GeometryShape's primitive set list. */
    bool insertPrimitiveSet(unsigned int i, DrawElements* DrawElements);

    /** Remove primitive set(s) from the specified position in GeometryShape's primitive set list. */
    bool removePrimitiveSet(unsigned int i, unsigned int numElementsToRemove = 1);

    /** Get the index number of a primitive set, return a value between
      * 0 and getNumPrimitiveSet()-1 if found, if not found then return getNumPrimitiveSet().
      * When checking for a valid find value use if ((value=GeometryShape->getPrimitiveSetIndex(primitive))!=GeometryShape.getNumPrimitiveSet())
    */
    unsigned int getPrimitiveSetIndex(const DrawElements* DrawElements) const;


    virtual void dirtyBound() {};

    bool getDrawElementsList(PrimitiveSetList& drawElementsList) const;

    /** Return the estimated size of GLObjects (display lists/vertex buffer objects) that are associated with this drawable.
      * This size is used a hint for reuse of deleted display lists/vertex buffer objects. */
    virtual unsigned int getGLObjectSizeHint() const;

    void setDataVariance(int) {};
    virtual void getAabb(const b3Transform& t, b3Vector3& aabbMin, b3Vector3& aabbMax) const
    {};
    virtual void draw(State& renderInfo) const {};
    virtual ~GeometryShape();

    protected:

        GeometryShape& operator = (const GeometryShape&) { return *this;}

        PrimitiveSetList                _primitives;
        std::shared_ptr<Array>             _vertexArray;
        std::shared_ptr<Array>             _normalArray;
        std::shared_ptr<Array>             _colorArray;
        std::shared_ptr<Array>             _secondaryColorArray;
        std::shared_ptr<Array>             _fogCoordArray;
        ArrayList                       _texCoordList;
        ArrayList                       _vertexAttribList;
        
};
 
/** Convenience function to be used for creating quad GeometryShape with texture coords.
  * Tex coords go from left bottom (l,b) to right top (r,t).
*/
extern   GeometryShape* createTexturedQuadGeometry(const Vec3f& corner,const Vec3f& widthVec,const Vec3f& heightVec, float l, float b, float r, float t);

/** Convenience function to be used for creating quad GeometryShape with texture coords.
  * Tex coords go from bottom left (0,0) to top right (s,t).
*/
inline GeometryShape* createTexturedQuadGeometry(const Vec3f& corner,const Vec3f& widthVec,const Vec3f& heightVec, float s=1.0f, float t=1.0f)
{
    return createTexturedQuadGeometry(corner,widthVec,heightVec, 0.0f, 0.0f, s, t);
}
 
#endif
