/* Copyright (c) Mark J. Kilgard, 1994. */

/* This program is freely distributable without licensing fees
   and is provided without guarantee or warrantee expressed or
   implied. This program is -not- in the public domain. */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <assert.h>

#include "glut.h"
#include "glutint.h"


#include <MacWindows.h>
#include <Menus.h>
#include <Events.h>


typedef struct glut_menu_rec {
	char      label[50];
	int       menuid;
	struct glut_menu_bar_rec *sub_menu;
	struct glut_menu_rec *next_menu;
} glut_menu;

typedef struct glut_menu_bar_rec {
	glut_menu   *menu;
	void        (*select)(int value);
	char        bar_label[50];
	short       menuid;
	MenuHandle  menu_handle;
	short       menu_rsrc_id;
	struct glut_menu_bar_rec *next_menu_bar;
} glut_menu_bar;

static GLint          glut_unique_num  = 1;
static glut_menu_bar  *cur_active_menu = NULL;
static glut_menu_bar  *all_menus       = NULL;
 
static char glut_menu_names[MAX_MENUS][50] = {"LeftButton", "MiddleButton", "RightButton"};

static glut_menu_bar *__glutFindMenu(int menuid);
static glut_menu *__glutFindMenuLastItem(glut_menu_bar *menu);
static glut_menu *__glutFindMenuItem(glut_menu_bar *glut_menu_bar, int menuid);
static glut_menu *__glutFindPrevMenuItem(glut_menu_bar *menu, int menuid);
static glut_menu *__glutFindMenuIndex(glut_menu_bar *menu, int index);
static glut_menu *__glutFindPrevMenuIndex(glut_menu_bar *menu, int index);
static glut_menu *__glutCreateMenuEntry(const char *label, int value);

static glut_menu_bar *__glutFindMenu(int menuid)
{
	glut_menu_bar  *menu;
	
	menu = all_menus;
	while(menu)
	{
		if(menu->menuid == menuid) break;
		menu = menu->next_menu_bar;
	}
	
	if(!menu) return NULL;
	
	return menu;
}

static glut_menu *__glutFindMenuLastItem(glut_menu_bar *menu_bar)
{
	glut_menu  *last_menu;
	
	if(!menu_bar) return NULL;
	
	last_menu = menu_bar->menu;
	while(last_menu && last_menu->next_menu)
		last_menu = last_menu->next_menu;
	
	return last_menu;
}

static glut_menu *__glutFindMenuItem(glut_menu_bar *menu_bar, int menuid)
{
	glut_menu  *menu_item;
	
	if(!menu_bar) return NULL;
	
	menu_item = menu_bar->menu;
	while(menu_item)
	{
		if(menu_item->menuid == menuid) break;
		menu_item = menu_item->next_menu;
	}
	
	return menu_item;
}

static glut_menu *__glutFindPrevMenuItem(glut_menu_bar *menu_bar, int menuid)
{
	glut_menu  *menu_item;
	glut_menu  *prev_menu_item;
	
	if(!menu_bar) return NULL;
	
	prev_menu_item = NULL;
	menu_item = menu_bar->menu;
	while(menu_item)
	{
		if(menu_item->menuid == menuid) break;		
		prev_menu_item = menu_item;
		menu_item = menu_item->next_menu;
	}
	
	return prev_menu_item;
}

static glut_menu *__glutFindMenuIndex(glut_menu_bar *menu_bar, int index)
{
	glut_menu  *menu_item;
	GLint i = 1;
	
	if(!menu_bar) return NULL;
	
	menu_item = menu_bar->menu;
	while(menu_item)
	{
		if(index == i) break;
		i++;
		menu_item = menu_item->next_menu;
	}
	
	return menu_item;
}

static glut_menu *__glutFindPrevMenuIndex(glut_menu_bar *menu_bar, int index)
{
	glut_menu  *menu_item;
	glut_menu  *prev_menu_item;
	GLint i = 1;
	
	if(!menu_bar) return NULL;
	
	prev_menu_item = NULL;
	menu_item = menu_bar->menu;
	while(menu_item)
	{
		if(index == i) break;
		i++;
		prev_menu_item = menu_item;
		menu_item = menu_item->next_menu;
	}
	
	return prev_menu_item;
}

static glut_menu *__glutCreateMenuEntry(const char *label, int value)
{
	glut_menu  *menu = (glut_menu *) malloc(sizeof(glut_menu));
	if(!menu) return NULL;
	
	strcpy(menu->label, label);
	menu->menuid    = value;
	menu->sub_menu  = NULL;
	menu->next_menu = NULL;
	
	return menu;
}

