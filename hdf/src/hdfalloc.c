/****************************************************************************
 * NCSA HDF                                                                 *
 * Software Development Group                                               *
 * National Center for Supercomputing Applications                          *
 * University of Illinois at Urbana-Champaign                               *
 * 605 E. Springfield, Champaign IL 61820                                   *
 *                                                                          *
 * For conditions of distribution and use, see the accompanying             *
 * hdf/COPYING file.                                                        *
 *                                                                          *
 ****************************************************************************/

#ifdef RCSID
static char RcsId[] = "@(#)$Revision$";
#endif

/* $Id$ */

#include <ctype.h>
#ifdef MALDEBUG
#define __MALDEBUG__
#endif
#include "hdf.h"

/*
LOCAL ROUTINES
  None
EXPORTED ROUTINES
  HDmemfill    -- copy a chunk of memory repetitively into another chunk
  HIstrncpy    -- string copy with termination
  HDspaceleft  -- calculates the amount of space available to HDmalloc
  HDmalloc     -- dynamicly allocates memory
  HDrealloc    -- dynamicly resize (reallocate) memory
  HDfree       -- free dynamicly allocated memory
  HDcalloc     -- dynamicly allocates memory and clears it to zero
  HDstrdup     -- in-library replacement for non-ANSI strdup()
  fmemcpy_big  -- function specific to the PC to copy 32-bits of data
  fmemset_big  -- function specific to the PC to set 32-bits of data
  fmemcmp_big  -- function specific to the PC to compare 32-bits of data
  memcpy_big   -- function specific to the PC to copy 32-bits of data(WIN3)
  memset_big   -- function specific to the PC to set 32-bits of data(WIN3)
  memcmp_big   -- function specific to the PC to compare 32-bits of data(WIN3)
*/

/*--------------------------------------------------------------------------
 NAME
    HDmemfill -- copy a chunk of memory repetitively into another chunk

 USAGE
    VOIDP HDmemfill(dest,src,item_size,num_items)
        VOIDP dest;         OUT: pointer to the chunk of memory to be filled
                            with a pattern
        VOIDP src;          IN: pointer to the pattern to copy
        uint32 item_size;   IN: size of the pattern to copy
        uint32 num_items;   IN: number of times to copy the pattern into the dest
                            buffer

 RETURNS
    Returns a pointer to the dest parameter

 DESCRIPTION
    Common code used to fill a chunk of memory with a pattern.  This
    routine can be used to copy a given "fill" value into an array
    of any number type.

 GLOBAL VARIABLES
 COMMENTS, BUGS, ASSUMPTIONS
    The src and dest pointers are assumed to point to valid portions of
    memory.
 EXAMPLES
 REVISION LOG
--------------------------------------------------------------------------*/
VOIDP
HDmemfill(VOIDP dest, const VOIDP src, uint32 item_size, uint32 num_items)
{
    uint32      copy_size;      /* size of the buffer to copy */
    uint32      copy_items;     /* number of items currently copying */
    uint32      items_left;     /* number of items left to copy */
    uint8      *curr_dest;      /* ptr into the 'dest' memory where we are currently */

    /* minimal error check for 0 sized array or item size */
    if (num_items > 0 && item_size > 0)
      {
          HDmemcpy(dest, src, item_size);   /* copy first item */

          copy_size = item_size;
          copy_items = 1;
          items_left = num_items - 1;
          curr_dest = ((uint8 *) dest) + item_size;

          /* copy until we've copied at least half of the items */
          while (items_left >= copy_items)
            {

                HDmemcpy(curr_dest, dest, copy_size);   /* copy the current chunk */
                curr_dest += copy_size;     /* move the offset for the next chunk */
                items_left -= copy_items;   /* decrement the number of items left */

                copy_size *= 2;     /* increase the size of the chunk to copy */
                copy_items *= 2;    /* increase the count of items we are copying */
            }   /* end while */
          if (items_left > 0)   /* if there are any items left to copy */
              HDmemcpy(curr_dest, dest, items_left * item_size);
      }     /* end if */
    return (dest);
}   /* end HDmemfill() */

