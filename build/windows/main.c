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
 * DLL entry point.  Provides a way to initialise and finalise the mpatrol
 * library when it is built as a DLL.
 */


#include "inter.h"
#include <windows.h>
#include <winbase.h>


#if MP_IDENT_SUPPORT
#ident "$Id: main.c,v 1.5 2001-02-05 23:15:05 graeme Exp $"
#else /* MP_IDENT_SUPPORT */
static MP_CONST MP_VOLATILE char *main_id = "$Id: main.c,v 1.5 2001-02-05 23:15:05 graeme Exp $";
#endif /* MP_IDENT_SUPPORT */


#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */


/* DLL entry point to initialise and finalise the library.
 */

BOOL
WINAPI
DllMain(HANDLE h, DWORD r, LPVOID d)
{
    switch (r)
    {
      case DLL_PROCESS_ATTACH:
        __mp_init();
        break;
      case DLL_PROCESS_DETACH:
        __mp_fini();
        break;
      default:
        break;
    }
    return TRUE;
}


#ifdef __cplusplus
}
#endif /* __cplusplus */
