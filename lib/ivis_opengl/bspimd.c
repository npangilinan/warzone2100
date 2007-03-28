/*
	This file is part of Warzone 2100.
	Copyright (C) 1999-2004  Eidos Interactive
	Copyright (C) 2005-2007  Warzone Resurrection Project

	Warzone 2100 is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	Warzone 2100 is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Warzone 2100; if not, write to the Free Software
	Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
*/
/*

	BSP Draw routines for iVis02

	- 5 Sept 1997  - Tim Cannell - Pumpkin Studios - Eidos



	- Main routines were written by Gareth Jones .... so if you find any bugs I think you better speak to him ...

*/

#include <string.h>

#include "lib/framework/frame.h"	// just for the typedef's
#include "lib/ivis_common/pietypes.h"
#include "piematrix.h"
#include "lib/ivis_common/ivisdef.h"// this can have the #define for BSPIMD in it
#include "lib/ivis_common/imd.h"// this has the #define for BSPPOLYID_TERMINATE
#include "lib/ivis_common/ivi.h"

#ifdef BSPIMD		// covers the whole file

//#define BSP_MAXDEBUG		// define this if you want max debug options (runs very slow)

#include "lib/ivis_common/bspimd.h"
#include "lib/ivis_common/bspfunc.h"

#include <stdio.h>
#include <assert.h>

// from imddraw.c
void DrawTriangleList(BSPPOLYID PolygonNumber);


// Local prototypes
static inline int IsPointOnPlane( PSPLANE psPlane, Vector3i * vP );
static Vector3i *IMDvec(int Vertex);

iIMDShape *BSPimd=NULL;		// This is a global ... it is used in imddraw.c (for speed)


// Local static variables
static Vector3i *CurrentVertexList=NULL;
//static int CurrentVertexListCount=0;



extern BOOL NoCullBSP;	// Oh yes... a global externaly referenced variable....

/***************************************************************************/
/*
 * Calculates whether point is on same side, opposite side or in plane;
 *
 * returns OPPOSITE_SIDE if opposite,
 *         IN_PLANE if contained in plane,
 *         SAME_SIDE if same side
 *       Also returns pvDot - the dot product
 * - inputs vP vector to the point
 * - psPlane structure containing the plane equation
 */
/***************************************************************************/
static inline int IsPointOnPlane( PSPLANE psPlane, Vector3i * vP )
{
	Vector3f	vecP;
	FRACT Dot;

	/* validate input */
#ifdef BSP_MAXDEBUG
	ASSERT( PTRVALID(psPlane,sizeof(PLANE)),
			"IsPointOnPlane: invalid plane\n" );
#endif

	/* subtract point on plane from input point to get position vector */
	vecP.x = MAKEFRACT(vP->x - psPlane->vP.x);
	vecP.y = MAKEFRACT(vP->y - psPlane->vP.y);
	vecP.z = MAKEFRACT(vP->z - psPlane->vP.z);

	/* get dot product of result with plane normal (a,b,c of plane) */
	Dot = (FRACT)(FRACTmul(vecP.x,psPlane->a) + FRACTmul(vecP.y,psPlane->b) + FRACTmul(vecP.z,psPlane->c));

	/* if result is -ve, return -1 */
	if ( (abs((int)Dot)) <  FRACTCONST(1,100) )
	{
		return IN_PLANE;
	}
	else if ( Dot < 0 )
	{
		return OPPOSITE_SIDE;
	}
	return SAME_SIDE;
}


/*
	These routines are used by the IMD Load_BSP routine
*/
typedef iIMDPoly * PSTRIANGLE;

static Vector3f *iNormalise(Vector3f * v);
static Vector3f * iCrossProduct( Vector3f * psD, Vector3f * psA, Vector3f * psB );
static void GetTriangleNormal( PSTRIANGLE psTri, Vector3f * psN, int pA, int pB, int pC );
PSBSPTREENODE InitNode(PSBSPTREENODE psBSPNode);


static FRACT GetDist( PSTRIANGLE psTri, int pA, int pB );

// little routine for getting an imd vector structure in the IMD from the vertex ID
static inline Vector3i *IMDvec(int Vertex)
{
#ifdef BSP_MAXDEBUG
	assert((Vertex>=0)&&(Vertex<CurrentVertexListCount));
#endif
	return (CurrentVertexList+Vertex);
}