/*--------------------------------------------------------------------------
 NAME
    HIstrncpy -- string copy with termination
 USAGE
    char *HIstrncpy(char *dest,char *source,int32 len)
        char *dest;             OUT: location to place string
        char *source;           IN: location of string to copy
        int32 len;              IN: mas. length of dest. string
 RETURNS
    dest on success, NULL on failure.
 DESCRIPTION
    This function creates a string in dest that is at most 'len' characters
    long.  The 'len' characters *include* the NULL terminator which must be
    added.  So if you have the string "Foo\0" you must call this copy function
    with len == 4.
 GLOBAL VARIABLES
 COMMENTS, BUGS, ASSUMPTIONS
 EXAMPLES
 REVISION LOG
--------------------------------------------------------------------------*/
char _HUGE *
HIstrncpy(char *dest, const char *source, int32 len)
{
    char       *destp;

    destp = dest;
    if (len == 0)
        return (destp);
    for (; (len > 1) && (*source != '\0'); len--)
        *dest++ = *source++;
    *dest = '\0';   /* Force the last byte be '\0'   */
    return (destp);
}   /* end HIstrncpy() */
/* *INDENT-OFF* */
/* GNU indent 1.9.1 urps on this section, so turn off indenting for now -QAK */

#if defined PC && !defined PC386
#ifdef WIN3
/*--------------------------------------------------------------------------
 NAME
    HDspaceleft -- calculates the amount of space available to HDmalloc
 USAGE
    int32 HDspaceleft(void)
 RETURNS
    Number of bytes able to be allocated on success, FAIL on failure.
 DESCRIPTION
    Determines the number of bytes able to be allocated through HDmalloc()
    and returns that value.
 GLOBAL VARIABLES
 COMMENTS, BUGS, ASSUMPTIONS
 EXAMPLES
 REVISION LOG
--------------------------------------------------------------------------*/
int32 HDspaceleft(void)
{
/* return the largest amount of memory Windows can give us */
   return(GlobalCompact(0));
}   /* end HDspaceleft() */

#else /* WIN3 */
/*--------------------------------------------------------------------------
 NAME
    HDspaceleft -- calculates the amount of space available to HDmalloc
 USAGE
    int32 HDspaceleft(void)
 RETURNS
    Number of bytes able to be allocated on success, FAIL on failure.
 DESCRIPTION
    Determines the number of bytes able to be allocated through HDmalloc()
    and returns that value.
 GLOBAL VARIABLES
 COMMENTS, BUGS, ASSUMPTIONS
 EXAMPLES
 REVISION LOG
--------------------------------------------------------------------------*/
int32 HDspaceleft(void)
{
    struct _heapinfo h_info;        /* structure for heap information, defined in <malloc.h> */
    int heap_status;                /* the current condition of the heap */
    int32 total_free,total_used;    /* variables to store the amount used and the amount free in the heap */

    total_free=0;
    total_used=0;
    h_info._pentry=NULL;
    while((heap_status=_heapwalk(&h_info))==_HEAPOK) {
        if(h_info._useflag==_USEDENTRY)
            total_used+=h_info._size;
        else
            total_free+=h_info._size;
    } /* end for */
    switch(heap_status) {
        case _HEAPEND:
        case _HEAPEMPTY:
            break;

        case _HEAPBADBEGIN:
            printf("%s block at %Fp of size %4.4X\n",(h_info._useflag==_USEDENTRY ? "USED" : "FREE"),h_info._pentry,h_info._size);
            printf("ERROR - heap is damaged\n\n");
            return((int32)-1);

        case _HEAPBADPTR:
            printf("%s block at %Fp of size %4.4X\n",(h_info._useflag==_USEDENTRY ? "USED" : "FREE"),h_info._pentry,h_info._size);
            printf("ERROR - bad pointer to heap\n\n");
            return((int32)-1);

        case _HEAPBADNODE:
            printf("%s block at %Fp of size %4.4X\n",(h_info._useflag==_USEDENTRY ? "USED" : "FREE"),h_info._pentry,h_info._size);
            printf("ERROR - bad node in heap\n\n");
            return((int32)-1);
    } /* end switch */
    return((int32)total_free);
} /* end HDspaceleft() */
#endif /* WIN3 */

