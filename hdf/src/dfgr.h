/*
$Header$

$Log$
Revision 1.1  1992/08/25 21:40:44  koziol
Initial revision

*/
/*-----------------------------------------------------------------------------
 * File:    dfgr.h
 * Purpose: header file for the Raster Image set
 * Invokes: df.h
 * Contents:
 *  Structure definitions: DFGRdr, DFGRrig
 * Remarks: This is included with user programs which use general raster
 *---------------------------------------------------------------------------*/


#ifndef DFGR_H                      /* avoid re-inclusion */
#define DFGR_H

#include "hdf.h"

typedef struct {
    uint16 tag, ref;
} DFdi;

/* description record: used to describe image data, palette data etc. */
typedef struct {
    int32 xdim, ydim;               /* dimensions of data */
    DFdi nt;                        /* number type of data */
    int16 ncomponents, interlace;   /* data ordering: chunky / planar etc */
    DFdi compr;                     /* compression */
        /* ### Note: compression is currently uniquely described with a tag.
            No data is attached to this tag/ref.  But this capability is
            provided for future expansion, when this tag/ref might point to
            some data needed for decompression, such as the actual encodings */
} DFGRdr;

/* structure to hold RIG info */
typedef struct {
    DFdi data[3];                   /* image/lut/mattechannel */
    DFGRdr datadesc[3];             /* description of image/lut/mattechannel */
    int32 xpos, ypos;               /* X-Y position of image on screen */
    float32 aspectratio;            /* ratio of pixel height to width */
    float32 ccngamma,               /* color correction parameters */
        ccnred[3], ccngrren[3], ccnblue[3], ccnwhite[3];
    char *cf;                       /* color format */
} DFGRrig;

#endif /* DFGR_H */
