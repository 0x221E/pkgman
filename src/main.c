#include <stdio.h>

#include <string.h>
#include <stdlib.h>
#include <zstd.h>
#include <stdio.h>
#include <sodium.h>

#include <pkgman.h>
#include <um.h>
#include <err.h>
#include <parser.h>
#include <lib/sv.h>
#include <net.h>
#include <lib/url.h>

/**
 *  Install package:
 *      pkgman install <package_name>
 *
 *  Remove package:
 *      pkgman remove <package_name>
 *
 *  Update:
 *      pkgman update
 *      pkgman update <package_name>
 *
 *  Kernel Update:
 *      pkgman kupdate
 *
 *  Fetch:
 *      pkgman fetch <query>
 *
 *  Pack:
 *      pkgman pack <directory>
 *
 *  System:
 *      pkgman system install
 */

void install_usage()
{
    printf("-------------------------------\n");
    printf("Package Manager Install Command\n");
    printf("command:\n");
    printf("    pkgman install <pkg1> ... <pkgn>\n");
}

int cmd_install(int argc, char **argv)
{
	if (argc < 1) {
		install_usage();
		return USAGE;
	}
	
	char *pkg = argv[0];
	
	int ret = pkgman_upstream_check(pkg);
	
	if (ret != SUCCESS)
		return -PKGNOTFND;
	
        ret = pkgman_upstream_integrity_download(pkg);

	if (ret != SUCCESS)
		return ret;

	ret = pkgman_install_pkg(pkg);

	if (ret != SUCCESS)
		return ret;

	return SUCCESS;
}

int cmd_build(int argc, char** argv)
{
	//  ZSTD_compress("test", 60, "aaa.pkg", 30, 3);
	printf("Produced a tar file!");
	return SUCCESS; 
}

typedef int (*cmd_fn)(int, char**);

struct cmd_entry
{
    struct string_view key;
    cmd_fn func;
};

// First-level command table
struct cmd_entry table[] = {
    { SV("build"), cmd_build },
    { SV("install"), cmd_install },
    { SV(NULL), NULL },
};

#define ARRAY_SIZE(x) sizeof((x)) / sizeof((x)[0])

void usage()
{
    printf("-------------------------------\n");
    printf("pkgman v0.0.1\n");
    printf("-------------------------------\n");
    printf("Available commands:\n"); 
    printf("    pkgman install <pkg>\n"); 
    printf("    pkgman build\n"); 
    printf("    pkgman update <pkg?>\n"); 
}

int main(int argc, char **argv) 
{
    if (argc < 2) { 
        usage();
        return 0;
    }

    cmd_fn cmd_func = NULL;
    
    struct string_view argv1 = (struct string_view)
                {.buf = argv[1], .len = strlen(argv[1])};

    for (int i = 0; i < ARRAY_SIZE(table); i++) {
        if (table[i].key.buf == NULL)
            break;

        if (sv_equal(&argv1, &table[i].key))
            cmd_func = table[i].func;
    }
    
    if (cmd_func == NULL) {
        usage();
        return 1;
    }
    
    if (net_init() != 0) {
        fprintf(stderr, "curl: network initialization error!");
        return 1;
    }

    int ret = 1;

    if(argv[2] == NULL)
        ret = cmd_func(0, NULL);
    else
        ret = cmd_func(argc - 2, &argv[2]);

    if(ret != 0)
        printf("Command '%s' ended with error '%d' and/or usage screen.\n",
                argv[1], ret);

    net_shutdown();
    return 0;
}