#ifdef WIN3
/*--------------------------------------------------------------------------
 NAME
    HDmalloc -- dynamicly allocates memory
 USAGE
    VOIDP HDmalloc(qty)
        uint32 qty;         IN: the (minimum) number of bytes to allocate in
                                the memory block.
 RETURNS
    Pointer to the memory allocated on success, NULL on failure.
 DESCRIPTION
    Dynamicly allocates a block of memory and returns a pointer to it.
 GLOBAL VARIABLES
 COMMENTS, BUGS, ASSUMPTIONS
    Acts like malloc().
 EXAMPLES
 REVISION LOG
--------------------------------------------------------------------------*/
VOIDP HDmalloc(uint32 qty)
{
    char FUNC[]="HDmalloc";
    HGLOBAL hTmp;
    HGLOBAL far *wfpTmp;

    hTmp=GlobalAlloc(GMEM_MOVEABLE|GMEM_DISCARDABLE|GMEM_ZEROINIT,qty+sizeof(HGLOBAL));
    if (!hTmp)
        HRETURN_ERROR(DFE_NOSPACE,NULL);
    wfpTmp=(HGLOBAL far *) GlobalLock(hTmp);
    if (!wfpTmp) {
        GlobalFree(hTmp);
        HRETURN_ERROR(DFE_NOSPACE,NULL);
      } /* end if */
    *wfpTmp=hTmp;
    wfpTmp++;
    return((void _HUGE *)wfpTmp);
}   /* end HDmalloc() */

/*--------------------------------------------------------------------------
 NAME
    HDrealloc -- dynamicly resize (reallocate) memory
 USAGE
    VOIDP HDrealloc(vfp,qty)
        VOIDP vfp;          IN: pointer to the memory block to resize.
        uint32 qty;         IN: the (minimum) number of bytes to allocate in
                                the new memory block.
 RETURNS
    Pointer to the memory allocated on success, NULL on failure.
 DESCRIPTION
    Dynamicly re-allocates a block of memory and returns a pointer to it.
 GLOBAL VARIABLES
 COMMENTS, BUGS, ASSUMPTIONS
    Acts like realloc().
 EXAMPLES
 REVISION LOG
--------------------------------------------------------------------------*/
VOIDP HDrealloc(VOIDP vfp, uint32 new_size)
{
    char FUNC[]="HDrealloc";
    HGLOBAL new_handle;         /* handle of the new memory block */
    HGLOBAL hTmp;
    WORD far *wfpTmp;

    if(!vfp)
        HRETURN_ERROR(DFE_ARGS,NULL);
    hTmp = (HGLOBAL)(*(--((WORD far *) vfp)));
    if (!hTmp)
        HRETURN_ERROR(DFE_ARGS,NULL);
    if (GlobalUnlock(hTmp))
        HRETURN_ERROR(DFE_NOSPACE,NULL);
    if((new_handle=GlobalReAlloc(hTmp,new_size,GMEM_MOVEABLE|GMEM_DISCARDABLE|GMEM_ZEROINIT))!=NULL) {
        wfpTmp=(WORD far *) GlobalLock(new_handle);
        if (!wfpTmp) {
            GlobalFree(new_handle);
            HRETURN_ERROR(DFE_NOSPACE,NULL);
          } /* end if */
        *wfpTmp=(WORD)hTmp;
        wfpTmp++;
        return(wfpTmp);
      } /* end if */
    else
        HRETURN_ERROR(DFE_NOSPACE,NULL);
}   /* end HDrealloc() */

