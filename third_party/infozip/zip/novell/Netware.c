#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include <ntypes.h>
#include <nwconio.h> 
#include <ctype.h>
#include <unistd.h>
#include <nit\nwdir.h>
#include <dirent.h>
#include <nwnamspc.h> 
#include <locale.h>
#include <nwlocale.h>
#include <time.h>

extern void UseAccurateCaseForPaths(int);

#include "zip.h"

   /*------------------------------------------------------------------
   **   Global Variables
   */

#define   skipspace( x )  while( isspace( *x ) ) ++x
#define   nextspace( x )  while( *x && !isspace( *x ) ) ++x
#define   CWS     0
#define   CWV     1
#define   CWP     2
#define   ALL     99

/* Globals */
extern      char   *GetWorkArea(void);
extern      char   *next_arg(char *);
extern		int NLM_exiting;
char        fid[100];
static      breakkey = FALSE;

#define MATCH shmatch

extern char *label;
local ulg label_time = 0;
local ulg label_mode = 0;
local time_t label_utim = 0;

#define PAD 0
#define PATH_END '/'

local char *readd(d)
DIR *d;                 /* directory stream to read from */
/* Return a pointer to the next name in the directory stream d, or NULL if
   no more entries or an error occurs. */
{
  struct dirent *e;

  e = readdir(d);
  return e == NULL ? (char *) NULL : e->d_name;
}

void findzip(char *s)
{
   dowhereis(s);
} 

void dowhereis(char *s)
{
   char    dir[_MAX_PATH];
   char    fsv[_MAX_SERVER+_MAX_VOLUME+1];
   char    fdir[_MAX_PATH];
   char    fname[_MAX_FNAME],fext[_MAX_EXT], both[_MAX_FNAME+_MAX_EXT];
   char    *p = next_arg(s); /* point at argument */

   if(!*p)
   {
      printf("No filename specified!");
      return;
   }

   //setlocale (LC_ALL, "NORWAY");
   NWLsetlocale (LC_ALL, "NORWAY");

   strcpy(dir,GetWorkArea());

   /* get the file name specification */
   _splitpath(p,fsv,fdir,fname,fext);

   //printf ("p %s, fsv %s, fdir %s, fname %s, fext %s\n", p,fsv,fdir,fname,fext);
   //getch();
   
   sprintf(both,"%s%s",strupr(fname),strupr(fext));

   breakkey = FALSE;

   /* startup the recursive file find operation */
   chdir(fsv);
   UseAccurateCaseForPaths(1);
   SetCurrentNameSpace (NW_NS_LONG);
   chdir(fdir);
   findit(both);
}

char *GetWorkArea(void)
{
   static  char   cwd[_MAX_PATH];
   static  char   serverName[_MAX_SERVER];
   static  char   volumeName[_MAX_VOLUME + 1];
   static  char   dirName[_MAX_DIR];

   if(getcwd(cwd,_MAX_PATH) == NULL) 
      return NULL;

   ParsePath(cwd,serverName,volumeName,dirName);   /* shouldn't fail! */

   return cwd;
}

char *next_arg(char *s)
{
   char    *p;

   skipspace(s);     /* ignore white */
   p = s;
   nextspace(s);     /* find next blank */
   *s = NULL;
   return(p);
}

