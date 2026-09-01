#define SUPER_KEY XCB_MOD_MASK_4

static const char* TERM[] = {"xterm", NULL};
static const char* RUN_PROG[] = {"dmenu_run", NULL};
static const char* BROWSER[] = {"surf", NULL};

static struct key keys[] = {
	{ SUPER_KEY,				XK_Return,		spawn,		{.n = TERM}},
	{ SUPER_KEY,				XK_d,			spawn,		{.n = RUN_PROG}},
	{ SUPER_KEY,				XK_b,			spawn,		{.n = BROWSER}},
	{ SUPER_KEY|XCB_MOD_MASK_SHIFT,		XK_e,			quit,		{NULL}}
};