/*--------------------------------------------------------------------------
 NAME
    HDfree -- free dynamicly allocated memory
 USAGE
    void HDfree(vfp)
        VOIDP vfp;          IN: pointer to the memory block to free.
 RETURNS
    NULL?
 DESCRIPTION
    Free dynamicly allocated blocks of memory.
 GLOBAL VARIABLES
 COMMENTS, BUGS, ASSUMPTIONS
    Acts like free().
 EXAMPLES
 REVISION LOG
--------------------------------------------------------------------------*/
void HDfree(void *vfp)
{
    HGLOBAL hTmp;

    if (!vfp)
        return;
    hTmp=(HGLOBAL)(*(--((WORD far *) vfp)));
    if (!hTmp)
        return;
    GlobalUnlock(hTmp);
    GlobalFree(hTmp);

}   /* end HDfree() */
#else /* !WIN3 */

/*--------------------------------------------------------------------------
 NAME
    HDmalloc -- dynamicly allocates memory
 USAGE
    VOIDP HDmalloc(qty)
        uint32 qty;         IN: the (minimum) number of bytes to allocate in
                                the memory block.
 RETURNS
    Pointer to the memory allocated on success, NULL on failure.
 DESCRIPTION
    Dynamicly allocates a block of memory and returns a pointer to it.
 GLOBAL VARIABLES
 COMMENTS, BUGS, ASSUMPTIONS
    Acts like malloc().
 EXAMPLES
 REVISION LOG
--------------------------------------------------------------------------*/
VOIDP HDmalloc(uint32 qty)
{
    char FUNC[]="HDmalloc";
    char huge *p;

#ifndef TEST_PC
    qty+=sizeof(char)+sizeof(uint32);   /* increment the quantity to allocate */

    if(qty>=(int32)64000) {   /* see if we have to use halloc() to get a really large chunk of memory */
        p = halloc((int32)qty,(size_t)1);
        if (p==NULL)
            HRETURN_ERROR(DFE_NOSPACE,NULL);
        *p++=1;     /* indicate that halloc() was used to acquire this memory */
      } /* end if */
    else {      /* we can get away with just malloc() */
        p = malloc((size_t)qty);
        if (p==NULL)
            HRETURN_ERROR(DFE_NOSPACE,NULL);
        *p++=0;     /* indicate that malloc() was used to acquire this memory */
      } /* end else */
    *(uint32 *)p=qty;   /* save the size of the block */
    p+=sizeof(uint32);
#else
    p = malloc((size_t)qty);
    if (p==NULL)
        HERROR(DFE_NOSPACE);
#endif
    return(p);
}   /* end HDmalloc() */

/*--------------------------------------------------------------------------
 NAME
    HDrealloc -- dynamicly resize (reallocate) memory
 USAGE
    VOIDP HDrealloc(vfp,qty)
        VOIDP vfp;          IN: pointer to the memory block to resize.
        uint32 qty;         IN: the (minimum) number of bytes to allocate in
                                the new memory block.
 RETURNS
    Pointer to the memory allocated on success, NULL on failure.
 DESCRIPTION
    Dynamicly re-allocates a block of memory and returns a pointer to it.
 GLOBAL VARIABLES
 COMMENTS, BUGS, ASSUMPTIONS
    Acts like realloc().
 EXAMPLES
 REVISION LOG
--------------------------------------------------------------------------*/
VOIDP HDrealloc(VOIDP ptr, uint32 qty)
{
    char FUNC[]="HDrealloc";
    uint32 old_size;
    char *p=ptr;
    char *p2;

    qty+=sizeof(char)+sizeof(uint32);   /* increment the quantity to allocate */

    p-=sizeof(uint32);    /* decrement the pointer to free */
    old_size=*(uint32 *)p;
    p-=sizeof(char);
    if(qty>=(int32)64000) {   /* see if we have to use halloc() to get a really large chunk of memory */
        p2=halloc((int32)qty,(size_t)1);
        if(p2==NULL)
            HRETURN_ERROR(DFE_NOSPACE,NULL);
        *p2++=1;     /* indicate that halloc() was used to acquire this memory */
        *(uint32 *)p2=qty;   /* save the size of the block */
      } /* end if */
    else {      /* we can get away with just malloc() */
        p2=malloc((size_t)qty);
        if(p2==NULL)
            HRETURN_ERROR(DFE_NOSPACE,NULL);
        *p2++=0;     /* indicate that malloc() was used to acquire this memory */
      } /* end else */
    *(uint32 *)p2=qty;   /* save the size of the block */
    p2+=sizeof(uint32);
    HDmemcpy(p2,p+(sizeof(char)+sizeof(uint32)),
            MIN(UINT_MAX,(uint16)MIN(old_size,qty))-(sizeof(char)+sizeof(uint32)));
    if(*p)    /* check whether block of memory was allocated with halloc() */
        hfree(p);
    else       /* memory was allocated through malloc() */
        free(p);
    return(p2);
}   /* end HDrealloc() */