static void findit(char *what)
{
   char dir[_MAX_PATH];
   char zipdir[_MAX_PATH];
   char szzipfile[_MAX_PATH];
   char *psz;
   DIR *dirStructPtr;
   DIR *dirStructPtrSave;
   int r;

   getcwd(dir,_MAX_PATH);

   psz = dir;

	  while (*psz)
	  {
		  if (*psz == ':')
		  {
			  strcpy (zipdir, psz + 1);
			  break;
		  }
		  psz++;
	  }

   dirStructPtrSave = dirStructPtr = opendir(what);

	/*
	_A_NORMAL Normal file; read/write permitted 
	_A_RDONLY Read-only file 
	_A_HIDDEN Hidden file 
	_A_SYSTEM System file 
	_A_VOLID Volume ID entry 
	_A_SUBDIR Subdirectory 
	_A_ARCH Archive file 
	*/

   if (hidden_files)
	  SetReaddirAttribute (dirStructPtr, _A_NORMAL | _A_RDONLY | _A_HIDDEN | _A_SYSTEM | _A_ARCH);
   else
	  SetReaddirAttribute (dirStructPtr, _A_NORMAL | _A_ARCH);

   //while(dirStructPtr && !breakkey)
   while(dirStructPtr && !NLM_exiting)
   {
	  //printf ("\n NLM_exiting test Line 167.... \n");
	   
      dirStructPtr = readdir(dirStructPtr);
      if((dirStructPtr == NULL) || (dirStructPtr == -1))
         break;
      
	  /* Filen er funnet */
      if(dirStructPtr->d_attr & _A_SUBDIR)
		continue;

	  strcpy (szzipfile, zipdir);
	  strcat (szzipfile, "/");
	  strcat (szzipfile, dirStructPtr->d_name);
	  procnamehho (szzipfile);

	  //ThreadSwitchWithDelay();

      //if(kbhit() && getch() == 3) 
        // printf("^C\n",breakkey = TRUE);
   }

   if(dirStructPtrSave) 
      closedir(dirStructPtrSave);

   if (!recurse)
	   return;

   /*  Now traverse the directories in this path */

   dirStructPtrSave = dirStructPtr = opendir("*.*");
   if(dirStructPtr == NULL) 
      return;

   if (hidden_files)
		SetReaddirAttribute (dirStructPtr, _A_NORMAL | _A_RDONLY | _A_HIDDEN | _A_SYSTEM | _A_ARCH | _A_SUBDIR);
	 else
		SetReaddirAttribute (dirStructPtr, _A_NORMAL | _A_ARCH | _A_SUBDIR);

   //ThreadSwitchWithDelay();

   while(!NLM_exiting)
   {
	  //printf ("\n NLM_exiting test Line 204.... \n"); getch ();

      dirStructPtr = readdir(dirStructPtr);
      if((dirStructPtr == NULL) || (dirStructPtr == -1))
         break;
      
      if(dirStructPtr->d_attr & _A_SUBDIR)
      {
		 strcpy (szzipfile, zipdir);
		 strcat (szzipfile, "/");
		 strcat (szzipfile, dirStructPtr->d_name);
	  	 procnamehho (szzipfile);

         chdir(dirStructPtr->d_name);
         findit(what);                
         chdir("..");
      }

      //if(kbhit() && getch() == 3) 
        // printf("^C\n",breakkey = TRUE);
   }

   if(dirStructPtrSave) 
      closedir(dirStructPtrSave);
} 


int wild(w)
char *w;                /* path/pattern to match */
/* If not in exclude mode, expand the pattern based on the contents of the
   file system.  Return an error code in the ZE_ class. */
{
  DIR *d;               /* stream for reading directory */
  char *e;              /* name found in directory */
  int r;                /* temporary variable */
  char *n;              /* constructed name from directory */
  int f;                /* true if there was a match */
  char *a;              /* alloc'ed space for name */
  //char *p;              /* path */
  char *q;              /* name */
  char v[5];            /* space for device current directory */


  char    dir[_MAX_PATH];
  char    fsv[_MAX_SERVER+_MAX_VOLUME+1];
  char    fdir[_MAX_PATH];
  char    fname[_MAX_FNAME],fext[_MAX_EXT], both[_MAX_FNAME+_MAX_EXT];
  char    *p; /* point at argument */

  p = w;


  /* Test HHO */
  findzip(p);

  return ZE_OK;


  strcpy(dir,GetWorkArea());

   /* get the file name specification */

   _splitpath(p,fsv,fdir,fname,fext);
   sprintf(both,"%s%s",strupr(fname),strupr(fext));

   /* startup the recursive file find operation */

   chdir(fsv);
   
  /* Search that level for matching names */
  if ((d = opendir(both)) == NULL)
  {
    free((zvoid *)a);
    return ZE_MISS;
  }
  
  f = 0;
  while ((e = readd(d)) != NULL) {
    if (strcmp(e, ".") && strcmp(e, "..") && MATCH(q, e))
    {
      f = 1;
      if (strcmp(p, ".") == 0) {                /* path is . */
        r = procname(e);                        /* name is name */
        if (r) {
           f = 0;
           break;
        }
      } else
      {
        if ((n = malloc(strlen(p) + strlen(e) + 2)) == NULL)
        {
          free((zvoid *)a);
          closedir(d);
          return ZE_MEM;
        }
        n = strcpy(n, p);
        if (n[r = strlen(n) - 1] != '/' && n[r] != ':')
          strcat(n, "/");
        r = procname(strcat(n, e));             /* name is path/name */
        free((zvoid *)n);
        if (r) {
          f = 0;
          break;
        }
      }
    }
  }
  closedir(d);

  /* Done */
  free((zvoid *)a);
  return f ? ZE_OK : ZE_MISS;
}