void __glutBuildMenuBar(void)
{
	Handle menuBar;
	
	menuBar = GetNewMBar(rMenuBar);
	if(!menuBar) return;
	SetMenuBar(menuBar);
	DisposeHandle(menuBar);
	
	AppendResMenu(GetMenuHandle(mApple),'DRVR');
	DrawMenuBar();
}

static void __glutRemoveMenuFromBar(int menunum)
{
	glut_menu_bar  *menu_bar;
	
	menu_bar = all_menus;
	while(menu_bar)
	{
		if(menunum == menu_bar->menuid) break;
		menu_bar = menu_bar->next_menu_bar;
	}
	
	if(!menu_bar) return;
	
	if(menu_bar->menu_handle)
	{
		DeleteMenu(menu_bar->menu_rsrc_id);
		DisposeMenu(menu_bar->menu_handle);
		
		menu_bar->menu_handle = NULL;
		menu_bar->menu_rsrc_id = 0;
	}
}

static void __glutRemoveAllMenusFromBar(void)
{
	glut_menu_bar  *menu_bar;
	
	menu_bar = all_menus;
	while(menu_bar)
	{
		if(menu_bar->menu_handle)
		{
			DeleteMenu(menu_bar->menu_rsrc_id);
			DisposeMenu(menu_bar->menu_handle);
			
			menu_bar->menu_handle = NULL;
			menu_bar->menu_rsrc_id = 0;
		}
		
		menu_bar = menu_bar->next_menu_bar;
	}
}

static void __glutFreeMenuMem(int menunum)
{
	glut_menu_bar *menu_bar;
	glut_menu_bar *prev_menu_bar;
	glut_menu *menu;
	glut_menu *next_menu;
	
	prev_menu_bar = NULL;
	menu_bar = all_menus;
	while(menu_bar)
	{
		if(menu_bar->menuid == menunum) break;
		prev_menu_bar = menu_bar;
		menu_bar = menu_bar->next_menu_bar;
	}
	
	if(!menu_bar) return;
	
	if(prev_menu_bar)
	{
		prev_menu_bar->next_menu_bar = menu_bar->next_menu_bar;
	}
	else
	{
		all_menus = menu_bar->next_menu_bar;
	}
	
	menu = menu_bar->menu;
	while(menu)
	{
		next_menu = menu->next_menu;
		free(menu);
		menu = next_menu;
	}
	
	if(menu_bar == cur_active_menu) cur_active_menu = NULL;
	free(menu_bar);
}

static glut_menu_bar *__glutGetMouseMenu(GLint mouse_id)
{
	glut_menu_bar *menu_bar;
	int menu_id;
	
	if(!__glutCurrentWindow) return NULL;
	
	menu_id = __glutCurrentWindow->menus[mouse_id];
	
	if(menu_id == 0) return NULL;
	
	menu_bar = all_menus;
	while(menu_bar)
	{
		if(menu_id == menu_bar->menuid) return menu_bar;
		menu_bar = menu_bar->next_menu_bar;
	}
	
	return NULL;
}

