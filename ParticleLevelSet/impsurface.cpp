#include "impsurface.h"
#include "glad/gl.h"

/*******************************************************************
 * Class ImpSurface
 * represents an implicit function on R^3
 ******************************************************************/
ImpSurface::ImpSurface ()
{
}

ImpSurface::~ImpSurface ()
{
}

Vec3d ImpSurface::grad (const Vec3d& location)
{
	Double dx = this->eval(location[0] + GRAD_EPSILON,
						   location[1],
						   location[2]) -
				this->eval(location[0] - GRAD_EPSILON,
						   location[1],
						   location[2]);
	Double dy = this->eval(location[0],
						   location[1] + GRAD_EPSILON,
						   location[2]) -
				this->eval(location[0],
						   location[1] - GRAD_EPSILON,
						   location[2]);
	Double dz = this->eval(location[0],
						   location[1],
						   location[2] + GRAD_EPSILON) -
				this->eval(location[0],
						   location[1],
						   location[2] - GRAD_EPSILON);

	Vec3d g(dx, dy, dz);
	g *= GRAD_EPS_INV * 0.5;
	g.normalize();
	return g;
}

Vec3d ImpSurface::normal (const Vec3d& location)
{
	return grad(location).normalized();
}

Double ImpSurface::eval (const Vec3d& location, Double t)
{
	return eval(location);
}

/*******************************************************************
 * Class IsoSurface
 * holds a tesselation of a specified implicit function representing
 * an isosurface of that function.
 ******************************************************************/

IsoSurface::IsoSurface ()
:function(NULL)
{
}

IsoSurface::IsoSurface (ImpSurface* function_)
:function(function_)
{
}

IsoSurface::~IsoSurface ()
{
}

int IsoSurface::addVertex (Vec3d& toAdd)
{
	int index = (int) vertices.size();
	try 
	{ 
		vertices.push_back(toAdd); 
	}
	catch(...)
	{
        std::cerr << "couldn't push vertex #" << index << std::endl;
	}
	return index;
}

void IsoSurface::addFace (int v1, int v2, int v3)
{
	try
	{
		faces.push_back(MeshTriangle(v1, v2, v3));
	}
	catch(...)
	{
		std::cerr << "couldn't push face #" << (int) faces.size() << std::endl;
	}
}

void IsoSurface::addFace (MeshTriangle& toAdd)
{
	try
	{
		faces.push_back(toAdd);
	}
	catch(...)
	{
		std::cerr << "couldn't push face #" << (int) faces.size() << std::endl;
	}
}

void IsoSurface::glDraw ()
{
    glVertexPointer(3, GL_DOUBLE, sizeof(Vec3d), &vertices[0][0]);
	glNormalPointer(GL_DOUBLE, sizeof(Vec3d), &vNormals[0][0]);
	glDrawElements(GL_TRIANGLES, (GLsizei) faces.size() * 3, GL_UNSIGNED_INT, &faces[0][0]);
}

void IsoSurface::calcVNorms ()
{
	int numV = (int)vertices.size();
	vNormals.clear();

	for (int i = 0; i < numV; ++i)
	{
		try
		{
			vNormals.push_back(function->normal(vertices[i]));
		}
		catch (...)
		{
			std::cerr << "couldn't push normal #" << i << std::endl;
			break;
		}
	}
}

void IsoSurface::clear ()
{
	vertices.clear();
	faces.clear();
	vNormals.clear();
}

ImpSurface* IsoSurface::getFunction ()
{
	return function;
}

/* 
 * Prints the mesh in the Scene Description Language used for POVRay
 */ 
std::ostream& operator << (std::ostream& out, const IsoSurface& s)
{
	out << "#include \"template.pov\"" << std::endl;
	out << "mesh2{" << std::endl;

	out << "\tvertex_vectors{" << std::endl;
	int numVertices = (int)s.vertices.size();
	out << "\t\t" << numVertices << "," << std::endl;
	for (int i = 0; i < numVertices; ++i)
	{
		out << "\t\t<" << s.vertices[i] << ">";
		if (i < (numVertices - 1))
			out << ", ";
		out << std::endl;
	}
	out << "\n\t}" << std::endl;

	out << "\tnormal_vectors{" << std::endl;
	out << "\t\t" << numVertices << "," << std::endl;
	for (int i = 0; i < numVertices; ++i)
	{
		out << "\t\t<" << s.vNormals[i] << ">";
		if (i < (numVertices - 1))
			out << ", ";
		out << std::endl;
	}
	out << "\n\t}" << std::endl;

	out << "\tface_indices{" << std::endl;
	int numFaces = (int) s.faces.size();
	out << "\t\t" << numFaces << "," << std::endl;
	for (int i = 0; i < numFaces; ++i)
	{
		out << "\t\t<" << s.faces[i] << ">";
		if (i < (numFaces - 1))
			out << ",";
		out << std::endl;
	}
	out << "\t}" << std::endl;

	out << "\tpigment {rgb 1}"<<std::endl;

    out << "\tphotons {"<<std::endl;
    out << "\trefraction off"<<std::endl;
    out << "\treflection off"<<std::endl;
    out << "\tcollect on"<<std::endl;
    out << "\t}"<<std::endl;

	out << "}" << std::endl;
	return out;
}