int procnamehho (char *n)
{
  int m;                /* matched flag */
  char *p;              /* path for recursion */
  struct stat s;        /* result of stat() */
  struct zlist far *z;  /* steps through zfiles list */

  char *a;

  if (n == NULL)        /* volume_label request in freshen|delete mode ?? */
    return ZE_OK;

  if (strcmp(n, "-") == 0)   /* if compressing stdin */
    return newname(n, 0);
  else if (stat(n, &s)
#if defined(__TURBOC__) || defined(__WATCOMC__)
           /* For these 2 compilers, stat() succeeds on wild card names! */
           || isshexp(n)
#endif
          )
  {
    /* Not a file or directory--search for shell expression in zip file */
    p = ex2in(n, 0, (int *)NULL);       /* shouldn't affect matching chars */
    m = 1;
    for (z = zfiles; z != NULL; z = z->nxt) {
      if (MATCH(p, z->iname))
      {
        z->mark = pcount ? filter(z->zname) : 1;
        if (verbose)
            fprintf(mesg, "zip diagnostic: %scluding %s\n",
               z->mark ? "in" : "ex", z->name);
        m = 0;
      }
    }
    free((zvoid *)p);
    return m ? ZE_MISS : ZE_OK;
  }

  /* Live name--use if file, recurse if directory */
  for (p = n; *p; p++)          /* use / consistently */
    if (*p == '\\')
      *p = '/';

  //printf ("\nHHO %s\n", n);
  if ((s.st_mode & S_IFDIR) == 0)
  {
	//printf ("\nHHO1 %s\n", n);
    /* add or remove name of file */
	//printf ("\nAdding name %s to list.\n", n);
    if ((m = newname(n, 0)) != ZE_OK)
      return m;
  } else {
    
	/* Add trailing / to the directory name */
    if ((p = malloc(strlen(n)+2)) == NULL)
     return ZE_MEM;
    if (strcmp(n, ".") == 0 || strcmp(n, "/.") == 0) {
      *p = '\0';  /* avoid "./" prefix and do not create zip entry */
    } else {
      strcpy(p, n);
      a = p + strlen(p);
      if (a[-1] != '/')
        strcpy(a, "/");
    //if (dirnames && (m = newname(p, 1)) != ZE_OK) {
	  if ((m = newname(p, 1)) != ZE_OK) {
        free((zvoid *)p);
        return m;
      }
	  free ((zvoid *)p);
    }
      
	return ZE_OK;
  }
  return ZE_OK;
}

