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
/*! \file block.h
 * \brief Routines to allocate memory from one large block.
 *
 * Any memory allocated is only available to be reallocated after
 * the whole block has been reset.
 */
#ifndef _block_h
#define _block_h

#include "mem.h"
#include "memint.h"

/**********************************************************************************/
/*                    type definitions                                            */

typedef struct _block_heap_mem
{
	SDWORD	size;		// size of block
	UBYTE	*pFree;		// pointer to the start of the free memory section
	UBYTE	*pMem;		// pointer to the base of the memory block
	UBYTE	*pLastAllocated;	// The start of the last allocated block (so that it can be freed by blkSpecialFree

	struct _block_heap_mem *psNext;
} BLOCK_HEAP_MEM;

typedef struct _block_heap
{
	SDWORD		init, ext;		// initial and extension block sizes
	BLOCK_HEAP_MEM	*psBlocks;
#ifdef DEBUG_MALLOC
	const char	*pFileName;
	SDWORD		line;
	MEM_NODE	*psMemTreap;	// treap of the memory blocks
	BOOL		free;			// whether free has been called for this block
	const char	*pFreeFile;		// where the last free was called from
	SDWORD		freeLine;
	UDWORD		TotalAllocated;	// Total amount of bytes used in the block (sum of all alloc's)
#endif

	struct _block_heap	*psNext;
} BLOCK_HEAP;


/**********************************************************************************/
/*                    function prototypes                                         */

// initialise the block system
extern BOOL blkInitialise(void);

// shutdown the block system
extern void blkShutDown(void);

// Create a new block heap
extern BOOL blkCreate(BLOCK_HEAP **ppsHeap, SDWORD init, SDWORD ext);

// Release a block heap
extern void blkDestroy(BLOCK_HEAP *psHeap);

// Allocate some memory from a block heap
extern void *blkAlloc(BLOCK_HEAP *psHeap, SDWORD size);

// return a chunk of memory to the block
// this only does anything whith DEBUG_BLOCK defined
extern void blkFree(BLOCK_HEAP *psHeap, void *pMemToFree);

// Reset a block heap
extern void blkReset(BLOCK_HEAP *psHeap);

// Find which block a pointer is from if any
extern BLOCK_HEAP *blkFind(void *pPtr);

// check if a pointer is valid in a block
extern BOOL blkPointerValid(BLOCK_HEAP *psHeap, void *pData, SDWORD size);

// check if a pointer is valid in any currently allocated block
extern BOOL blkPointerValidAll(void *pData, SDWORD size);

// Note the call position for a blkAlloc or blkFree
extern void blkCallPos(const char *pFileName, SDWORD line);

void blkPrintDetails(BLOCK_HEAP *psHeap);
void blkReport(void);
BOOL blkSpecialFree(BLOCK_HEAP *psHeap, void *Ptr);
void  blockSuspendUsage(void);
void blockUnsuspendUsage(void);


/**********************************************************************************/
/*                    macro definitions                                           */


#ifdef DEBUG_MALLOC

#define BLOCK_CREATE(ppsHeap, init, ext) \
	(blkCallPos(__FILE__, __LINE__), \
	 blkCreate((ppsHeap), (init), (ext)))

#define BLOCK_DESTROY(psHeap) \
	blkCallPos(__FILE__, __LINE__); \
	blkDestroy(psHeap)

#define BLOCK_ALLOC(psHeap, size) \
	(blkCallPos(__FILE__, __LINE__), \
	 blkAlloc(psHeap, size))

#define BLOCK_FREE(psHeap, pMemToFree) \
	blkCallPos(__FILE__, __LINE__); \
	blkFree(psHeap, pMemToFree)

#define BLOCK_RESET(psHeap) \
	blkCallPos(__FILE__, __LINE__); \
	blkReset(psHeap)

#define BLOCK_PTRVALID(psHeap, pData, size) \
	blkPointerValid(psHeap, pData, size)

#else

#define BLOCK_CREATE(ppsHeap, init, ext) \
	blkCreate((ppsHeap), (init), (ext))

#define BLOCK_DESTROY(psHeap) \
	blkDestroy(psHeap)

#define BLOCK_ALLOC(psHeap, size) \
	 blkAlloc(psHeap, size)

#define BLOCK_FREE(psHeap, pMemToFree) \
	blkFree(psHeap, pMemToFree)

#define BLOCK_RESET(psHeap) \
	blkReset(psHeap)

#define BLOCK_PTRVALID(psHeap, pData, size) \
	(TRUE)

#endif


#endif