/*--------------------------------------------------------------------------
 NAME
    HDfree -- free dynamicly allocated memory
 USAGE
    void HDfree(vfp)
        VOIDP vfp;          IN: pointer to the memory block to free.
 RETURNS
    NULL?
 DESCRIPTION
    Free dynamicly allocated blocks of memory.
 GLOBAL VARIABLES
 COMMENTS, BUGS, ASSUMPTIONS
    Acts like free().
 EXAMPLES
 REVISION LOG
--------------------------------------------------------------------------*/
void HDfree(void *ptr)
{
    char *p=ptr;

    if(ptr!=NULL) {
#ifndef TEST_PC
        p-=(sizeof(char)+sizeof(uint32));    /* decrement the pointer to free */
        if(*p)    /* check whether block of memory was allocated with halloc() */
            hfree(p);
        else       /* memory was allocated through malloc() */
#endif
            free(p);
      } /* end if */
}   /* end HDfree() */

#endif /* WIN3 */

#else /* !PC | PC386 */

/* define MALLOC_CHECK to get some more information when malloc/realloc fail */

#ifdef MALLOC_CHECK
/*--------------------------------------------------------------------------
 NAME
    HDmalloc -- dynamicly allocates memory
 USAGE
    VOIDP HDmalloc(qty)
        uint32 qty;         IN: the (minimum) number of bytes to allocate in
                                the memory block.
 RETURNS
    Pointer to the memory allocated on success, NULL on failure.
 DESCRIPTION
    Dynamicly allocates a block of memory and returns a pointer to it.
 GLOBAL VARIABLES
 COMMENTS, BUGS, ASSUMPTIONS
    Acts like malloc().
 EXAMPLES
 REVISION LOG
--------------------------------------------------------------------------*/
VOIDP HDmalloc(uint32 qty)
{
    char FUNC[]="HDmalloc";
    char *p;

    p = (char *) malloc(qty);
    if (p== (char *) NULL) {
        HEreport("Attempted to allocate %d bytes", qty);
        HRETURN_ERROR(DFE_NOSPACE,NULL);
      } /* end if */
    return(p);
}   /* end HDmalloc() */

/*--------------------------------------------------------------------------
 NAME
    HDrealloc -- dynamicly resize (reallocate) memory
 USAGE
    VOIDP HDrealloc(vfp,qty)
        VOIDP vfp;          IN: pointer to the memory block to resize.
        uint32 qty;         IN: the (minimum) number of bytes to allocate in
                                the new memory block.
 RETURNS
    Pointer to the memory allocated on success, NULL on failure.
 DESCRIPTION
    Dynamicly re-allocates a block of memory and returns a pointer to it.
 GLOBAL VARIABLES
 COMMENTS, BUGS, ASSUMPTIONS
    Acts like realloc().
 EXAMPLES
 REVISION LOG
--------------------------------------------------------------------------*/
VOIDP HDrealloc(VOIDP where, uint32 qty)
{
    char FUNC[]="HDrealloc";
    char *p;

    p = (char *) realloc(where, qty);
    if (p== (char *) NULL) {
        HEreport("Attempted to re-allocate %d bytes", qty);
        HRETURN_ERROR(DFE_NOSPACE,NULL);
      } /* end if */
    return(p);
}   /* end HDrealloc() */