int procname(n)
char *n;                /* name to process */
/* Process a name or sh expression to operate on (or exclude).  Return
   an error code in the ZE_ class. */
{
  char *a;              /* path and name for recursion */
  DIR *d;               /* directory stream from opendir() */
  char *e;              /* pointer to name from readd() */
  int m;                /* matched flag */
  char *p;              /* path for recursion */
  struct stat s;        /* result of stat() */
  struct zlist far *z;  /* steps through zfiles list */

  if (n == NULL)        /* volume_label request in freshen|delete mode ?? */
    return ZE_OK;

  if (strcmp(n, "-") == 0)   /* if compressing stdin */
    return newname(n, 0);
  else if (stat(n, &s)
#if defined(__TURBOC__) || defined(__WATCOMC__)
           /* For these 2 compilers, stat() succeeds on wild card names! */
           || isshexp(n)
#endif
          )
  {
    /* Not a file or directory--search for shell expression in zip file */
    p = ex2in(n, 0, (int *)NULL);       /* shouldn't affect matching chars */
    m = 1;
    for (z = zfiles; z != NULL; z = z->nxt) {
      if (MATCH(p, z->iname))
      {
        z->mark = pcount ? filter(z->zname) : 1;
        if (verbose)
            fprintf(mesg, "zip diagnostic: %scluding %s\n",
               z->mark ? "in" : "ex", z->name);
        m = 0;
      }
    }
    free((zvoid *)p);
    return m ? ZE_MISS : ZE_OK;
  }

  /* Live name--use if file, recurse if directory */
  for (p = n; *p; p++)          /* use / consistently */
    if (*p == '\\')
      *p = '/';
  if ((s.st_mode & S_IFDIR) == 0)
  {
    /* add or remove name of file */
    if ((m = newname(n, 0)) != ZE_OK)
      return m;
  } else {
    /* Add trailing / to the directory name */
    if ((p = malloc(strlen(n)+2)) == NULL)
      return ZE_MEM;
    if (strcmp(n, ".") == 0 || strcmp(n, "/.") == 0) {
      *p = '\0';  /* avoid "./" prefix and do not create zip entry */
    } else {
      strcpy(p, n);
      a = p + strlen(p);
      if (a[-1] != '/')
        strcpy(a, "/");
      if (dirnames && (m = newname(p, 1)) != ZE_OK) {
        free((zvoid *)p);
        return m;
      }
    }
    /* recurse into directory */
    if (recurse && (d = opendir(n)) != NULL)
    {
      while ((e = readd(d)) != NULL) {
        if (strcmp(e, ".") && strcmp(e, ".."))
        {
          if ((a = malloc(strlen(p) + strlen(e) + 1)) == NULL)
          {
            closedir(d);
            free((zvoid *)p);
            return ZE_MEM;
          }
          strcat(strcpy(a, p), e);
          if ((m = procname(a)) != ZE_OK)   /* recurse on name */
          {
            if (m == ZE_MISS)
              zipwarn("name not matched: ", a);
            else
              ziperr(m, a);
          }
          free((zvoid *)a);
        }
      }
      closedir(d);
    }
    free((zvoid *)p);
  } /* (s.st_mode & S_IFDIR) == 0) */
  return ZE_OK;
}

char *szRelativParameter;
char szRelativ[512];
int	iRelativOK = FALSE;
int	iRelativPakking = FALSE;

int fixRelativpath ()
{
	char *szp;
	
	szp = szRelativParameter;

	if (szRelativParameter[0] == '/' || szRelativParameter[0] == '\\')
		szp++;

	while (*szp) {
		if (*szp == '\\')
			*szp = '/';
		szp++;
	}

	szp = szRelativParameter;
	if (szRelativParameter[0] == '/')
		szp++;

	strcpy (szRelativ, szp);

	if (strlen(szp) == 0) {
		szRelativ[0] = '\0';
		return FALSE;
	}
	return TRUE;
}


char *ex2in(x, isdir, pdosflag)
char *x;                /* external file name */
int isdir;              /* input: x is a directory */
int *pdosflag;          /* output: force MSDOS file attributes? */
/* Convert the external file name to a zip file name, returning the malloc'ed
   string or NULL if not enough memory. */
{
  char *n;              /* internal file name (malloc'ed) */
  char *t;              /* shortened name */
  int dosflag;
  char *sztUpper;
  
  
  /* Find starting point in name before doing malloc */
  t = *x && *(x + 1) == ':' ? x + 2 : x;
  while (*t == '/' || *t == '\\')
    t++;

  /* Make changes, if any, to the copied name (leave original intact) */
  for (n = t; *n; n++)
    if (*n == '\\')
      *n = '/';
	
  if (iRelativPakking) {
	  //printf ("\n LINE 516  *ex2ex Internt navn %s external name %s.\n", t, x); getch ();
	  if (!iRelativOK) {
		  if (!fixRelativpath()) {
			iRelativOK = FALSE;
			iRelativPakking = FALSE;
		  }
		  else {
			sztUpper = malloc (strlen(t) + 10);
			strcpy (sztUpper, t);
			NWLstrupr (sztUpper);
			NWLstrupr (szRelativ);
			if (strncmp (sztUpper, szRelativ, strlen(szRelativ)) == 0) {
				t = t + strlen(szRelativ);
				iRelativPakking = TRUE;
				iRelativOK = TRUE;
			}
			else {
				iRelativOK = FALSE;
				iRelativPakking = FALSE;
			}
			free (sztUpper);
		  }
	  }
	  else 
	  {
		t = t + strlen(szRelativ);
	  }
  }
  
  if (!pathput)
    t = last(t, PATH_END);

  /* Malloc space for internal name and copy it */
  if ((n = malloc(strlen(t) + 1)) == NULL)
    return NULL;
  strcpy(n, t);

  if (dosify)
    msname(n);

  /* Returned malloc'ed name */
  if (pdosflag)
    *pdosflag = dosflag;

  return n;
}


