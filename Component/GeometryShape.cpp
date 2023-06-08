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
#include <stdlib.h>

#include <GeometryShape.h>
 
GeometryShape::GeometryShape() :btCollisionShape()
{
  
 
}

GeometryShape::GeometryShape(const GeometryShape& geometryShape):
    btCollisionShape(geometryShape),
    _vertexArray((geometryShape._vertexArray)),
    _normalArray((geometryShape._normalArray)),
    _colorArray((geometryShape._colorArray)),
    _secondaryColorArray((geometryShape._secondaryColorArray)),
    _fogCoordArray((geometryShape._fogCoordArray))
{
 

    for(PrimitiveSetList::const_iterator pitr= geometryShape._primitives.begin();
        pitr!= geometryShape._primitives.end();
        ++pitr)
    {
 
        _primitives.push_back(*pitr);
    }

    for(ArrayList::const_iterator titr= geometryShape._texCoordList.begin();
        titr!= geometryShape._texCoordList.end();
        ++titr)
    {
        _texCoordList.push_back( (*titr));
    }

    for(ArrayList::const_iterator vitr= geometryShape._vertexAttribList.begin();
        vitr!= geometryShape._vertexAttribList.end();
        ++vitr)
    {
        _vertexAttribList.push_back( (*vitr));
    }
 
}

GeometryShape::~GeometryShape()
{
 
}

#define ARRAY_NOT_EMPTY(array) (array!=0 && array->getNumElements()!=0)

bool GeometryShape::empty() const
{
    if (!_primitives.empty()) return false;
    if (ARRAY_NOT_EMPTY(_vertexArray.get())) return false;
    if (ARRAY_NOT_EMPTY(_normalArray.get())) return false;
    if (ARRAY_NOT_EMPTY(_colorArray.get())) return false;
    if (ARRAY_NOT_EMPTY(_secondaryColorArray.get())) return false;
    if (ARRAY_NOT_EMPTY(_fogCoordArray.get())) return false;
    if (!_texCoordList.empty()) return false;
    if (!_vertexAttribList.empty()) return false;
    return true;
}

 
void GeometryShape::setVertexArray(Array* array)
{
    if (array && array->getBinding()==Array::BIND_UNDEFINED) array->setBinding(Array::BIND_PER_VERTEX);

    _vertexArray.reset(array);
     
    dirtyBound();

    if (/*_useVertexBufferObjects && */array)
    {
 
    }
}

void GeometryShape::setNormalArray(Array* array, Array::Binding binding)
{
    if (array && binding!=Array::BIND_UNDEFINED) array->setBinding(binding);

    _normalArray.reset(array);
     

    if (/*_useVertexBufferObjects && */array)
    {
 
    }
}

void GeometryShape::setColorArray(Array* array, Array::Binding binding)
{
    if (array && binding!=Array::BIND_UNDEFINED) array->setBinding(binding);

    _colorArray.reset(array);
     
    if (/*_useVertexBufferObjects && */array)
    {
 
    }
}

void GeometryShape::setSecondaryColorArray(Array* array, Array::Binding binding)
{
    if (array && binding!=Array::BIND_UNDEFINED) array->setBinding(binding);

    _secondaryColorArray.reset(array);
     

    if (/*_useVertexBufferObjects && */array)
    {
 
    }
}

void GeometryShape::setFogCoordArray(Array* array, Array::Binding binding)
{
    if (array && binding!=Array::BIND_UNDEFINED) array->setBinding(binding);

    _fogCoordArray.reset(array);
     
    if (/*_useVertexBufferObjects && */array)
    {
 
    }
}



void GeometryShape::setTexCoordArray(unsigned int index,Array* array, Array::Binding binding)
{
    if (_texCoordList.size()<=index)
        _texCoordList.resize(index+1);

    if (array)
    {
        if (binding!=Array::BIND_UNDEFINED) array->setBinding(binding);
        else array->setBinding(Array::BIND_PER_VERTEX);
    }

    _texCoordList[index].reset(array);
     
    if (/*_useVertexBufferObjects && */array)
    {
 
    }
}

Array* GeometryShape::getTexCoordArray(unsigned int index)
{
    if (index<_texCoordList.size()) return _texCoordList[index].get();
    else return 0;
}

const Array* GeometryShape::getTexCoordArray(unsigned int index) const
{
    if (index<_texCoordList.size()) return _texCoordList[index].get();
    else return 0;
}
 
void GeometryShape::setVertexAttribArray(unsigned int index, Array* array, Array::Binding binding)
{
    if (_vertexAttribList.size()<=index)
        _vertexAttribList.resize(index+1);

    if (array && binding!=Array::BIND_UNDEFINED) array->setBinding(binding);

    _vertexAttribList[index].reset(array);
     
    if (/*_useVertexBufferObjects && */array)
    {
 
    }
}

Array *GeometryShape::getVertexAttribArray(unsigned int index)
{
    if (index<_vertexAttribList.size()) return _vertexAttribList[index].get();
    else return 0;
}

const Array *GeometryShape::getVertexAttribArray(unsigned int index) const
{
    if (index<_vertexAttribList.size()) return _vertexAttribList[index].get();
    else return 0;
}
 
bool GeometryShape::addPrimitiveSet(DrawElements* drawElements)
{
    if (drawElements)
    { 
        _primitives.push_back(std::shared_ptr<DrawElements>(drawElements));
      
        dirtyBound();
        return true;
    }

    //OSG_WARN<<"Warning: invalid DrawElements passed to GeometryShape::addPrimitiveSet(i, DrawElements), ignoring call."<<std::endl;
    return false;
}