void __glutRebuildMenuBar(void)
{
	glut_menu_bar  *menu_bar;
	glut_menu      *menu_item, *sub_menu_item, *sub_sub_menu_item;
	short          menu_id, mouse_id, item_id, sub_item_id;
	Str255         pstr;
	
	if(!__glutCurrentWindow->fullscreen)
	{
		if(__glutCurrentWindow != __glutGetWindow((CGrafPtr) FrontWindow())) return;
	}
	
	__glutRemoveAllMenusFromBar();
	
	if(!__glutCurrentWindow) return;
	
	menu_id = mMouseMenuStart;
	for(mouse_id = 0; mouse_id < MAX_MENUS; mouse_id++)
	{
		menu_bar = __glutGetMouseMenu(mouse_id);
		if(menu_bar)
		{
			__glutCToPStr(menu_bar->bar_label, pstr);
			menu_bar->menu_handle = NewMenu(menu_id, pstr);
			
			if(menu_bar->menu_handle)
			{
				InsertMenu(menu_bar->menu_handle, 0);
				menu_bar->menu_rsrc_id = menu_id++;
				
				item_id = 1;
				menu_item = menu_bar->menu;
				while(menu_item)
				{
					__glutCToPStr(menu_item->label, pstr);
					AppendMenu(menu_bar->menu_handle, pstr);
						
					if(menu_item->sub_menu)
					{
						__glutCToPStr(menu_item->label, pstr);
						menu_item->sub_menu->menu_handle = NewMenu(menu_id, pstr);
						
						if(menu_item->sub_menu->menu_handle)
						{
							menu_item->sub_menu->menu_rsrc_id = menu_id++;
							
							sub_item_id = 1;
							sub_menu_item = menu_item->sub_menu->menu;
							while(sub_menu_item)
							{
								__glutCToPStr(sub_menu_item->label, pstr);
								AppendMenu(menu_item->sub_menu->menu_handle, pstr);
								
								if(sub_menu_item->sub_menu)
								{
									__glutCToPStr(sub_menu_item->label, pstr);
									sub_menu_item->sub_menu->menu_handle = NewMenu(menu_id, pstr);
									
									if(sub_menu_item->sub_menu->menu_handle)
									{
										sub_menu_item->sub_menu->menu_rsrc_id = menu_id++;
										
										sub_sub_menu_item = sub_menu_item->sub_menu->menu;
										while(sub_sub_menu_item)
										{
											__glutCToPStr(sub_sub_menu_item->label, pstr);
											AppendMenu(sub_menu_item->sub_menu->menu_handle, pstr);
											
											sub_sub_menu_item = sub_sub_menu_item->next_menu;
										}
										
										SetItemCmd(menu_item->sub_menu->menu_handle, sub_item_id, hMenuCmd);
										SetItemMark(menu_item->sub_menu->menu_handle, sub_item_id, sub_menu_item->sub_menu->menu_rsrc_id);
							
										InsertMenu(sub_menu_item->sub_menu->menu_handle, hierMenu);
									}
								}
								
								sub_item_id++;
								sub_menu_item = sub_menu_item->next_menu;
							}
							
							SetItemCmd(menu_bar->menu_handle, item_id, hMenuCmd);
							SetItemMark(menu_bar->menu_handle, item_id, menu_item->sub_menu->menu_rsrc_id);
							
							InsertMenu(menu_item->sub_menu->menu_handle, hierMenu);
						}
					}
					
					item_id++;
					menu_item = menu_item->next_menu;
				}
			}
		}
	}
	
	DrawMenuBar();
}

void __glutMenuKeyCommand(void)
{
	
}

void __glutMenuCommand(short menuNum, short itemNum)
{
	glut_menu_bar  *menu_bar;
	glut_menu      *menu_item;
	short          menu_item_id;
	glut_menu_bar  *old_active_menu;
	
	menu_bar = all_menus;
	while(menu_bar)
	{
		if(menu_bar->menu_rsrc_id == menuNum)
		{
			menu_item = menu_bar->menu;
			menu_item_id = 1;
			while(menu_item)
			{
				if(menu_item_id == itemNum)
				{
					old_active_menu = cur_active_menu;
					cur_active_menu = menu_bar;
					
					if(menu_bar->select) menu_bar->select(menu_item->menuid);
					
					cur_active_menu = old_active_menu;
					
					return;
				}
				
				menu_item = menu_item->next_menu;
				menu_item_id++;
			}
		}
		
		menu_bar = menu_bar->next_menu_bar;
	}
}

int __glutGetCurNumMenuItems(void)
{
	glut_menu *menu_item;
	int count = 0;

	if(!cur_active_menu) return 0;
	
	menu_item = cur_active_menu->menu;
	while(menu_item)
	{
		count++;
		menu_item = menu_item->next_menu;
	}
	
	return count;
}

void glutMenuStateFunc(GLUTmenuStateCB menuStateFunc)
{
	__glutCurrentWindow->menu_state = menuStateFunc;
}

void glutMenuStatusFunc(GLUTmenuStatusCB menuStatusFunc)
{
	__glutCurrentWindow->menu_status = menuStatusFunc;
}

int glutCreateMenu(void (*select)(int value))
{
	glut_menu_bar  *menu_bar;
	
	/* Update menu bar list */
	menu_bar = (glut_menu_bar *) malloc(sizeof(glut_menu_bar));
	if(!menu_bar) return 0;
	
	menu_bar->menu          = NULL;
	menu_bar->menuid        = glut_unique_num++;
	menu_bar->menu_handle   = NULL;
	menu_bar->menu_rsrc_id  = 0;
	menu_bar->bar_label[0]  = 0;
	menu_bar->select        = select;
	
	menu_bar->next_menu_bar = all_menus;
 	all_menus = menu_bar;
	
	/* Set current menu */
	cur_active_menu = menu_bar;
	
	return menu_bar->menuid;
}

void glutDestroyMenu(int menunum)
{
	__glutRemoveMenuFromBar(menunum);
	__glutFreeMenuMem(menunum);
	
	DrawMenuBar();
}

int glutGetMenu(void)
{
	if(cur_active_menu)
		return cur_active_menu->menuid;
	else
		return 0;
}

void glutSetMenu(int menuid)
{
	cur_active_menu = __glutFindMenu(menuid);
}

