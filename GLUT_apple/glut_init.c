/* Copyright (c) Mark J. Kilgard, 1994. */

/* This program is freely distributable without licensing fees
   and is provided without guarantee or warrantee expressed or
   implied. This program is -not- in the public domain. */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "glut.h"
#include "glutint.h"


#include <MacWindows.h>
#include <Dialogs.h>
#include <Fonts.h>
#include <Menus.h>
#include <Menus.h>
#include <TextEdit.h>


#if defined(__MWERKS__) || defined(__SC__)
	#include "console.h"
#endif

#if defined(__MWERKS__)
	#include "SIOUX.h"
#endif

/* GLUT inter-file variables */
char *__glutProgramName = NULL;
int __glutArgc = 0;
char **__glutArgv = NULL;
char *__glutGeometry = NULL;
GLboolean __glutDebug = GL_FALSE;
static GLboolean __glutInited = GL_FALSE;

unsigned int __glutDisplayMode = GLUT_RGB | GLUT_SINGLE | GLUT_DEPTH;
  
int __glutInitWidth = 300;
int __glutInitHeight = 300;
int __glutInitX = 50;
int __glutInitY = 50;

/*static char *strdup(const char *string);*/
static void parseGeometry(char *param, int *x, int *y, int *w, int *h);

static void DoInitManagers(void);
static void DoInitConsole(void);

#if defined(__MWERKS__)
extern OSErr __initialize(const CFragInitBlock *theInitBlock);
extern void __terminate(void);

OSErr __glutInitLibrary(const CFragInitBlock *theInitBlock);
void __glutTerminateLibrary(void);

OSErr __glutInitLibrary(const CFragInitBlock *theInitBlock)
{
	OSErr err;
	
	err = __initialize(theInitBlock);

	return err;
}

void __glutTerminateLibrary(void)
{
	GLint i;
	
	/* Remove all windows */
	for(i = 0; i < __glutWindowListSize; i++)
	{
		if(__glutWindowList[i])
		{
			__glutDestroyWindow(__glutWindowList[i], __glutWindowList[i]);
		}
	}
	
	if(__glutWindowList) free(__glutWindowList);
	if(freeTimerList)    free(freeTimerList);
	
	/* Free command line args */
	for(i = 0; i < __glutArgc; i++)
	{
		free(__glutArgv[i]);
	}
	
	if(__glutArgc > 0) free(__glutArgv);
	
	/* Call default terminate */
	__terminate();
}
#endif

/*static char *strdup(const char *string)
{
	char *new;

	new = malloc(strlen(string) + 1);
	if (new == NULL)
	return NULL;
	strcpy(new, string);
	return new;
}*/

double __glutTime(void)
{
   UnsignedWide tk_time;
   
   Microseconds(&tk_time);
   
   return (4294.967296 * tk_time.hi + 0.000001 * tk_time.lo) * 1000.0;
}

void __glutInitTime(double *beginning)
{
	static int beenhere = 0;
	static double genesis;

	if (!beenhere) {
		genesis = __glutTime();
		beenhere = 1;
	}
	
	*beginning = genesis;
}

static void removeArgs(int *argcp, char **argv, int numToRemove)
{
	int i, j;

	for(i = 0, j = numToRemove; argv[j]; i++, j++) argv[i] = argv[j];

	argv[i] = NULL;
	*argcp -= numToRemove;
}

static void parseGeometry(char *param, int *x, int *y, int *w, int *h)
{
	int i, j;
	char s[21], *sp;
	char st[4] = {'x', '+', '+', '\0'};
	int *pt[4];

	/* Default numbers */
	*x = 20;
	*y = 20;
	*w = 300;
	*h = 300;

	/* Init param pointers */
	pt[0] = x;
	pt[1] = y;
	pt[2] = w;
	pt[3] = h;

	sp = param;

	for(j = 0; j < 4; j++)
	{
		for(i = 0; (i < 20) && (*sp) && (*sp != st[j]); i++)
		{
			s[i] = *sp;
			sp++;
		}
		
		sp++;

		if(i == 20) return;

		s[i] = 0;

		*pt[j] = atoi(s);
	}
}

static void DoInitManagers(void)
{
	MaxApplZone();
	MoreMasters();

	InitGraf(&qd.thePort);
	InitFonts();
	FlushEvents(everyEvent, 0);
	InitWindows();
	InitMenus();
	TEInit();
	InitDialogs(nil);
	InitCursor();
}