bool GeometryShape::setPrimitiveSet(unsigned int i,DrawElements* drawElements)
{
    if (i<_primitives.size() && drawElements)
    { 
        _primitives[i].reset(drawElements);
   
        dirtyBound();
        return true;
    }
    //OSG_WARN<<"Warning: invalid index i or DrawElements passed to GeometryShape::setPrimitiveSet(i,DrawElements), ignoring call."<<std::endl;
    return false;
}

bool GeometryShape::insertPrimitiveSet(unsigned int i,DrawElements* drawElements)
{

    if (drawElements)
    { 
        if (i<_primitives.size())
        {
            _primitives.insert(_primitives.begin()+i, std::shared_ptr<DrawElements>(drawElements));
  
            dirtyBound();
            return true;
        }
        else if (i==_primitives.size())
        {
            return addPrimitiveSet(drawElements);
        }

    }
    //OSG_WARN<<"Warning: invalid index i or DrawElements passed to GeometryShape::insertPrimitiveSet(i,DrawElements), ignoring call."<<std::endl;
    return false;
}
 
bool GeometryShape::removePrimitiveSet(unsigned int i, unsigned int numElementsToRemove)
{
    if (numElementsToRemove==0) return false;

    if (i<_primitives.size())
    {
        if (i+numElementsToRemove<=_primitives.size())
        {
            _primitives.erase(_primitives.begin()+i,_primitives.begin()+i+numElementsToRemove);
        }
        else
        {
            // asking to delete too many elements, report a warning, and delete to
            // the end of the primitive list.
            //OSG_WARN<<"Warning: GeometryShape::removePrimitiveSet(i,numElementsToRemove) has been asked to remove more elements than are available,"<<std::endl;
            //OSG_WARN<<"         removing on from i to the end of the list of primitive sets."<<std::endl;
            _primitives.erase(_primitives.begin()+i,_primitives.end());
        }

    
        dirtyBound();
        return true;
    }
    //OSG_WARN<<"Warning: invalid index i passed to GeometryShape::removePrimitiveSet(i,numElementsToRemove), ignoring call."<<std::endl;
    return false;
}

unsigned int GeometryShape::getPrimitiveSetIndex(const DrawElements* drawElements) const
{
    for (unsigned int primitiveSetIndex=0;primitiveSetIndex<_primitives.size();++primitiveSetIndex)
    {
        if (_primitives[primitiveSetIndex].get() == drawElements) return primitiveSetIndex;
    }
    return _primitives.size(); // node not found.
}

unsigned int GeometryShape::getGLObjectSizeHint() const
{
    unsigned int totalSize = 0;
    if (_vertexArray) totalSize += _vertexArray->getTotalDataSize();

    if (_normalArray) totalSize += _normalArray->getTotalDataSize();

    if (_colorArray) totalSize += _colorArray->getTotalDataSize();

    if (_secondaryColorArray) totalSize += _secondaryColorArray->getTotalDataSize();

    if (_fogCoordArray) totalSize += _fogCoordArray->getTotalDataSize();


    unsigned int unit;
    for(unit=0;unit<_texCoordList.size();++unit)
    {
        const Array* array = _texCoordList[unit].get();
        if (array) totalSize += array->getTotalDataSize();

    }

    bool handleVertexAttributes = true;
    if (handleVertexAttributes)
    {
        unsigned int index;
        for( index = 0; index < _vertexAttribList.size(); ++index )
        {
            const Array* array = _vertexAttribList[index].get();
            if (array) totalSize += array->getTotalDataSize();
        }
    }

    for(PrimitiveSetList::const_iterator itr=_primitives.begin();
        itr!=_primitives.end();
        ++itr)
    {

        totalSize += 4*(*itr)->getNumIndices();

    }


    // do a very simply mapping of display list size proportional to vertex datasize.
    return totalSize;
}
 
bool GeometryShape::getDrawElementsList(PrimitiveSetList& drawElementsList) const
{
    unsigned int startSize = drawElementsList.size();

    for(PrimitiveSetList::const_iterator itr = _primitives.begin();
        itr != _primitives.end();
        ++itr)
    {
        drawElementsList.push_back(*itr);
    }

    return drawElementsList.size()!=startSize;
}
 
GeometryShape* createTexturedQuadGeometry(const Vec3f& corner,const Vec3f& widthVec,const Vec3f& heightVec, float l, float b, float r, float t)
{
    GeometryShape* geom = new GeometryShape;

    Vec3Array* coords = new Vec3Array(4);
    (*coords)[0] = corner+heightVec;
    (*coords)[1] = corner;
    (*coords)[2] = corner+widthVec;
    (*coords)[3] = corner+widthVec+heightVec;
    geom->setVertexArray(coords);

    Vec2Array* tcoords = new Vec2Array(4);
    (*tcoords)[0] = Vec2f(l,t);
    (*tcoords)[1] = Vec2f(l,b);
    (*tcoords)[2] = Vec2f(r, b);
    (*tcoords)[3] = Vec2f(r, t);
    geom->setTexCoordArray(0,tcoords, Array::BIND_PER_VERTEX);

    Vec4Array* colours = new Vec4Array(1);
    (*colours)[0] = Vec4f(1.0f,1.0f,1.0,1.0f);
    geom->setColorArray(colours, Array::BIND_OVERALL);

    Vec3Array* normals = new Vec3Array(1);
    (*normals)[0] = widthVec^heightVec;
    (*normals)[0].normalize();
    geom->setNormalArray(normals, Array::BIND_OVERALL);

    DrawElementsUShort* elems = new DrawElementsUShort(DrawElements::TRIANGLES);
    elems->push_back(0);
    elems->push_back(1);
    elems->push_back(2);

    elems->push_back(2);
    elems->push_back(3);
    elems->push_back(0);
    geom->addPrimitiveSet(elems);

    return geom;
}
 