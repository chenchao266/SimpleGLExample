#include "Polygon.h"
#include "Bullet3Common/geometry.h"
#include <algorithm>

static const float kAlmostZero = 0.000001f;

//--------------------------------------------------------------------
Polygon::Polygon()
	//--------------------------------------------------------------------
{
	boundary_min_.setValue(0.0f, 0.0f, 0.0f);
	boundary_max_.setValue(0.0f, 0.0f, 0.0f);
}

//--------------------------------------------------------------------
Polygon::~Polygon()
	//--------------------------------------------------------------------
{}

//--------------------------------------------------------------------
void Polygon::AddVertex(const b3Vector3& vertex)
	//--------------------------------------------------------------------
{
	// update min & max of the bounding box
	for(int i=0; i<3; ++i)
	{
		boundary_min_[i] = min(vertex[i], boundary_min_[i]);
		boundary_max_[i] = max(vertex[i], boundary_max_[i]);
	}
	// calculate the normal
	if(vertices_.size() == 2)
	{
		normal_ = (vertices_[0]-vertices_[1]).cross(vertex-vertices_[1]);
		normal_.normalize();
	}
	vertices_.push_back(vertex);
}

//--------------------------------------------------------------------
void Polygon::GetBoundaryBox(b3Vector3& min_point, b3Vector3& max_point)
	//--------------------------------------------------------------------
{
	// NOTE: at least one vertex must have been given for this to work!
	min_point = boundary_min_;
	max_point = boundary_max_;
}

//--------------------------------------------------------------------
bool Polygon::DoesLineIntersect(const b3Vector3& start_pos, const b3Vector3& end_pos)
	//--------------------------------------------------------------------
{
	// NOTE: 3 vertices must have been given for this to work!
	b3Vector3 dist_vec = end_pos - start_pos;
	float denominator = dist_vec.dot(normal_);
	if(fabs(denominator)<kAlmostZero) // denominator == 0
	{
		return false; // the line is parallel to the polygon
	}
	float d = -vertices_[0].dot(normal_); // plane equation: normal*'point in plane'+d=0

	float line_DOF = -(d + start_pos.dot(normal_) ) / denominator; // the degree of freedom on the ray

	if(line_DOF<0 || line_DOF>1)
	{
		return false; // the line is entirely on one side of the plane of the polygon
	}

	b3Vector3 intersection = start_pos + dist_vec * line_DOF; // intersection between the line & the plane

	// now test the actual vertices: is the point of intersection between plane & line inside the polygon?
	return IsPointInPlaneInside(intersection);
}

//--------------------------------------------------------------------
bool Polygon::ArePointsOnBothSidesOfTheLine(const b3Vector3& line_start, const b3Vector3& line_end, const b3Vector3& point1, const b3Vector3& point2)
	//--------------------------------------------------------------------
{
	// projection on 2D: just use x & y coordinates, except the plane of the lines is parallel to the z axis
	int first_index, second_index;
	if(normal_[0] == 0)
	{
		first_index = 0;
		if(normal_[1] == 0)
		{
			second_index = 1;
		}
		else
		{
			second_index = 2;
		}
	}
	else
	{
		first_index = 1;
		second_index = 2;
	}
	// ccw(line_start,line_end,point1)
	float dxES = line_end[first_index] - line_start[first_index];
	float dx1E = point1[first_index] - line_end[first_index];
	float dySE = line_start[second_index] - line_end[second_index];
	float dyE1 = line_end[second_index] - point1[second_index];
	bool ccwSE1 = (dySE * dx1E) > (dyE1 * dxES);
	// ccw(line_start,line_end,point2)
	float dx2E = point2[first_index] - line_end[first_index];
	float dyE2 = line_end[second_index] - point2[second_index];
	bool ccwSE2 = (dySE * dx2E) > (dyE2 * dxES);
	return ccwSE1 != ccwSE2;
}

//--------------------------------------------------------------------
bool Polygon::DoesPointTouch(b3Vector3& point)
	//--------------------------------------------------------------------
{
	// NOTE: 3 vertices must have been given for this to work!
	float d = -vertices_[0].dot(normal_); // plane equation: normal*'point in plane'+d=0
	float equation = point.dot(normal_)+d;
	if(fabs(equation) > kAlmostZero) // point is not on the plane of the polygon
	{
		return false;
	}
	return IsPointInPlaneInside(point);
}

//--------------------------------------------------------------------
bool Polygon::IsPointInPlaneInside(const b3Vector3& point)
	//--------------------------------------------------------------------
{
	// NOTE: assumes that point is in the plane of the polygon
	bool inside = false; // ray between outside & point crosses the lines of the polygon -> # is odd <=> 'point' is inside
	b3Vector3 outside = boundary_min_ - b3Vector3(1,  1,  1); // a point outside of the polygon. it's not on the plane, but we'll be doing a projection anyway
	std::vector<b3Vector3>::iterator endIter = vertices_.end();
	for(std::vector<b3Vector3>::iterator v = vertices_.begin(); v!=endIter; ++v)
	{
		b3Vector3& start_vertex = *v;
		b3Vector3& end_vertex = ((v+1) != endIter) ? *(v+1) : vertices_[0]; // next vertex/first one

		if(ArePointsOnBothSidesOfTheLine(outside, point, start_vertex, end_vertex) && ArePointsOnBothSidesOfTheLine(start_vertex, end_vertex, outside, point))
		{
			inside = !inside;
		}
	}
	return inside;
}
