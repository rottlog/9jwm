#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include <X11/keysym.h>
#include <xcb/xcb_keysyms.h>
#include <xcb/xcb.h>

#include "util.h"
#include "cursor.h"

#define WM_NAME "9jwm"
#define WM_VERSION "0.1"

#define ROOT_EVENT_MASK (XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY | XCB_EVENT_MASK_STRUCTURE_NOTIFY | XCB_EVENT_MASK_FOCUS_CHANGE)

struct key {
    unsigned int mod;
    xcb_keysym_t keysym;
    void (*function)(const Arg arg);
    const Arg arg;
};

static void usage();
static void setwallpaper(char *w);
static void checkforanotherwm();

static void keypress(xcb_generic_event_t *ev);
static void buttonpress(xcb_generic_event_t *ev);
static void motionnotify();
static void setupkeys();

static void run();

#include "config.h"

static int wl, s_height, s_width;

static xcb_key_symbols_t *syms;
static uint32_t values[4];

static int r = 1;

static xcb_cursor_t cur;
static xcb_connection_t *dpy;
static xcb_screen_t *s;
static xcb_drawable_t win;

void
usage()
{
    fprintf(stderr, "usage: %s [-h] [-v] [-w wallpaper]\n", WM_NAME);
    exit(1);
}

void
setwallpaper(char *w)
{
   size_t strl = strlen(w) + 24;
   char *str = malloc(strl);
	
   snprintf(str, strl, "feh --bg-scale %s &", w);
   system(str);

   free(str);
}

void
checkforanotherwm()
{
    xcb_generic_error_t *error = xcb_request_check(dpy, xcb_change_window_attributes_checked(dpy, s->root, XCB_CW_EVENT_MASK, (uint32_t[]){ROOT_EVENT_MASK}));
    if (error != NULL) emsg("theres already another window manager running");
}

void
keypress(xcb_generic_event_t *ev)
{
	size_t i;

	xcb_key_press_event_t *e = ( xcb_key_press_event_t *) ev;

	xcb_keysym_t keysym = xcb_key_symbols_get_keysym(syms, e->detail, 0);

	for (i = 0; i < arrlength(keys); ++i) {
		    if (keys[i].keysym == keysym && keys[i].mod == e->state) {
			keys[i].function(keys[i].arg);
		}
	}

	xcb_flush(dpy);
}

void
buttonpress(xcb_generic_event_t *ev)
{
	xcb_drawable_t root;

    	xcb_get_geometry_reply_t *g;  
	xcb_button_press_event_t *e = ( xcb_button_press_event_t *) ev;

	root = s->root;

	if (e->child == 0 || e->child == root) {
		return;
	}

	win = e->child; 
	       
	values[0] = XCB_STACK_MODE_ABOVE;
  	xcb_configure_window(dpy, win, XCB_CONFIG_WINDOW_STACK_MODE, values);	    

	xcb_set_input_focus(dpy, XCB_INPUT_FOCUS_POINTER_ROOT, win, XCB_CURRENT_TIME);
   	g = xcb_get_geometry_reply(dpy, xcb_get_geometry(dpy, win), NULL);
	switch(e->detail) {
		case 1:
	       values[2] = 1; 
	       xcb_warp_pointer(dpy, XCB_NONE, win, 0, 0, 0, 0, g->width / 2, g->height / 2);
		   break;
		case 2:
		   values[2] = 2;
		   xcb_destroy_window(dpy, win);
		   break;
		case 3:
	       values[2] = 3; 
	       xcb_warp_pointer(dpy, XCB_NONE, win, 0, 0, 0, 0, g->width, g->height);
	       break;
	}
	
	xcb_grab_pointer(dpy, 0, root, XCB_EVENT_MASK_BUTTON_RELEASE
	               | XCB_EVENT_MASK_BUTTON_MOTION | XCB_EVENT_MASK_POINTER_MOTION_HINT, 
	               XCB_GRAB_MODE_ASYNC, XCB_GRAB_MODE_ASYNC, root, cur, XCB_CURRENT_TIME);
	xcb_flush(dpy);
	free(g);
}

