/* ------------------------------------------------------

 Axis Aligned Boxes - Lighthouse3D

  -----------------------------------------------------*/


#ifndef _AABOX_
#define _AABOX_

#ifndef _Vec3_
#include "Vec3.h"
#endif

class Vec3;

class AABox 
{

public:

	Vec3 corner;
	float x,y,z;


        AABox( Vec3 &corner, float x, float y, float z);
        AABox(void);
        ~AABox();

        void setBox( Vec3 &corner, float x, float y, float z);

	// for use in frustum computations
        Vec3 getVertexP(Vec3 normal);
        Vec3 getVertexN(Vec3 normal);


};


#endif