char *in2ex(n)
char *n;                /* internal file name */
/* Convert the zip file name to an external file name, returning the malloc'ed
   string or NULL if not enough memory. */
{
  char *x;              /* external file name */

  if ((x = malloc(strlen(n) + 1 + PAD)) == NULL)
    return NULL;
  strcpy(x, n);

  //if ( !IsFileNameValid(x) )
    //ChangeNameForFAT(x);

  //printf ("\n *in2ex Internt navn %s external name %s.\n", n, x); getch ();

  return x;
}


void stamp(f, d)
char *f;                /* name of file to change */
ulg d;                  /* dos-style time to change it to */
/* Set last updated and accessed time of file f to the DOS time d. */
{
  //SetFileTime(f, d);
}

ulg filetime(f, a, n, t)
char *f;                /* name of file to get info on */
ulg *a;                 /* return value: file attributes */
long *n;                /* return value: file size */
iztimes *t;             /* return value: access, modific. and creation times */
/* If file *f does not exist, return 0.  Else, return the file's last
   modified date and time as an MSDOS date and time.  The date and
   time is returned in a long with the date most significant to allow
   unsigned integer comparison of absolute times.  Also, if a is not
   a NULL pointer, store the file attributes there, with the high two
   bytes being the Unix attributes, and the low byte being a mapping
   of that to DOS attributes.  If n is not NULL, store the file size
   there.  If t is not NULL, the file's access, modification and creation
   times are stored there as UNIX time_t values.
   If f is "-", use standard input as the file. If f is a device, return
   a file size of -1 */
{
  struct stat s;        /* results of stat() */
  /* convert FNMAX to malloc - 11/8/04 EG */
  char *name;
  int len = strlen(f);

  if (f == label) {
    if (a != NULL)
      *a = label_mode;
    if (n != NULL)
      *n = -2L; /* convention for a label name */
    if (t != NULL)
      t->atime = t->mtime = t->ctime = label_utim;
    return label_time;
  }
  if ((name = malloc(len + 1)) == NULL) {
    ZIPERR(ZE_MEM, "filetime");
  }
  strcpy(name, f);
  if (name[len - 1] == '/')
    name[len - 1] = '\0';
  /* not all systems allow stat'ing a file with / appended */
  if (strcmp(f, "-") == 0) {
    if (fstat(fileno(stdin), &s) != 0)
      error("fstat(stdin)");
  }
  else if (stat(name, &s) != 0) {
    /* Accept about any file kind including directories
     * (stored with trailing / with -r option)
     */
    free(name);
    return 0;
  }
  free(name);

  if (a != NULL) {
	  *a = s.st_attr; // << 16) | !(s.st_mode & S_IWRITE);
    //*a = ((ulg)s.st_mode << 16) | !(s.st_mode & S_IWRITE);
    //if ((s.st_mode & S_IFMT) == S_IFDIR) {
    //*a |= MSDOS_DIR_ATTR;
    //}
  }
  if (n != NULL)
	  *n = (s.st_mode & S_IFMT) == S_IFREG ? s.st_size : -1L;
	    
  if (t != NULL) {
    t->atime = s.st_atime;
    t->mtime = s.st_mtime;
    t->ctime = t->mtime;   /* best guess, (s.st_ctime: last status change!!) */
  }
  return unix2dostime(&s.st_mtime);
}