void
motionnotify()
{
	xcb_drawable_t root;
	xcb_get_geometry_reply_t *g;  

	root = s->root;

	xcb_query_pointer_reply_t *p;
	p = xcb_query_pointer_reply(dpy, xcb_query_pointer(dpy, root), 0);
	switch(values[2]) {
	    case 1: 
	    	g = xcb_get_geometry_reply(dpy, xcb_get_geometry(dpy, win), NULL);
	    	values[0] = p->root_x - g->width / 2;
	    	values[1] = p->root_y - g->height / 2;
	    	xcb_configure_window(dpy, win, XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y, values);
		xcb_flush(dpy);
		free(g);
		break;
	    case 3: 
	  	g = xcb_get_geometry_reply(dpy, xcb_get_geometry(dpy, win), NULL);
		values[0] = p->root_x - g->x;
	    	values[1] = p->root_y - g->y;
	    	xcb_configure_window(dpy, win, XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT, values);
	    	xcb_flush(dpy);
		free(g);
		break;
	}
}

void
setupkeys()
{
    size_t i;
    xcb_drawable_t root;
    
    syms = xcb_key_symbols_alloc(dpy); 
    xcb_keycode_t *keycode;

    root = s->root;

    for (i = 0; i < arrlength(keys); ++i) {
		if ((keycode = xcb_key_symbols_get_keycode(syms, keys[i].keysym))) {
			xcb_grab_key(dpy, 1, root, keys[i].mod, keycode[0], XCB_GRAB_MODE_ASYNC, XCB_GRAB_MODE_ASYNC);
			free(keycode);
		}
    }
   
    for (i = 1; i < 4; ++i) {
	    xcb_grab_button(dpy, 0, root, XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE,
			    XCB_GRAB_MODE_ASYNC, XCB_GRAB_MODE_ASYNC, root, XCB_NONE, i, SUPER_KEY);
    }
}


void
run()
{
	xcb_generic_event_t *ev;

    while (r) {
	ev = xcb_wait_for_event(dpy);	
    switch (ev->response_type & ~0x80) {

		case XCB_KEY_PRESS:
		    keypress(ev);
			break;

		case XCB_BUTTON_PRESS:
  	     	buttonpress(ev);
			break;

 		case XCB_MOTION_NOTIFY:
    		motionnotify();       
    		break;

    	case XCB_BUTTON_RELEASE:
           	xcb_ungrab_pointer(dpy, XCB_CURRENT_TIME);
     	   	xcb_flush(dpy);
   		    break;	

	}
  }
}

int 
main (int argc, char **argv)
{

    int opt;
    char *w;

    while ((opt = getopt(argc, argv, "w:vh")) != -1) {
	switch(opt) {
		case 'v':
			emsg(WM_VERSION);
			break;
		case 'h':
			usage();
			break;
		case 'w':
			wl = 1;
			w = optarg;
			break;
		default:
			usage();
			break;
	}
    }
    
    dpy = xcb_connect(NULL, NULL);
    if (xcb_connection_has_error(dpy)) emsg("cannot open display");

    s = xcb_setup_roots_iterator(xcb_get_setup(dpy)).data;
    if (s == NULL) emsg("cannot find screen");
    
    checkforanotherwm();

    s_height = s->height_in_pixels;
    s_width = s->width_in_pixels;

    if (wl != 0) {
	setwallpaper(w);	
    } else {
    	system("xsetroot -solid grey30");
    }

    loadcursor(s, dpy, cur);
    setupkeys(); 

    xcb_flush(dpy);
	
    run();

    xcb_free_cursor(dpy, cur);
    xcb_disconnect(dpy);
    return 0;
}
