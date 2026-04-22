#include <stdio.h>

#include <string.h>

#include <um.h>
#include <parser.h>
#include <lib/sv.h>
#include <net.h>

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

void cmd_install(int argv, char **cmd)
{
    if(argv < 1) {
        install_usage();
        return;
    }

    struct memory mem = { 0 };
    net_send_request("https://packages.0xinfinity.dev/list", &mem);

    struct um_user_data userdata = { 0 };
    struct parser parser;
    struct parser_backend backend = um_backend();

    parser_init(&parser, &mem, &backend, (void*)&userdata);
    parser_parse(&parser);

    int found = 0;

    LL_FOREACH(manifest, &userdata.manifest) {
        if (!current->data.key.buf)
            continue;

        if (strncmp(cmd[0],
                    current->data.key.buf,
                    current->data.key.len) == 0) {
            found = 1;
        }
    }

    if (found)
        printf("Package '%s' found on upstream!\n", cmd[0]);
    else
        printf("Package '%s' not found!\n", cmd[0]);
}

typedef void (*cmd_fn)(int, char**);

struct cmd_entry
{
    const char *key;
    cmd_fn func;
};

// First-level command table
struct cmd_entry table[] = {
    {"install", cmd_install},
    {NULL, NULL},
};

#define ARRAY_SIZE(x) sizeof((x)) / sizeof((x)[0])

void usage()
{
    printf("-------------------------------\n");
    printf("pkgman v0.0.1\n");
    printf("-------------------------------\n");
    printf("Available commands:\n"); 
    printf("    pkgman install <pkg>\n"); 
    printf("    pkgman update <pkg?>\n"); 
}

int main(int argc, char **argv) 
{
    if (argc < 2) { 
        usage();
        return 0;
    }

    cmd_fn cmd_func = NULL;
    
    for (int i = 0; i < ARRAY_SIZE(table); i++) {
        if (table[i].key == NULL)
            break;
        if (strcmp(argv[1], table[i].key) == 0)
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

    if(argv[2] == NULL)
        cmd_func(0, NULL);
    else
        cmd_func(argc - 2, &argv[2]);

    net_shutdown();
    return 0;
}
