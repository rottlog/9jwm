#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

#include "util.h"

void
emsg(char *m)
{
    fprintf(stderr, "9jwm: %s\n", m);
    exit(1);
}

void
quit()
{
	emsg("9jwm exit");
}

void
spawn(const Arg arg)
{
	pid_t pid = fork();

	if (pid == 0) {
		setsid();
		execvp((char *)arg.n[0], (char **)arg.n);
		emsg("exec fail");
	}
}