static void DoInitConsole(void)
{
	#if defined(__MWERKS__)
		SIOUXSettings.initializeTB       = FALSE;
		SIOUXSettings.standalone         = FALSE;
		SIOUXSettings.setupmenus         = FALSE;
		SIOUXSettings.autocloseonquit    = TRUE;
		SIOUXSettings.asktosaveonclose   = TRUE;
		SIOUXSettings.showstatusline     = TRUE;
		SIOUXSettings.userwindowtitle    = NULL;
		SIOUXSettings.tabspaces          = 4;
		SIOUXSettings.columns            = 60;
		SIOUXSettings.rows               = 8;
		SIOUXSettings.toppixel           = 100;
		SIOUXSettings.leftpixel          = 100;
		SIOUXSettings.fontid             = kFontIDMonaco;
		SIOUXSettings.fontsize           = 9;
		SIOUXSettings.fontface           = normal;
		SIOUXSettings.enabledraganddrop  = FALSE;
		SIOUXSettings.outlinehilite      = FALSE;
		SIOUXSettings.wasteusetempmemory = TRUE;
	#endif
}

void __glutInitApp(void)
{
	double unused;
	
	if(!__glutInited)
	{
		DoInitConsole();
		DoInitManagers();
		__glutBuildMenuBar();
		__glutInitTime(&unused);
		
		__glutInited = GL_TRUE;
	}
}

void glutInitMac(int *argcp, char ***argv)
{
	double unused;
	int i;

	#if defined(__MWERKS__) || defined(__SC__)
		*argcp = ccommand(argv);
	#endif

	/* Make private copy of command line arguments. */
	__glutArgc = *argcp;
	if(__glutArgc > 0)
	{
		__glutArgv = (char **) malloc(__glutArgc * sizeof(char *));
		if(!__glutArgv) __glutFatalError("out of memory.");

		for(i = 0; i < __glutArgc; i++)
		{
			__glutArgv[i] = strdup((*argv)[i]);
			if(!__glutArgv[i]) __glutFatalError("out of memory.");
		}
	}
	else
	{
		__glutArgc = 1;
		__glutArgv = (char **) malloc(__glutArgc * sizeof(char *));
		__glutArgv[0] = strdup("GLUT Program");
	}

	/* determine permanent program name */
	__glutProgramName = __glutArgv[0];

	DoInitConsole();
	DoInitManagers();
	__glutBuildMenuBar();

	/* parse arguments for standard options */
	for(i = 1; i < __glutArgc; i++)
	{
		if(!strcmp(__glutArgv[i], "-geometry"))
		{
			int x, y, width, height;

			if(++i >= __glutArgc)
			{
				__glutFatalError("follow -geometry option with geometry parameter.");
			}
			
			/* Fix bogus "{width|height} may be used before set" warning */
			width  = 0;
			height = 0;

			parseGeometry(__glutArgv[i], &x, &y, &width, &height);

			if(width > 0)  __glutInitWidth = width;
			if(height > 0) __glutInitHeight = height;
			
			glutInitWindowSize(__glutInitWidth, __glutInitHeight);

			if(x >= 0) __glutInitX = x;
			if(y >= 0) __glutInitY = y;

			glutInitWindowPosition(__glutInitX, __glutInitY);

			removeArgs(argcp, &(*argv)[1], 2);
		}
		else if(!strcmp(__glutArgv[i], "-gldebug"))
		{
			__glutDebug = GL_TRUE;
			removeArgs(argcp, &(*argv)[1], 1);
		}
		else
		{
			/* Once unknown option encountered, stop command line processing. */
			break;
		}
	}

	__glutInitTime(&unused);
	
	__glutInited = GL_TRUE;
}

#undef glutInit
void glutInit(int *argcp, char **argv)
{
  glutInitMac(argcp,&argv);
}

void glutInitWindowPosition(int x, int y)
{
	__glutInitX = x;
	__glutInitY = y;
}

void glutInitWindowSize(int width, int height)
{
	__glutInitWidth = width;
	__glutInitHeight = height;
}

void glutInitDisplayMode(unsigned int mask)
{
	__glutDisplayMode = mask;
}