ulg filetimeHHO(f, a, n, t)
char *f;                /* name of file to get info on */
ulg *a;                 /* return value: file attributes */
long *n;                /* return value: file size */
iztimes *t;             /* return value: access, modific. and creation times */
/* If file *f does not exist, return 0.  Else, return the file's last
   modified date and time as an MSDOS date and time.  The date and
   time is returned in a long with the date most significant to allow
   unsigned integer comparison of absolute times.  Also, if a is not
   a NULL pointer, store the file attributes there, with the high two
   bytes being the Unix attributes, and the low byte being a mapping
   of that to DOS attributes.  If n is not NULL, store the file size
   there.  If t is not NULL, the file's access, modification and creation
   times are stored there as UNIX time_t values.
   If f is "-", use standard input as the file. If f is a device, return
   a file size of -1 */
{
  struct stat s;        /* results of stat() */
  char *name;
  int len = strlen(f), isstdin = !strcmp(f, "-");

  if (f == label) {
    if (a != NULL)
      *a = label_mode;
    if (n != NULL)
      *n = -2L; /* convention for a label name */
    if (t != NULL)
      t->atime = t->mtime = t->ctime = label_utim;
    return label_time;
  }
  if ((name = malloc(len + 1)) == NULL) {
    ZIPERR(ZE_MEM, "filetimeHHO");
  }
  strcpy(name, f);
  if (name[len - 1] == '/')
    name[len - 1] = '\0';
  /* not all systems allow stat'ing a file with / appended */

  if (isstdin) {
    /* it is common for some PC based compilers to 
       fail with fstat() on devices or pipes */
    if (fstat(fileno(stdin), &s) != 0) {
      s.st_mode = S_IFREG; s.st_size = -1L;
    }
    time(&s.st_ctime);
    s.st_atime = s.st_mtime = s.st_ctime;
  } else if (stat(name, &s) != 0) {
             /* Accept about any file kind including directories
              * (stored with trailing / with -r option)
              */
    free(name);
    return 0;
  }

  if (a != NULL) {
    //*a = ((ulg)s.st_mode << 16) | (isstdin ? 0L : (ulg)GetFileMode(name));
	//*a = (ulg)s.st_mode;
	  *a = s.st_attr;
  }

  printf ("\nDette er en test LINE : 721 \n"); getch();

  if (n != NULL)
    *n = (s.st_mode & S_IFMT) == S_IFREG ? s.st_size : -1L;
#ifdef __WATCOMC__
  /* of course, Watcom always has to make an exception */
  if (s.st_atime == 312764400)
    s.st_atime = s.st_mtime;
  if (s.st_ctime == 312764400)
    s.st_ctime = s.st_mtime;
#endif
  if (t != NULL) {
    t->atime = s.st_atime;
    t->mtime = s.st_mtime;
    t->ctime = s.st_ctime;
  }

  printf ("\nDette er en test LINE : 735 \n"); getch();

  //return GetFileTime(name);
  free(name);
  return t->atime;
}

int deletedir(d)
char *d;                /* directory to delete */
/* Delete the directory *d if it is empty, do nothing otherwise.
   Return the result of rmdir(), delete(), or system().
 */
{
    return rmdir(d);
}

int set_extra_field(z, z_utim)
  struct zlist far *z;
  iztimes *z_utim;
  /* create extra field and change z->att if desired */
{
#ifdef USE_EF_UT_TIME
  if ((z->extra = (char *)malloc(EB_HEADSIZE+EB_UT_LEN(1))) == NULL)
    return ZE_MEM;

  z->extra[0]  = 'U';
  z->extra[1]  = 'T';
  z->extra[2]  = EB_UT_LEN(1);          /* length of data part of e.f. */
  z->extra[3]  = 0;
  z->extra[4]  = EB_UT_FL_MTIME;
  z->extra[5]  = (char)(z_utim->mtime);
  z->extra[6]  = (char)(z_utim->mtime >> 8);
  z->extra[7]  = (char)(z_utim->mtime >> 16);
  z->extra[8]  = (char)(z_utim->mtime >> 24);

  z->cext = z->ext = (EB_HEADSIZE+EB_UT_LEN(1));
  z->cextra = z->extra;

  return ZE_OK;
#else /* !USE_EF_UT_TIME */
  return (int)(z-z);
#endif /* ?USE_EF_UT_TIME */
}


/******************************/
/*  Function version_local()  */
/******************************/

static ZCONST char CompiledWith[] = "Compiled with %s%s for %s%s%s%s.\n\n";
                        /* At module level to keep Turbo C++ 1.0 happy !! */