/*--------------------------------------------------------------------------
 NAME
    HDfree -- free dynamicly allocated memory
 USAGE
    void HDfree(vfp)
        VOIDP vfp;          IN: pointer to the memory block to free.
 RETURNS
    NULL?
 DESCRIPTION
    Free dynamicly allocated blocks of memory.
 GLOBAL VARIABLES
 COMMENTS, BUGS, ASSUMPTIONS
    Acts like free().
 EXAMPLES
 REVISION LOG
--------------------------------------------------------------------------*/
void HDfree(VOIDP ptr)
{
    if (ptr!=NULL)
        free(ptr);
}   /* end HDfree() */
#endif /* MALLOC_CHECK */

#endif /* !PC | PC386 */

/* *INDENT-ON* */

#if defined MALLOC_CHECK | (defined PC & !defined PC386)
/*--------------------------------------------------------------------------
 NAME
    HDcalloc -- dynamicly allocates memory and clears it to zero
 USAGE
    VOIDP HDcalloc(n,size)
        uint32 n;         IN: the number of blocks to allocate
        uint32 size;      IN: the size of the block
 RETURNS
    Pointer to the memory allocated on success, NULL on failure.
 DESCRIPTION
    Dynamicly allocates a block of memory and returns a pointer to it
    after setting it to zero.
 GLOBAL VARIABLES
 COMMENTS, BUGS, ASSUMPTIONS
    Acts like calloc().  Instead of doing all the work ourselves, this calls
    HDmalloc and HDmemset().
 EXAMPLES
 REVISION LOG
--------------------------------------------------------------------------*/
VOIDP
HDcalloc(uint32 n, uint32 size)
{
    char        FUNC[] = "HDcalloc";
    VOIDP       p;

    p = HDmalloc(n * size);
    if (p == NULL)
      {
          HEreport("Attempted to allocate %d blocks of %d bytes", (int) n, (int) size);
          HRETURN_ERROR(DFE_NOSPACE, NULL);
      }     /* end if */
    else
        HDmemset(p, 0, n * size);
    return (p);
}   /* end HDcalloc() */
#endif /* MALLOC_CHECK */

#if defined VMS | (defined PC & !defined PC386) | defined macintosh | defined MIPSEL | defined NEXT | defined CONVEX
/*--------------------------------------------------------------------------
 NAME
    HDstrdup -- in-library replacement for non-ANSI strdup()
 USAGE
    char *HDstrdup(s)
        const char *s;          IN: pointer to the string to duplicate
 RETURNS
    Pointer to the duplicated string, or NULL on failure.
 DESCRIPTION
    Duplicates a string (i.e. allocates space and copies it over).
 GLOBAL VARIABLES
 COMMENTS, BUGS, ASSUMPTIONS
    Acts like strdup().
 EXAMPLES
 REVISION LOG
--------------------------------------------------------------------------*/
char       *
HDstrdup(const char *s)
{
    char       *ret;

    ret = (char *) HDmalloc((uint32) HDstrlen(s) + 1);
    if (ret == NULL)
        return (NULL);
    HDstrcpy(ret, s);
    return (ret);
}   /* end HDstrdup() */

#endif /* VMS & (PC & !PC386) & macinosh */

#if defined WIN3 || defined PC
#ifdef WIN3
/*--------------------------------------------------------------------------

 NAME
    fmemcpy_big -- function specific to the PC to copy 32-bits of data
 USAGE
    VOIDP fmemcpy_big (dst,src,len)
        VOIDP dst;          IN: the buffer to put bytes into
        VOIDP src;          IN: the source buffer to copy from
        uint32 len;         IN: the number of bytes to copy
 RETURNS
    returns the original pointer to dst
 DESCRIPTION
    Because the IBM PC compilers use 16-bit number for memcpy, this function
    blocks that up into 64kb blocks to copy at a time.
 GLOBAL VARIABLES
 COMMENTS, BUGS, ASSUMPTIONS
 EXAMPLES
 REVISION LOG
--------------------------------------------------------------------------*/
VOIDP
fmemcpy_big(VOIDP dst, VOIDP src, uint32 len)
{
    uint8      *s, *d;           /* alias for the buffers */

    if (len <= UINT_MAX)    /* if the size is small enough copy all at once */
        return (_fmemcpy(dst, src, (size_t) len));
    else
      {     /* number of bytes to read */
          s = (uint8 *) src;
          d = (uint8 *) dst;
          while (len > UINT_MAX)
            {
                _fmemcpy(d, s, UINT_MAX);
                s += UINT_MAX;
                d += UINT_MAX;
                len -= UINT_MAX;
            }   /* end while */
          if (len > 0)
              _fmemcpy(d, s, (size_t) len);
      }     /* end else */
    return (dst);
}   /* end fmemcpy_big() */

