#ifndef GPU_UNIFIED_POLYGON_H_
#define GPU_UNIFIED_POLYGON_H_ 

#include <vector>
 
#include "Bullet3Common/b3Vector3.h"

/**
* This class represents a polygonal Face.
* A Polygon must at least have 3 vertices added, and their position must not be identical.
* All vertices must be on one plane.
*/
class Polygon  
{
public:
	Polygon();
	~Polygon();

	/**
	* Adds another vertex to the polygon at the position of 'vertex'.
	*/
	void AddVertex(const b3Vector3& vertex);
	/**
	* Implements
	* Face::GetBoundaryBox(b3Vector3* min_point, b3Vector3* max_point).
	*/
	void GetBoundaryBox(b3Vector3& min_point, b3Vector3& max_point);
	/**
	* Implements
	* Face::DoesLineIntersect(b3Vector3& start_pos, b3Vector3& end_pos).
	*/
	bool DoesLineIntersect(const b3Vector3& start_pos, const b3Vector3& end_pos);
	/**
	* Implements
	* Face::DoesPointTouch(b3Vector3& point).
	*/
	bool DoesPointTouch(b3Vector3& point);

private:
	/**
	* Tests if point1 and point2 are on differing sides of the line from
	* lineStart to lineEnd.
	*/
	bool ArePointsOnBothSidesOfTheLine(const b3Vector3& lineStart, const b3Vector3& lineEnd, const b3Vector3& point1, const b3Vector3& point2);
	/**
	* Tests if a point in the plane of the polygon is inside the polygon.
	*/
	bool IsPointInPlaneInside(const b3Vector3& point);

private:
	/**
	* Holds the vertices of this polygon.
	*/
	std::vector<b3Vector3> vertices_;
	/**
	* Cached value for GetBoundaryBox
	*/
	b3Vector3 boundary_min_;
	/**
	* Cached value for GetBoundaryBox.
	*/
	b3Vector3 boundary_max_;
	/**
	* Normal of the polygon, cached for DoesLineIntersect.
	*/
	b3Vector3 normal_;

};

#endif	// GPU_UNIFIED_POLYGON_H_
