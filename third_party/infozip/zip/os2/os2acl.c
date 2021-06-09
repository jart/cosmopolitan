/*
  Copyright (c) 1990-2000 Info-ZIP.  All rights reserved.

  See the accompanying file LICENSE, version 2000-Apr-09 or later
  (the contents of which are also included in zip.h) for terms of use.
  If, for some reason, all these files are missing, the Info-ZIP license
  also may be found at:  ftp://ftp.info-zip.org/pub/infozip/license.html
*/
/* os2acl.c - access to OS/2 (LAN Server) ACLs
 *
 * Author:  Kai Uwe Rommel <rommel@ars.de>
 * Created: Mon Aug 08 1994
 *
 */

/*
 * supported 32-bit compilers:
 * - emx+gcc
 * - IBM C Set++ 2.1 or newer
 * - Watcom C/C++ 10.0 or newer
 *
 * supported 16-bit compilers:
 * - MS C 6.00A
 * - Watcom C/C++ 10.0 or newer
 *
 * supported OS/2 LAN environments:
 * - IBM LAN Server/Requester 3.0, 4.0 and 5.0 (Warp Server)
 * - IBM Peer 1.0 (Warp Connect)
 */

#ifdef KUR
   static char *rcsid =
   "$Id: os2acl.c,v 1.3 1996/04/03 19:18:27 rommel Exp rommel $";
   static char *rcsrev = "$Revision: 1.3 $";
#endif

/*
 * $Log: os2acl.c,v $
 * Revision 1.3  1996/04/03 19:18:27  rommel
 * minor fixes
 *
 * Revision 1.2  1996/03/30 22:03:52  rommel
 * avoid frequent dynamic allocation for every call
 * streamlined code
 *
 * Revision 1.1  1996/03/30 09:35:00  rommel
 * Initial revision
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>

#define INCL_NOPM
#define INCL_DOS
#define INCL_DOSERRORS
#include <os2.h>

#include "os2/os2acl.h"

#define UNLEN 20

#if defined(__WATCOMC__) && defined(__386__) && !defined(__32BIT__)
#define __32BIT__
#endif

#ifdef __32BIT__
typedef ULONG U_INT;
#ifdef __EMX__
#define PSTR16 _far16ptr
#define PTR16(x) _emx_32to16(x)
#else /* other 32-bit */
#define PSTR16 PCHAR16
#define PTR16(x) ((PCHAR16)(x))
#endif
#else /* 16-bit */
typedef USHORT U_INT;
#define PSTR16 PSZ
#define PTR16(x) (x)
#endif

typedef struct access_list
{
  char acl_ugname[UNLEN+1];
  char acl_pad;
  USHORT acl_access;
}
ACCLIST;

typedef struct access_info
{
  PSTR16 acc_resource_name;
  USHORT acc_attr;
  USHORT acc_count;
}
ACCINFO;

static ACCINFO *ai;
static char *path, *data;

#ifdef __32BIT__

#ifdef __EMX__

static USHORT (APIENTRY *_NetAccessGetInfo)(PSZ pszServer, PSZ pszResource,
  USHORT sLevel, PVOID pbBuffer, USHORT cbBuffer, PUSHORT pcbTotalAvail);
static USHORT (APIENTRY *_NetAccessSetInfo)(PSZ pszServer, PSZ pszResource,
  USHORT sLevel, PVOID pbBuffer, USHORT cbBuffer, USHORT sParmNum);
static USHORT (APIENTRY *_NetAccessAdd)(PSZ pszServer,
  USHORT sLevel, PVOID pbBuffer, USHORT cbBuffer);

USHORT NetAccessGetInfo(PSZ pszServer, PSZ pszResource, USHORT sLevel,
                        PVOID pbBuffer, USHORT cbBuffer, PUSHORT pcbTotalAvail)
{
  return (USHORT)
          (_THUNK_PROLOG (4+4+2+4+2+4);
           _THUNK_FLAT (pszServer);
           _THUNK_FLAT (pszResource);
           _THUNK_SHORT (sLevel);
           _THUNK_FLAT (pbBuffer);
           _THUNK_SHORT (cbBuffer);
           _THUNK_FLAT (pcbTotalAvail);
           _THUNK_CALLI (_emx_32to16(_NetAccessGetInfo)));
}

