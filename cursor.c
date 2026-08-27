#include <xcb/xcb_cursor.h>
#include <xcb/xcb.h>

static const uint8_t arrow[] = {
	0xff, 0xff, 0x80, 0x01, 0x80, 0x03, 0x80, 0x0f, 
	0x80, 0x1f, 0x80, 0x1f, 0x80, 0x0f, 0x80, 0x07, 
	0x80, 0x03, 0x80, 0x01, 0x80, 0x03, 0x8c, 0x07, 
	0x9e, 0x0f, 0x9f, 0x1f, 0xbf, 0xbf, 0xff, 0xff
};

static xcb_cursor_t cursor;

xcb_pixmap_t
curpixmap(xcb_screen_t *s, xcb_connection_t *dpy, int isrc)
{
	
	int i;
	uint8_t inverted[64] = {0}; 	
	for (i = 0; i < 16; i++) {

		uint8_t a = isrc ? 0x00 : arrow[i * 2];
		a = (a & 0xf0) >> 4 | (a & 0x0f) << 4;
		a = (a & 0xcc) >> 2 | (a & 0x33) << 2;
		a = (a & 0xaa) >> 1 | (a & 0x55) << 1;

		uint8_t b = isrc ? 0x00 : arrow[i * 2 + 1];
		b = (b & 0xf0) >> 4 | (b & 0x0f) << 4;
		b = (b & 0xcc) >> 2 | (b & 0x33) << 2;
		b = (b & 0xaa) >> 1 | (b & 0x55) << 1;
	
		inverted[i * 4]     = a;
		inverted[i * 4 + 1] = b;
		inverted[i * 4 + 2] = 0x00;
		inverted[i * 4 + 3] = 0x00;
	}

	xcb_pixmap_t pixmap = xcb_generate_id(dpy);
	xcb_create_pixmap(dpy, 1, pixmap, s->root, 16, 16);
	
	xcb_gcontext_t gc = xcb_generate_id(dpy);
	xcb_create_gc(dpy, gc, pixmap, 0, NULL);
	
	xcb_put_image(dpy, XCB_IMAGE_FORMAT_XY_BITMAP, pixmap, gc, 16, 16, 0, 0, 0, 1, 64, inverted);

	xcb_free_gc(dpy, gc);
	return pixmap;
}

void
loadcursor(xcb_screen_t *s, xcb_connection_t *dpy)
{
	xcb_pixmap_t src_pixmap = curpixmap(s, dpy, 1);
	xcb_pixmap_t mask_pixmap = curpixmap(s, dpy, 0);

	cursor = xcb_generate_id(dpy);
	xcb_create_cursor(dpy, cursor, src_pixmap, mask_pixmap, 0, 0, 0, 0x0000, 0x0000, 0x0000, 0, 0);
	
	uint32_t values[] = {cursor};
    xcb_change_window_attributes(dpy, s->root, XCB_CW_CURSOR, values);
   
    xcb_flush(dpy);

    xcb_free_pixmap(dpy, src_pixmap);
    xcb_free_pixmap(dpy, mask_pixmap);
}
