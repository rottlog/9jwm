#define arrlength(A)	(sizeof(A)/sizeof(*A))

int r;

void emsg(char *m);
void quit();
void spawn(const Arg arg);