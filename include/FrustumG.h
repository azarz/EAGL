/* ------------------------------------------------------

 View Frustum - Lighthouse3D

 Class used for the frustum culling
 
  -----------------------------------------------------*/


#ifndef _FRUSTUMG_
#define _FRUSTUMG_

#ifndef _VEC3_
#include "Vec3.h"
#endif

class Vec3;

#ifndef _PLANE_
#include "Plane.h"
#endif

class Plane;



class FrustumG 
{
private:

	enum {
		TOP = 0,
		BOTTOM,
		LEFT,
		RIGHT,
		NEARP,
		FARP
	};


public:

        enum {OUTSIDE, INTERSECT, INSIDE};

	Plane pl[6];


	Vec3 ntl,ntr,nbl,nbr,ftl,ftr,fbl,fbr;
	float nearD, farD, ratio, angle,tang;
	float nw,nh,fw,fh;

        FrustumG();
        ~FrustumG();

        void setCamInternals(float angle, float ratio, float nearD, float farD);
        void setCamDef(Vec3 p, Vec3 l, Vec3 u);
        int pointInFrustum(Vec3 &p);
        int sphereInFrustum(Vec3 p, float raio);

        void drawPoints();
        void drawLines();
        void drawPlanes();
        void drawNormals();
};


#endif
