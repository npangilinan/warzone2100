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
/***************************************************************************/
/*
 * pieState.h
 *
 * render State controlr all pumpkin image library functions.
 *
 */
/***************************************************************************/

#ifndef _piestate_h
#define _piestate_h


/***************************************************************************/

#include "lib/framework/frame.h"
#include "piedef.h"

/***************************************************************************/
/*
 *	Global Definitions
 */
/***************************************************************************/

typedef	enum	REND_MODE
				{
					REND_GOURAUD_TEX,
					REND_ALPHA_TEX,
					REND_ADDITIVE_TEX,
					REND_TEXT,
					REND_ALPHA_TEXT,
					REND_FLAT,
					REND_ALPHA_FLAT,
					REND_ALPHA_ITERATED,
					REND_FILTER_FLAT,
					REND_FILTER_ITERATED
				}
				REND_MODE;

typedef	enum	DEPTH_MODE
				{
					DEPTH_CMP_LEQ_WRT_ON,
					DEPTH_CMP_ALWAYS_WRT_ON,
					DEPTH_CMP_LEQ_WRT_OFF,
					DEPTH_CMP_ALWAYS_WRT_OFF
				}
				DEPTH_MODE;

typedef	enum	TRANSLUCENCY_MODE
				{
					TRANS_DECAL,
					TRANS_DECAL_FOG,
					TRANS_FILTER,
					TRANS_ALPHA,
					TRANS_ADDITIVE
				}
				TRANSLUCENCY_MODE;

typedef	enum	FOG_CAP
				{
					FOG_CAP_NO,
					FOG_CAP_GREY,
					FOG_CAP_UNDEFINED
				}
				FOG_CAP;

typedef	enum	TEX_CAP
				{
					TEX_CAP_2M,
					TEX_CAP_8BIT,
					TEX_CAP_FULL,
					TEX_CAP_UNDEFINED
				}
				TEX_CAP;

typedef	enum	COLOUR_MODE
				{
					COLOUR_FLAT_CONSTANT,
					COLOUR_FLAT_ITERATED,
					COLOUR_TEX_ITERATED,
					COLOUR_TEX_CONSTANT
				}
				COLOUR_MODE;

typedef	enum	TEX_MODE
				{
					TEX_LOCAL,
					TEX_NONE
				}
				TEX_MODE;

typedef	enum	ALPHA_MODE
				{
					ALPHA_ITERATED,
					ALPHA_CONSTANT
				}
				ALPHA_MODE;


typedef struct	RENDER_STATE
				{
					FOG_CAP				fogCap;
					BOOL				fogEnabled;
					BOOL				fog;
					UDWORD				fogColour;
					SDWORD				texPage;
					REND_MODE			rendMode;
					BOOL				bilinearOn;
					BOOL				keyingOn;
					COLOUR_MODE			colourCombine;
					TEX_MODE			texCombine;
					ALPHA_MODE			alphaCombine;
					TRANSLUCENCY_MODE	transMode;
					UDWORD				colour;
#ifdef STATES
					BOOL				textured;
					UBYTE				lightLevel;
#endif
				}
				RENDER_STATE;

#define NO_TEXPAGE -1

/***************************************************************************/
/*
 *	Global Variables
 */
/***************************************************************************/

extern SDWORD	pieStateCount;

/***************************************************************************/
/*
 *	Global ProtoTypes
 */
/***************************************************************************/
extern void pie_SetDefaultStates(void);//Sets all states
extern void pie_SetDepthBufferStatus(DEPTH_MODE depthMode);
extern void pie_SetGammaValue(float val);
//renderer capability
extern void pie_SetFogCap(FOG_CAP val);
extern FOG_CAP pie_GetFogCap(void);
extern void pie_SetTexCap(TEX_CAP val);
extern TEX_CAP pie_GetTexCap(void);
//fog available
extern void pie_EnableFog(BOOL val);
extern BOOL pie_GetFogEnabled(void);
//fog currently on
extern void pie_SetFogStatus(BOOL val);
extern BOOL pie_GetFogStatus(void);
extern void pie_SetFogColour(UDWORD colour);
extern UDWORD pie_GetFogColour(void);
extern void pie_UpdateFogDistance(float begin, float end);
//render states
extern void pie_SetTexturePage(SDWORD num);
extern void pie_SetBilinear(BOOL bilinearOn);
extern BOOL pie_GetBilinear(void);
extern void pie_SetColourKeyedBlack(BOOL keyingOn);
extern void pie_SetRendMode(REND_MODE rendMode);
extern void pie_SetColour(UDWORD val);
extern UDWORD pie_GetColour(void);

#endif // _pieState_h