/*
 Its easy enough to calculate the plane equation if there is only 3 points
 ... but if there is four things get a little tricky ...

In theory you should be able the pick any 3 of the points to calculate the equation, however
in practise mathematically inacurraceys mean that  you need the three points that are the furthest apart

*/
void GetPlane( iIMDShape *s, UDWORD PolygonID, PSPLANE psPlane )
{

	Vector3f Result;
	iIMDPoly *psTri;
	/* validate input */
	ASSERT( PTRVALID(psPlane,sizeof(PLANE)),
			"GetPlane: invalid plane\n" );

	psTri=&(s->polys[PolygonID]);
	CurrentVertexList=s->points;
#ifdef BSP_MAXDEBUG
	CurrentVertexListCount=s->npoints;	// for error detection
#endif

	if (psTri->npnts==4)
	{
		int pA,pB,pC;
		FRACT ShortDist,Dist;

		ShortDist=MAKEFRACT(999);
		pA=0;pB=1;pC=2;

		Dist=GetDist(psTri,0,1);		// Distance between two points in the quad
		if (Dist < ShortDist)	{	ShortDist=Dist;	pA=0;pB=2;pC=3;		}

		Dist=GetDist(psTri,0,2);
		if (Dist < ShortDist)	{	ShortDist=Dist;	pA=0;pB=1;pC=3;		}

		Dist=GetDist(psTri,0,3);
		if (Dist < ShortDist)	{	ShortDist=Dist;	pA=0;pB=1;pC=2;		}

		Dist=GetDist(psTri,1,2);
		if (Dist < ShortDist)	{	ShortDist=Dist;	pA=0;pB=1;pC=3;		}

		Dist=GetDist(psTri,1,3);
		if (Dist < ShortDist)	{	ShortDist=Dist;	pA=0;pB=1;pC=2;		}

		Dist=GetDist(psTri,2,3);
		if (Dist < ShortDist)	{	ShortDist=Dist;	pA=0;pB=1;pC=2;		}

		GetTriangleNormal(psTri,&Result,pA,pB,pC);
	}
	else
	{
		GetTriangleNormal(psTri,&Result,0,1,2);		// for a triangle there is no choice ...
	}

	/* normalise normal */
	iNormalise( &Result );

// This result MUST be cast to a fract and not called using MAKEFRACT
//
//  This is because on a playstation we are casting from FRACT->FRACT (so no conversion is needed)
//    ... and on a PC we are converting from DOUBLE->FLOAT  (so a cast is needed)
	psPlane->a = (FRACT)(Result.x);
	psPlane->b = (FRACT)(Result.y);
	psPlane->c = (FRACT)(Result.z);
	/* since plane eqn is ax + by + cz + d = 0,
	 * d = -(ax + by + cz).
	 */
	// Because we are multiplying a FRACT by a const we can use normal '*'
	psPlane->d = - (	( psPlane->a* IMDvec(psTri->pindex[0])->x ) +
		( psPlane->b* IMDvec(psTri->pindex[0])->y ) +
		( psPlane->c* IMDvec(psTri->pindex[0])->z ) );
	/* store first triangle vertex as point on plane for later point /
	 * plane classification in IsPointOnPlane
	 */
	memcpy( &psPlane->vP, IMDvec(psTri->pindex[0]), sizeof(Vector3i) );
}


/***************************************************************************/
/*
 * iCrossProduct
 *
 * Calculates cross product of a and b.
 */
/***************************************************************************/

static Vector3f * iCrossProduct( Vector3f * psD, Vector3f * psA, Vector3f * psB )
{
    psD->x = FRACTmul(psA->y , psB->z) - FRACTmul(psA->z ,psB->y);
    psD->y = FRACTmul(psA->z , psB->x) - FRACTmul(psA->x ,psB->z);
    psD->z = FRACTmul(psA->x , psB->y) - FRACTmul(psA->y ,psB->x);

    return psD;
}


static FRACT GetDist( PSTRIANGLE psTri, int pA, int pB )
{
	FRACT vx,vy,vz;
	FRACT sum_square,dist;

	vx = MAKEFRACT( IMDvec(psTri->pindex[pA])->x - IMDvec(psTri->pindex[pB])->x);
	vy = MAKEFRACT( IMDvec(psTri->pindex[pA])->y - IMDvec(psTri->pindex[pB])->y);
	vz = MAKEFRACT( IMDvec(psTri->pindex[pA])->z - IMDvec(psTri->pindex[pB])->z);



	sum_square = (FRACTmul(vx,vx)+FRACTmul(vy,vy)+FRACTmul(vz,vz) );
	dist = fSQRT(sum_square);
	return dist;
}


