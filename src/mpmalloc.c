/*
 * mpatrol
 * A library for controlling and tracing dynamic memory allocations.
 * Copyright (C) 1997-2008 Graeme S. Roy <graemeroy@users.sourceforge.net>
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at         * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser
 * General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


/*
 * Release version of uninitialised memory allocation function.  This is used
 * by the version of MP_MALLOC() that is defined in mpalloc.h and will call a
 * failure handler function instead of returning NULL.
 */


#include "config.h"
#include "mpalloc.h"


#if MP_IDENT_SUPPORT
#ident "$Id: mpmalloc.c,v 1.9 2008-07-17 10:51:53 groy Exp $"
#else /* MP_IDENT_SUPPORT */
static MP_CONST MP_VOLATILE char *mpmalloc_id = "$Id: mpmalloc.c,v 1.9 2008-07-17 10:51:53 groy Exp $";
#endif /* MP_IDENT_SUPPORT */


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


extern __mp_failhandler __mp_failure;


/* Allocate an uninitialised memory block of a given size and abort on failure.
 */

MP_API
void *
__mp_xmalloc(size_t l, MP_CONST char *s, MP_CONST char *t, unsigned long u,
             MP_CONST char *g, size_t h)
{
    void *p;

    if (l == 0)
        if (h != 0)
            l = h;
        else
            l = 1;
    if ((p = malloc(l)) == NULL)
    {
        if (__mp_failure != NULL)
            __mp_failure(l, s, t, u, g, h);
        abort();
    }
    return p;
}


#ifdef __cplusplus
}
#endif /* __cplusplus */
