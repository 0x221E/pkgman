#include <cookbook.h>

#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>

#include <lib/sv.h>
#include <lib/url.h>
#include <err.h>

int cookbook_recipe_run(struct url *script_loc,
			char *script,
			struct string_view *out)
{
	printf("Launching cookbook recipe...\n");

	int pipefd[2];
	
	if (pipe(pipefd) == -1)
		return -FORKERR;
	
	pid_t ps = fork();

	switch (ps) {
	case -1:
		perror("fork");
		return -FORKERR;
	case 0:
		close(pipefd[0]);
		dup2(pipefd[1], STDOUT_FILENO);
		close(pipefd[1]);
		
		chdir(script_loc->buffer);
		url_append(script_loc, "build.sh");
			if(execve(
				  script_loc->buffer,
				  (char*[]){ script_loc->buffer, script, NULL},
				  (char*[]){"PATH=/bin:/usr/bin", NULL }) == -1)
				fprintf(stderr, "An error was encountered!\n");
		break;
	default: {
		char buf[2];
		buf[1] = '\0';
		close(pipefd[1]);
		
		while (read(pipefd[0], buf, 1) > 0)
			sv_concat_cstr(out, buf);
		
		int ret;
		waitpid(ps, &ret, 0);
		printf("Cookbook process exited with code %d\n", ret);
		return ret;
	}
	}
}