/*--------------------------------------------------------------------------

 NAME
    fmemset_big -- function specific to the PC to set 32-bits of data
 USAGE
    VOIDP fmemset_big (src, c, len)
        VOIDP src;          IN: the buffer to set to a value
        intn c;             IN: the value to use to set
        uint32 len;         IN: the number of bytes to set
 RETURNS
    returns the original pointer to s
 DESCRIPTION
    Because the IBM PC compilers use 16-bit number for memcpy, this function
    blocks that up into 64kb blocks to set at a time.
 GLOBAL VARIABLES
 COMMENTS, BUGS, ASSUMPTIONS
 EXAMPLES
 REVISION LOG
--------------------------------------------------------------------------*/
VOIDP
fmemset_big(VOIDP src, intn c, uint32 len)
{
    uint8      *s;              /* alias for the buffers */

    if (len <= UINT_MAX)    /* if the size is small enough copy all at once */
        return (_fmemset(src, c, (size_t) len));
    else
      {     /* number of bytes to read */
          s = (uint8 *) src;
          while (len > UINT_MAX)
            {
                _fmemset(s, c, UINT_MAX);
                s += UINT_MAX;
                len -= UINT_MAX;
            }   /* end while */
          if (len > 0)
              _fmemset(s, c, (size_t) len);
      }     /* end else */
    return (src);
}   /* end fmemset_big() */

/*--------------------------------------------------------------------------

 NAME
    fmemcmp_big -- function specific to the PC to compare 32-bits of data
 USAGE
    VOIDP fmemcmp_big (s1,s2,len)
        VOIDP s1;           IN: the first buffer
        VOIDP s2;           IN: the second buffer
        uint32 len;         IN: the number of bytes to copy
 RETURNS
    returns a value less than, equal to, or greater than 0 indicating
    that the object pointed to by s1 is less than, equal to, or greater
    than the object pointed to by s2
 DESCRIPTION
    Because the IBM PC compilers use 16-bit number for memcpy, this function
    blocks that up into 64kb blocks to compare at a time.
 GLOBAL VARIABLES
 COMMENTS, BUGS, ASSUMPTIONS
 EXAMPLES
 REVISION LOG
--------------------------------------------------------------------------*/
intn
fmemcmp_big(VOIDP s1, VOIDP s2, uint32 len)
{
    intn        ret_val;

    if (len <= UINT_MAX)    /* if the size is small enough copy all at once */
        return (_fmemcmp(s1, s2, (size_t) len));
    else
      {     /* number of bytes to read */
          while (len > UINT_MAX)
            {
                ret_val = _fmemcmp(s1, s2, UINT_MAX);
                if (ret_val != 0)
                    return (ret_val);
                ((uint8 huge *) s1) += UINT_MAX;
                ((uint8 huge *) s2) += UINT_MAX;
                len -= UINT_MAX;
            }   /* end while */
          if (len > 0)
              return (_fmemcmp(s1, s2, (size_t) len));
      }     /* end else */
    return (0);
}   /* end fmemcmp_big() */

#else  /* !WIN3 */

