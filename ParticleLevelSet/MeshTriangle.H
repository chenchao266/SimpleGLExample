/**************************************************************************
	ORIGINAL AUTHOR: 
		Emud Mokhberi (emud@ucla.edu)
	MODIFIED BY:
	
	CONTRIBUTORS:
		

-----------------------------------------------
	
 ***************************************************************
 ******General License Agreement and Lack of Warranty ***********
 ****************************************************************

 This software is distributed for noncommercial use in the hope that it will 
 be useful but WITHOUT ANY WARRANTY. The author(s) do not accept responsibility
 to anyone for the consequences of using it or for whether it serves any 
 particular purpose or works at all. No guarantee is made about the software 
 or its performance.

 You are allowed to modify the source code, add your name to the
 appropriate list above and distribute the code as long as 
 this license agreement is distributed with the code and is included at 
 the top of all header (.h) files.

 Commercial use is strictly prohibited.
***************************************************************************/


#ifndef MESHTRIANGLE_H
#define MESHTRIANGLE_H

#include "main.h"
#include "Bullet3Common/geometry.h"

inline Double intensity(Vec3d v)  
{
    return (v.norm2() / 3);
}
inline std::ostream& operator<<(std::ostream& out, const Vec3d& g)
{ 
    for (int i = 0; i < 3; i++)
    {
        out << g[0] << "," << g[1] << "," << g[2] << " ";

    }
  
    return out;
}

 
class MeshTriangle
{
public:
	MeshTriangle	();
	MeshTriangle	(unsigned int v0, unsigned int v1, unsigned int v2);
	~MeshTriangle	();

	unsigned int&	operator	[]	(int index);
private:
	unsigned int	indices[3];

	friend std::ostream& operator << (std::ostream& out, const MeshTriangle& toPrint);

};
#endif // GEOMETRY_H
