#ifndef MP_INFO_H
#define MP_INFO_H


/*
 * mpatrol
 * A library for controlling and tracing dynamic memory allocations.
 * Copyright (C) 1997-2001 Graeme S. Roy <graeme@epc.co.uk>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307, USA.
 */


/*
 * Allocation information.  The high-level details of every memory
 * allocation are stored by this module, while the low-level details
 * are dealt with by the memory allocation module.
 */


/*
 * $Id: info.h,v 1.42 2001-02-22 19:38:34 graeme Exp $
 */


#include "config.h"
#include "alloc.h"
#include "addr.h"
#include "profile.h"
#include "trace.h"
#include "signals.h"


#define FLG_CHECKALLOCS   1     /* check all memory allocations */
#define FLG_CHECKREALLOCS 2     /* check all memory reallocations */
#define FLG_CHECKFREES    4     /* check all memory deallocations */
#define FLG_CHECKMEMORY   8     /* check all memory operations */
#define FLG_LOGALLOCS     16    /* log all memory allocations */
#define FLG_LOGREALLOCS   32    /* log all memory reallocations */
#define FLG_LOGFREES      64    /* log all memory deallocations */
#define FLG_LOGMEMORY     128   /* log all memory operations */
#define FLG_SHOWFREE      256   /* show all free blocks */
#define FLG_SHOWFREED     512   /* show all freed allocations */
#define FLG_SHOWUNFREED   1024  /* show all unfreed allocations */
#define FLG_SHOWMAP       2048  /* show memory map of heap */
#define FLG_SHOWSYMBOLS   4096  /* show all symbols read */
#define FLG_ALLOWOFLOW    8192  /* allow memory operations to overflow */
#define FLG_SAFESIGNALS   16384 /* save and restore signal handlers */
#define FLG_NOPROTECT     32768 /* do not protect internal structures */

#define FLG_FREED         1     /* allocation has been freed */
#define FLG_PROFILED      2     /* allocation has been profiled */
#define FLG_TRACED        4     /* allocation has been traced */
#define FLG_INTERNAL      8     /* allocation was made inside the library */


/* The different types of memory allocation and operation functions.
 * This must be kept up to date with the definition of __mp_alloctype
 * in mpatrol.h.
 */

typedef enum alloctype
{
    AT_MALLOC,    /* malloc() */
    AT_CALLOC,    /* calloc() */
    AT_MEMALIGN,  /* memalign() */
    AT_VALLOC,    /* valloc() */
    AT_PVALLOC,   /* pvalloc() */
    AT_ALLOCA,    /* alloca() */
    AT_STRDUP,    /* strdup() */
    AT_STRNDUP,   /* strndup() */
    AT_STRSAVE,   /* strsave() */
    AT_STRNSAVE,  /* strnsave() */
    AT_STRDUPA,   /* strdupa() */
    AT_STRNDUPA,  /* strndupa() */
    AT_REALLOC,   /* realloc() */
    AT_REALLOCF,  /* reallocf() */
    AT_RECALLOC,  /* recalloc() */
    AT_EXPAND,    /* expand() */
    AT_FREE,      /* free() */
    AT_CFREE,     /* cfree() */
    AT_DEALLOCA,  /* dealloca() */
    AT_XMALLOC,   /* xmalloc() */
    AT_XCALLOC,   /* xcalloc() */
    AT_XSTRDUP,   /* xstrdup() */
    AT_XREALLOC,  /* xrealloc() */
    AT_XFREE,     /* xfree() */
    AT_NEW,       /* operator new */
    AT_NEWVEC,    /* operator new[] */
    AT_DELETE,    /* operator delete */
    AT_DELETEVEC, /* operator delete[] */
    AT_MEMSET,    /* memset() */
    AT_BZERO,     /* bzero() */
    AT_MEMCCPY,   /* memccpy() */
    AT_MEMCPY,    /* memcpy() */
    AT_MEMMOVE,   /* memmove() */
    AT_BCOPY,     /* bcopy() */
    AT_MEMCHR,    /* memchr() */
    AT_MEMMEM,    /* memmem() */
    AT_MEMCMP,    /* memcmp() */
    AT_BCMP,      /* bcmp() */
    AT_MAX
}
alloctype;


/* An allocanode belongs to a stack of memory allocations that were made
 * by the alloca() and related functions.  Some memory allocations at the
 * top of the stack may be automatically freed when the next call to an
 * mpatrol function is made and the stack has been unwound.
 */

typedef struct allocanode
{
    listnode node;   /* list node */
    void *block;     /* pointer to block of memory */
    union
    {
        size_t size; /* size of block of memory */
        void *frame; /* pointer to stack frame */
    }
    data;
}
allocanode;


/* A loginfo structure is used to pass around information about a calling
 * function.
 */

typedef struct loginfo
{
    char *func;         /* calling function name */
    char *file;         /* file name in which call took place */
    unsigned long line; /* line number at which call took place */
    stackinfo *stack;   /* call stack details */
    char *typestr;      /* type stored in allocation */
    size_t typesize;    /* size of type stored in allocation */
}
loginfo;