/*--------------------------------------------------------------------------

 NAME
    memcpy_big -- function specific to the PC to copy 32-bits of data
 USAGE
    VOIDP memcpy_big (dst,src,len)
        VOIDP dst;          IN: the buffer to put bytes into
        VOIDP src;          IN: the source buffer to copy from
        uint32 len;         IN: the number of bytes to copy
 RETURNS
    returns the original pointer to dst
 DESCRIPTION
    Because the IBM PC compilers use 16-bit number for memcpy, this function
    blocks that up into 64kb blocks to copy at a time.
 GLOBAL VARIABLES
 COMMENTS, BUGS, ASSUMPTIONS
 EXAMPLES
 REVISION LOG
--------------------------------------------------------------------------*/
VOIDP
memcpy_big(VOIDP dst, VOIDP src, uint32 len)
{
    uint8      *s, *d;          /* alias for the buffers */

    if (len <= UINT_MAX)    /* if the size is small enough copy all at once */
        return (memcpy(dst, src, (size_t) len));
    else
      {     /* number of bytes to read */
          s = (uint8 *) src;
          d = (uint8 *) dst;
          while (len > UINT_MAX)
            {
                memcpy(d, s, UINT_MAX);
                s += UINT_MAX;
                d += UINT_MAX;
                len -= UINT_MAX;
            }   /* end while */
          if (len > 0)
              memcpy(d, s, (size_t) len);
      }     /* end else */
    return (dst);
}   /* end memcpy_big() */

/*--------------------------------------------------------------------------

 NAME
    memset_big -- function specific to the PC to set 32-bits of data
 USAGE
    VOIDP memset_big (src, c, len)
        VOIDP src;          IN: the buffer to set to a value
        intn c;             IN: the value to use to set
        uint32 len;         IN: the number of bytes to set
 RETURNS
    returns the original pointer to s
 DESCRIPTION
    Because the IBM PC compilers use 16-bit number for memcpy, this function
    blocks that up into 64kb blocks to set at a time.
 GLOBAL VARIABLES
 COMMENTS, BUGS, ASSUMPTIONS
 EXAMPLES
 REVISION LOG
--------------------------------------------------------------------------*/
VOIDP
memset_big(VOIDP src, intn c, uint32 len)
{
    uint8      *s;              /* alias for the buffers */

    if (len <= UINT_MAX)    /* if the size is small enough copy all at once */
        return (memset(src, c, (size_t) len));
    else
      {     /* number of bytes to read */
          s = (uint8 *) src;
          while (len > UINT_MAX)
            {
                memset(s, c, UINT_MAX);
                s += UINT_MAX;
                len -= UINT_MAX;
            }   /* end while */
          if (len > 0)
              memset(s, c, (size_t) len);
      }     /* end else */
    return (src);
}   /* end memset_big() */

/*--------------------------------------------------------------------------

 NAME
    memcmp_big -- function specific to the PC to compare 32-bits of data
 USAGE
    VOIDP memcmp_big (s1,s2,len)
        VOIDP s1;           IN: the first buffer
        VOIDP s2;           IN: the second buffer
        uint32 len;         IN: the number of bytes to copy
 RETURNS
    returns a value less than, equal to, or greater than 0 indicating
    that the object pointed to by s1 is less than, equal to, or greater
    than the object pointed to by s2
 DESCRIPTION
    Because the IBM PC compilers use 16-bit number for memcpy, this function
    blocks that up into 64kb blocks to compare at a time.
 GLOBAL VARIABLES
 COMMENTS, BUGS, ASSUMPTIONS
 EXAMPLES
 REVISION LOG
--------------------------------------------------------------------------*/
intn
memcmp_big(VOIDP s1, VOIDP s2, uint32 len)
{
    uint8      *t1 = (uint8 *) s1, *t2 = (uint8 *) s2;
    intn        ret_val;

    if (len <= UINT_MAX)    /* if the size is small enough copy all at once */
        return (memcmp(t1, t2, (size_t) len));
    else
      {     /* number of bytes to read */
          while (len > UINT_MAX)
            {
                ret_val = memcmp(t1, t2, UINT_MAX);
                if (ret_val != 0)
                    return (ret_val);
                t1 += UINT_MAX;
                t2 += UINT_MAX;
                len -= UINT_MAX;
            }   /* end while */
          if (len > 0)
              return (memcmp(t1, t2, (size_t) len));
      }     /* end else */
    return (0);
}   /* end memcmp_big() */
#endif /* WIN3 */

#endif /* WIN3 | PC */
