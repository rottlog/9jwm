#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int r; // check if running

void
emsg(char *m)
{
    fprintf(stderr, "%s: %s\n", WM_NAME, m);
    exit(1);
}

void
quit()
{
	emsg("jellowm exit");
	r = 0;
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
