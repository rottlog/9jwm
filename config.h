#ifndef CONFIG_H
#define CONFIG_H

#define WM_NAME "9jwm"
#define WM_VERSION "0.1"

#include "cursor.c"
#include "util.c"

const char* TERM[] = {"xterm", NULL};
const char* RUN_PROG[] = {"dmenu_run", NULL};
const char* BROWSER[] = {"surf", NULL};

#define SUPER_KEY XCB_MOD_MASK_4

#define ROOT_EVENT_MASK (XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY | XCB_EVENT_MASK_STRUCTURE_NOTIFY | XCB_EVENT_MASK_FOCUS_CHANGE)

static struct key keys[] = {
	{ SUPER_KEY,				XK_Return,		spawn,		{.n = TERM}},
	{ SUPER_KEY,				XK_d,			spawn,		{.n = RUN_PROG}},
	{ SUPER_KEY,				XK_b,			spawn,		{.n = BROWSER}},
	{ SUPER_KEY|XCB_MOD_MASK_SHIFT,		XK_e,			quit,		{NULL}}
};


#endif