/* The structure used to record source level information about recursive
 * calls to C++ operator delete and operator delete[].
 */

typedef struct delstack
{
    char *func;         /* calling function name */
    char *file;         /* file name in which call took place */
    unsigned long line; /* line number at which call took place */
}
delstack;


/* An allocation information node belongs to a table of nodes, although
 * details of internal memory allocations are also stored in allocation
 * information nodes as part of a list.
 */

typedef union infonode
{
    struct
    {
        listnode node;         /* internal list node */
        void *block;           /* pointer to block of memory */
        size_t size;           /* size of block of memory */
    }
    index;
    struct
    {
        alloctype type;        /* type of memory allocation */
        unsigned long alloc;   /* allocation index */
        unsigned long realloc; /* reallocation index */
#if MP_THREADS_SUPPORT
        unsigned long thread;  /* thread identifier */
#endif /* MP_THREADS_SUPPORT */
        unsigned long event;   /* event of last modification */
        char *func;            /* calling function name */
        char *file;            /* file name in which call took place */
        unsigned long line;    /* line number at which call took place */
        addrnode *stack;       /* call stack details */
        char *typestr;         /* type stored in allocation */
        size_t typesize;       /* size of type stored in allocation */
        void *userdata;        /* user data associated with allocation */
        unsigned long flags;   /* allocation flags */
    }
    data;
}
infonode;


/* An infohead holds the table of allocation information nodes as well
 * as all of the other data structures used by the mpatrol library.
 */

typedef struct infohead
{
    allochead alloc;                  /* allocation table */
    addrhead addr;                    /* stack address table */
    symhead syms;                     /* symbol table */
    sighead signals;                  /* signal handler table */
    profhead prof;                    /* profiling information */
    tracehead trace;                  /* tracing information */
    slottable table;                  /* table of information nodes */
    slottable atable;                 /* table of allocanodes */
    listhead list;                    /* internal allocation list */
    listhead alist;                   /* internal allocanode list */
    listhead astack;                  /* alloca allocation stack */
    size_t size;                      /* internal allocation total */
    size_t event;                     /* allocation event count */
    size_t count;                     /* allocation count */
    size_t cpeak;                     /* allocation count peak */
    size_t peak;                      /* allocation peak */
    size_t limit;                     /* allocation limit */
    size_t astop;                     /* allocation stop index */
    size_t rstop;                     /* reallocation stop index */
    size_t fstop;                     /* free stop index */
    size_t uabort;                    /* unfreed abort minimum */
    size_t lrange;                    /* lower check range */
    size_t urange;                    /* upper check range */
    size_t check;                     /* check frequency */
    size_t dtotal;                    /* total bytes compared */
    size_t ltotal;                    /* total bytes located */
    size_t ctotal;                    /* total bytes copied */
    size_t stotal;                    /* total bytes set */
    unsigned long ffreq;              /* failure frequency */
    unsigned long fseed;              /* failure seed */
    void (*prologue)(void *, size_t,
                     void *);         /* prologue function */
    void (*epilogue)(void *, void *); /* epilogue function */
    void (*nomemory)(void);           /* low-memory handler function */
    char *log;                        /* log filename */
    delstack dels[MP_MAXDELSTACK];    /* delete stack */
    long delpos;                      /* delete stack pointer */
    unsigned long flags;              /* global flags */
    memaccess prot;                   /* protection status */
    size_t recur;                     /* recursion count */
    char init;                        /* initialisation flag */
    char fini;                        /* finalisation flag */
}
infohead;


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


MP_EXPORT void __mp_newinfo(infohead *);
MP_EXPORT void __mp_deleteinfo(infohead *);
MP_EXPORT void *__mp_getmemory(infohead *, size_t, size_t, alloctype,
                               loginfo *);
MP_EXPORT void *__mp_resizememory(infohead *, void *, size_t, size_t, alloctype,
                                  loginfo *);
MP_EXPORT void __mp_freememory(infohead *, void *, alloctype, loginfo *);
MP_EXPORT void __mp_setmemory(infohead *, void *, size_t, unsigned char,
                              alloctype, loginfo *);
MP_EXPORT void *__mp_copymemory(infohead *, void *, void *, size_t,
                                unsigned char, alloctype, loginfo *);
MP_EXPORT void *__mp_locatememory(infohead *, void *, size_t, void *, size_t,
                                  alloctype, loginfo *);
MP_EXPORT int __mp_comparememory(infohead *, void *, void *, size_t, alloctype,
                                 loginfo *);
MP_EXPORT int __mp_protectinfo(infohead *, memaccess);
MP_EXPORT void __mp_checkinfo(infohead *);
MP_EXPORT int __mp_checkrange(infohead *, void *, size_t, alloctype, loginfo *);
MP_EXPORT int __mp_checkstring(infohead *, char *, size_t *, alloctype,
                               loginfo *, int);


#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* MP_INFO_H */