void version_local()
{
#if defined(__DJGPP__) || defined(__WATCOMC__) || \
    (defined(_MSC_VER) && (_MSC_VER != 800))
    char buf[80];
#endif

    printf(CompiledWith,

#ifdef __GNUC__
#  if defined(__DJGPP__)
      (sprintf(buf, "djgpp v%d / gcc ", __DJGPP__), buf),
#  elif defined(__GO32__)
      "djgpp v1.x / gcc ",
#  elif defined(__EMX__)            /* ...so is __EMX__ (double sigh) */
      "emx+gcc ",
#  else
      "gcc ",
#  endif
      __VERSION__,
#elif defined(__WATCOMC__)
#  if (__WATCOMC__ % 10 > 0)
/* We do this silly test because __WATCOMC__ gives two digits for the  */
/* minor version, but Watcom packaging prefers to show only one digit. */
      (sprintf(buf, "Watcom C/C++ %d.%02d", __WATCOMC__ / 100,
               __WATCOMC__ % 100), buf), "",
#  else
      (sprintf(buf, "Watcom C/C++ %d.%d", __WATCOMC__ / 100,
               (__WATCOMC__ % 100) / 10), buf), "",
#  endif
#elif defined(__TURBOC__)
#  ifdef __BORLANDC__
      "Borland C++",
#    if (__BORLANDC__ < 0x0200)
        " 1.0",
#    elif (__BORLANDC__ == 0x0200)   /* James:  __TURBOC__ = 0x0297 */
        " 2.0",
#    elif (__BORLANDC__ == 0x0400)
        " 3.0",
#    elif (__BORLANDC__ == 0x0410)   /* __BCPLUSPLUS__ = 0x0310 */
        " 3.1",
#    elif (__BORLANDC__ == 0x0452)   /* __BCPLUSPLUS__ = 0x0320 */
        " 4.0 or 4.02",
#    elif (__BORLANDC__ == 0x0460)   /* __BCPLUSPLUS__ = 0x0340 */
        " 4.5",
#    elif (__BORLANDC__ == 0x0500)   /* __TURBOC__ = 0x0500 */
        " 5.0",
#    else
        " later than 5.0",
#    endif
#  else
      "Turbo C",
#    if (__TURBOC__ > 0x0401)
        "++ later than 3.0"
#    elif (__TURBOC__ == 0x0401)     /* Kevin:  3.0 -> 0x0401 */
        "++ 3.0",
#    elif (__TURBOC__ == 0x0295)     /* [661] vfy'd by Kevin */
        "++ 1.0",
#    elif ((__TURBOC__ >= 0x018d) && (__TURBOC__ <= 0x0200)) /* James: 0x0200 */
        " 2.0",
#    elif (__TURBOC__ > 0x0100)
        " 1.5",                    /* James:  0x0105? */
#    else
        " 1.0",                    /* James:  0x0100 */
#    endif
#  endif
#elif defined(MSC)
      "Microsoft C ",
#  ifdef _MSC_VER
#    if (_MSC_VER == 800)
        "(Visual C++ v1.1)",
#    elif (_MSC_VER == 850)
        "(Windows NT v3.5 SDK)",
#    elif (_MSC_VER == 900)
        "(Visual C++ v2.0/v2.1)",
#    elif (_MSC_VER > 900)
        (sprintf(buf2, "(Visual C++ v%d.%d)", _MSC_VER/100 - 6,
          _MSC_VER%100/10), buf2),
#    else
        (sprintf(buf, "%d.%02d", _MSC_VER/100, _MSC_VER%100), buf),
#    endif
#  else
      "5.1 or earlier",
#  endif
#else
      "unknown compiler", "",
#endif

      "MS-DOS",

#if (defined(__GNUC__) || (defined(__WATCOMC__) && defined(__386__)))
      " (32-bit)",
#elif defined(M_I86HM) || defined(__HUGE__)
      " (16-bit, huge)",
#elif defined(M_I86LM) || defined(__LARGE__)
      " (16-bit, large)",
#elif defined(M_I86MM) || defined(__MEDIUM__)
      " (16-bit, medium)",
#elif defined(M_I86CM) || defined(__COMPACT__)
      " (16-bit, compact)",
#elif defined(M_I86SM) || defined(__SMALL__)
      " (16-bit, small)",
#elif defined(M_I86TM) || defined(__TINY__)
      " (16-bit, tiny)",
#else
      " (16-bit)",
#endif

#ifdef __DATE__
      " on ", __DATE__
#else
      "", ""
#endif
    );

} /* end function version_local() */


#ifdef __WATCOMC__

/* This papers over a bug in Watcom 10.6's standard library... sigh */
/* Apparently it applies to both the DOS and Win32 stat()s.         */

int stat_bandaid(const char *path, struct stat *buf)
{
  char newname[4];
  if (!stat(path, buf))
    return 0;
  else if (!strcmp(path, ".") || (path[0] && !strcmp(path + 1, ":."))) {
    strcpy(newname, path);
    newname[strlen(path) - 1] = '\\';   /* stat(".") fails for root! */
    return stat(newname, buf);
  } else
    return -1;
}

#endif