void glutAddMenuEntry(const char *label, int value)
{
	glut_menu  *last_menu;
	glut_menu  *new_menu;
	
	if(!cur_active_menu) return;
	
	/* Add it to the bottom of current menu */
	last_menu = __glutFindMenuLastItem(cur_active_menu);
	
	/* Create menu entry */
	new_menu = __glutCreateMenuEntry(label, value);
	if(!new_menu) return;
	
	/* Add menu entry to last menu item */
	if(last_menu) last_menu->next_menu = new_menu;
	else cur_active_menu->menu = new_menu;
	
	/* Rebuild menu bar */
	__glutRebuildMenuBar();
}

void glutAddSubMenu(const char *label, int menu)
{
	glut_menu_bar  *sub_menu;
	glut_menu  *new_menu;
	glut_menu  *last_menu;
	
	if(!cur_active_menu) return;
	
	/* Find sub menu */
	sub_menu = __glutFindMenu(menu);
	if(!sub_menu) return;
	
	/* Add it to the bottom of current menu */
	last_menu = __glutFindMenuLastItem(cur_active_menu);
	
	/* Create sub menu entry */
	new_menu = __glutCreateMenuEntry(label, 0);
	if(!new_menu) return;
	
	/* Add menu entry to current menu */
	if(last_menu) last_menu->next_menu = new_menu;
	else cur_active_menu->menu = new_menu;
	
	/* Attach sub menu to menu entry */
	new_menu->sub_menu   = sub_menu;
	
	/* Rebuild menu bar */
	__glutRebuildMenuBar();
}

void glutChangeToMenuEntry(int num, const char *label, int value)
{
	glut_menu  *menu;
	
	if(!cur_active_menu) return;
	
	/* Find menu at index num */
	menu = __glutFindMenuIndex(cur_active_menu, num);
	if(!menu) return;
	
	/* Set menu index to menu value */
	strcpy(menu->label, label);
	menu->menuid   = value;
	
	/* Rebuild menu bar */
	__glutRebuildMenuBar();
}

void glutChangeToSubMenu(int num, const char *label, int menu)
{
	glut_menu  *index_menu;
	glut_menu_bar  *sub_menu;
	
	if(!cur_active_menu) return;
	
	/* Find menu at index num */
	index_menu = __glutFindMenuIndex(cur_active_menu, num);
	if(!index_menu) return;
	
	/* Find sub menu */
	sub_menu = __glutFindMenu(menu);
	if(!sub_menu) return;
	
	/* Set menu index to sub menu values */
	strcpy(index_menu->label, label);
	index_menu->menuid = 0;
	index_menu->sub_menu = sub_menu;
	
	/* Rebuild menu bar */
	__glutRebuildMenuBar();
}

void glutRemoveMenuItem(int num)
{
	glut_menu *menu;
	glut_menu *prev_menu;
	
	if(!cur_active_menu) return;
	
	menu = __glutFindMenuIndex(cur_active_menu, num);
	if(!menu) return;
	
	prev_menu = __glutFindPrevMenuIndex(cur_active_menu, num);
	
	if(prev_menu)
		prev_menu->next_menu = menu->next_menu;
	else
		cur_active_menu->menu = menu->next_menu;
		
	free(menu);
	
	/* Rebuild menu bar */
	__glutRebuildMenuBar();
}

void glutAttachMenu(int button)
{
	if(!__glutCurrentWindow)
	{
		__glutWarning("glutAttachMenu: no active window");
		return;
	}
	
	if(!cur_active_menu) return;
	
	/* Set menu label to button name */
	__glutCurrentWindow->menus[button] = cur_active_menu->menuid;
	
	strcpy(cur_active_menu->bar_label, glut_menu_names[button]);
	
	/* Rebuild menu bar */
	__glutRebuildMenuBar();
}

void glutAttachMenuName(int button, char *menu_name)
{
	if(!__glutCurrentWindow)
	{
		__glutWarning("glutAttachMenu: no active window");
		return;
	}
	
	if(!cur_active_menu) return;
	
	/* Set menu label to button name */
	__glutCurrentWindow->menus[button] = cur_active_menu->menuid;
	
	strcpy(cur_active_menu->bar_label, menu_name);
	
	/* Rebuild menu bar */
	__glutRebuildMenuBar();
}

void glutDetachMenu(int button)
{
	if(!__glutCurrentWindow)
	{
		__glutWarning("glutDetachMenu: no active window");
		return;
	}
	
	__glutCurrentWindow->menus[button] = 0;
	
	/* Rebuild menu bar */
	__glutRebuildMenuBar();
}