static void GetTriangleNormal( PSTRIANGLE psTri, Vector3f * psN,int pA, int pB, int pC )
{
	Vector3f vecA, vecB;

	/* validate input */
	ASSERT( PTRVALID(psTri,sizeof(iIMDPoly)),
			"GetTriangleNormal: invalid triangle\n" );

	/* get triangle edge vectors */
	vecA.x = MAKEFRACT( IMDvec(psTri->pindex[pA])->x - IMDvec(psTri->pindex[pB])->x);
	vecA.y = MAKEFRACT( IMDvec(psTri->pindex[pA])->y - IMDvec(psTri->pindex[pB])->y);
	vecA.z = MAKEFRACT( IMDvec(psTri->pindex[pA])->z - IMDvec(psTri->pindex[pB])->z);


	vecB.x = MAKEFRACT( IMDvec(psTri->pindex[pA])->x - IMDvec(psTri->pindex[pC])->x);
	vecB.y = MAKEFRACT( IMDvec(psTri->pindex[pA])->y - IMDvec(psTri->pindex[pC])->y);
	vecB.z = MAKEFRACT( IMDvec(psTri->pindex[pA])->z - IMDvec(psTri->pindex[pC])->z);

	/* normal is cross product */
	iCrossProduct( psN, &vecA, &vecB );
}


/***************************************************************************/
/*
 * Normalises the vector v
 */
/***************************************************************************/

static Vector3f *iNormalise(Vector3f * v)
{
    FRACT vx, vy, vz, mod, sum_square;

	vx = (FRACT)v->x;
	vy = (FRACT)v->y;
	vz = (FRACT)v->z;

 	if ((vx == 0) && (vy == 0) && (vz == 0))
	{
        return v;
	}


	sum_square = (FRACTmul(vx,vx)+FRACTmul(vy,vy)+FRACTmul(vz,vz) );
	mod = fSQRT(sum_square);

	v->x = FRACTdiv( vx , mod);
	v->y = FRACTdiv( vy , mod);
	v->z = FRACTdiv( vz , mod);

	return v;
}




PSBSPTREENODE InitNode(PSBSPTREENODE psBSPNode)
{
	PSBNODE			psBNode;

	/* create node triangle lists */
	psBSPNode->TriSameDir=BSPPOLYID_TERMINATE;
	psBSPNode->TriOppoDir=BSPPOLYID_TERMINATE;

	psBNode = (PSBNODE) psBSPNode;

	psBNode->link[LEFT] = NULL;
	psBNode->link[RIGHT] = NULL;

	return psBSPNode;
}

// Calculate the real camera position based on the coordinates of the camera and the camera
// distance - the result is stores in CameraLoc ,,  up is +ve Y
void GetRealCameraPos(OBJPOS *Camera, SDWORD Distance, Vector3i *CameraLoc)
{
	int Yinc;

//  as pitch is negative ... we need to subtract the value from y to go up the axis
	CameraLoc->y = Camera->y - 	((SIN(Camera->pitch) * Distance)>>FP12_SHIFT);
//	CameraLoc->y = Camera->y + 	((iV_SIN(Camera->pitch) * Distance)>>FP12_SHIFT);

	Yinc =  ((COS(Camera->pitch) * Distance)>>FP12_SHIFT);

//	DBPRINTF(("camera x=%d y=%d z=%d\n",Camera->x,Camera->y,Camera->z));
//	DBPRINTF(("pitch=%ld yaw=%ld Yinc=%ld Dist=%ld\n",Camera->pitch,Camera->yaw,Yinc,Distance));

	CameraLoc->x = Camera->x + ((SIN(-Camera->yaw) * (-Yinc))>>FP12_SHIFT);
	CameraLoc->z = Camera->z + ((COS(-Camera->yaw) * (Yinc))>>FP12_SHIFT);

//	DBPRINTF(("out) camera x=%d y=%d z=%d\n",CameraLoc->x,CameraLoc->y,CameraLoc->z));
//	DBPRINTF(("t=%d t1=%d t2=%d t3=%d z=%d\n",t,t1,t2,t3,CameraLoc->z));

}

#endif			// #ifdef BSPIMD
