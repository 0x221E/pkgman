#include <cookbook.h>

#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>

#include <lib/url.h>
#include <err.h>

int cookbook_run(struct url *script_loc, char *script)
{
	printf("Launching build script...\n");

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
			if(execve(script_loc->buffer,
			  (char*[]){ script_loc->buffer, script,NULL},
			  (char*[]){
				  "PWD=/tmp/pkgman/nvim-extract",
				  "PATH=/bin:/usr/bin",
				  NULL }) == -1) {
			fprintf(stderr, "An error was encountered!\n");
		}
		break;
	default: {
		size_t size = 0;
		size_t cap = 40;
		char *mem = malloc(40);
		char buf[1024];
		memset(buf, 0, 1024);

		close(pipefd[1]);
		while (read(pipefd[0], buf, 1) > 0) {
		        if (size >= cap) {
				cap *= 2;
				mem = realloc(mem, cap);
				printf("Realloced!");
			}
					       
			memcpy(mem + size, buf, 1);
			size++;
		}
		
		printf("Build script stdouts: %.*s\n", size, mem);

		free(mem);
		int ret;
		waitpid(ps, &ret, 0);
		printf("Build script exited with code %d\n", ret);
	}
	}
}
