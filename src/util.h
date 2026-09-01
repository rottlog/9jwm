#define arrlength(A)	(sizeof(A)/sizeof(*A))

typedef union {
    const char** n;
    const int i;
} Arg;

void emsg(char *m);
void quit();
void spawn(const Arg arg);