USHORT NetAccessSetInfo(PSZ pszServer, PSZ pszResource, USHORT sLevel,
                        PVOID pbBuffer, USHORT cbBuffer, USHORT sParmNum)
{
  return (USHORT)
          (_THUNK_PROLOG (4+4+2+4+2+2);
           _THUNK_FLAT (pszServer);
           _THUNK_FLAT (pszResource);
           _THUNK_SHORT (sLevel);
           _THUNK_FLAT (pbBuffer);
           _THUNK_SHORT (cbBuffer);
           _THUNK_SHORT (sParmNum);
           _THUNK_CALLI (_emx_32to16(_NetAccessSetInfo)));
}

USHORT NetAccessAdd(PSZ pszServer, USHORT sLevel,
                    PVOID pbBuffer, USHORT cbBuffer)
{
  return (USHORT)
          (_THUNK_PROLOG (4+2+4+2);
           _THUNK_FLAT (pszServer);
           _THUNK_SHORT (sLevel);
           _THUNK_FLAT (pbBuffer);
           _THUNK_SHORT (cbBuffer);
           _THUNK_CALLI (_emx_32to16(_NetAccessAdd)));
}

#else /* other 32-bit */

APIRET16 (* APIENTRY16 NetAccessGetInfo)(PCHAR16 pszServer, PCHAR16 pszResource,
  USHORT sLevel, PVOID16 pbBuffer, USHORT cbBuffer, PVOID16 pcbTotalAvail);
APIRET16 (* APIENTRY16 NetAccessSetInfo)(PCHAR16 pszServer, PCHAR16 pszResource,
  USHORT sLevel, PVOID16 pbBuffer, USHORT cbBuffer, USHORT sParmNum);
APIRET16 (* APIENTRY16 NetAccessAdd)(PCHAR16 pszServer,
  USHORT sLevel, PVOID16 pbBuffer, USHORT cbBuffer);

#define _NetAccessGetInfo NetAccessGetInfo
#define _NetAccessSetInfo NetAccessSetInfo
#define _NetAccessAdd NetAccessAdd

#if !defined(__IBMC__) || !defined(__TILED__)
#define _tmalloc malloc
#define _tfree free
#endif

#endif
#else /* 16-bit */

USHORT (APIENTRY *NetAccessGetInfo)(PSZ pszServer, PSZ pszResource,
  USHORT sLevel, PVOID pbBuffer, USHORT cbBuffer, PUSHORT pcbTotalAvail);
USHORT (APIENTRY *NetAccessSetInfo)(PSZ pszServer, PSZ pszResource,
  USHORT sLevel, PVOID pbBuffer, USHORT cbBuffer, USHORT sParmNum);
USHORT (APIENTRY *NetAccessAdd)(PSZ pszServer,
  USHORT sLevel, PVOID pbBuffer, USHORT cbBuffer);

#define _NetAccessGetInfo NetAccessGetInfo
#define _NetAccessSetInfo NetAccessSetInfo
#define _NetAccessAdd NetAccessAdd

#define _tmalloc malloc
#define _tfree free

#define DosQueryProcAddr(handle, ord, name, funcptr) \
        DosGetProcAddr(handle, name, funcptr)
#define DosQueryCurrentDir DosQCurDir
#define DosQueryCurrentDisk DosQCurDisk

#endif


static BOOL acl_init(void)
{
  static BOOL initialized, netapi_avail;
  HMODULE netapi;
  char buf[256];

  if (initialized)
    return netapi_avail;

  initialized = TRUE;

  if (DosLoadModule(buf, sizeof(buf), "NETAPI", &netapi))
    return FALSE;

  if (DosQueryProcAddr(netapi, 0, "NETACCESSGETINFO", (PFN *) &_NetAccessGetInfo) ||
      DosQueryProcAddr(netapi, 0, "NETACCESSSETINFO", (PFN *) &_NetAccessSetInfo) ||
      DosQueryProcAddr(netapi, 0, "NETACCESSADD", (PFN *) &_NetAccessAdd))
    return FALSE;

#if defined(__WATCOMC__) && defined(__386__)
  NetAccessGetInfo = (PVOID) (ULONG) (PVOID16) NetAccessGetInfo;
  NetAccessSetInfo = (PVOID) (ULONG) (PVOID16) NetAccessSetInfo;
  NetAccessAdd     = (PVOID) (ULONG) (PVOID16) NetAccessAdd;
#endif

  if ((path = _tmalloc(CCHMAXPATH)) == NULL)
    return FALSE;
  if ((data = _tmalloc(ACL_BUFFERSIZE)) == NULL)
    return FALSE;
  if ((ai = _tmalloc(sizeof(ACCINFO))) == NULL)
    return -1;

  netapi_avail = TRUE;

  return netapi_avail;
}

static void acl_mkpath(char *buffer, const char *source)
{
  char *ptr;
  static char cwd[CCHMAXPATH];
  static U_INT cwdlen;
  U_INT cdrive;
  ULONG drivemap;

  if (isalpha((int)source[0]) && source[1] == ':')
    buffer[0] = 0; /* fully qualified names */
  else
  {
    if (cwd[0] == 0)
    {
      DosQueryCurrentDisk(&cdrive, &drivemap);
      cwd[0] = (char)(cdrive + '@');
      cwd[1] = ':';
      cwd[2] = '\\';
      cwdlen = sizeof(cwd) - 3;
      DosQueryCurrentDir(0, cwd + 3, &cwdlen);
      cwdlen = strlen(cwd);
    }

    if (source[0] == '/' || source[0] == '\\')
    {
      if (source[1] == '/' || source[1] == '\\')
        buffer[0] = 0; /* UNC names */
      else
      {
        strncpy(buffer, cwd, 2);
        buffer[2] = 0;
      }
    }
    else
    {
      strcpy(buffer, cwd);
      if (cwd[cwdlen - 1] != '\\' && cwd[cwdlen - 1] != '/')
        strcat(buffer, "/");
    }
  }

  strcat(buffer, source);

  for (ptr = buffer; *ptr; ptr++)
    if (*ptr == '/')
      *ptr = '\\';

  if (ptr[-1] == '\\')
    ptr[-1] = 0;

  strupr(buffer);
}

static int acl_bin2text(char *data, char *text)
{
  ACCINFO *ai;
  ACCLIST *al;
  U_INT cnt, offs;

  ai = (ACCINFO *) data;
  al = (ACCLIST *) (data + sizeof(ACCINFO));

  offs = sprintf(text, "ACL1:%X,%d\n",
                 ai -> acc_attr, ai -> acc_count);

  for (cnt = 0; cnt < ai -> acc_count; cnt++)
    offs += sprintf(text + offs, "%s,%X\n",
                    al[cnt].acl_ugname, al[cnt].acl_access);

  return strlen(text);
}

int acl_get(char *server, const char *resource, char *buffer)
{
  USHORT datalen;
  PSZ srv = NULL;
  int rc;

  if (!acl_init())
    return -1;

  if (server)
    srv = server;

  acl_mkpath(path, resource);
  datalen = 0;

  rc = NetAccessGetInfo(srv, path, 1, data, ACL_BUFFERSIZE, &datalen);

  if (rc == 0)
    acl_bin2text(data, buffer);

  return rc;
}

static int acl_text2bin(char *data, char *text, char *path)
{
  ACCINFO *ai;
  ACCLIST *al;
  char *ptr, *ptr2;
  U_INT cnt;

  ai = (ACCINFO *) data;
  ai -> acc_resource_name = PTR16(path);

  if (sscanf(text, "ACL1:%hX,%hd",
             &ai -> acc_attr, &ai -> acc_count) != 2)
    return ERROR_INVALID_PARAMETER;

  al = (ACCLIST *) (data + sizeof(ACCINFO));
  ptr = strchr(text, '\n') + 1;

  for (cnt = 0; cnt < ai -> acc_count; cnt++)
  {
    ptr2 = strchr(ptr, ',');
    strncpy(al[cnt].acl_ugname, ptr, ptr2 - ptr);
    al[cnt].acl_ugname[ptr2 - ptr] = 0;
    sscanf(ptr2 + 1, "%hx", &al[cnt].acl_access);
    ptr = strchr(ptr, '\n') + 1;
  }

  return sizeof(ACCINFO) + ai -> acc_count * sizeof(ACCLIST);
}

int acl_set(char *server, const char *resource, char *buffer)
{
  USHORT datalen;
  PSZ srv = NULL;

  if (!acl_init())
    return -1;

  if (server)
    srv = server;

  acl_mkpath(path, resource);

  ai -> acc_resource_name = PTR16(path);
  ai -> acc_attr = 0;
  ai -> acc_count = 0;

  NetAccessAdd(srv, 1, ai, sizeof(ACCINFO));
  /* Ignore any errors, most probably because ACL already exists. */
  /* In any such case, try updating the existing ACL. */

  datalen = acl_text2bin(data, buffer, path);

  return NetAccessSetInfo(srv, path, 1, data, datalen, 0);
}

/* end of os2acl.c */
