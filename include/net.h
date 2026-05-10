#ifndef NET_H
#define NET_H

#include <stddef.h>
#include <stdio.h>

#define WRITE_OPT_MEMORY    1
#define WRITE_OPT_FILE      2

typedef struct string_view string_view;

struct net_write_data {
    char *buffer;
    size_t size;
};

struct net_file_write_data {
    FILE *file;
};

int net_init();
void net_shutdown();

int net_send_request(char *url, int write_opts, void *userdata);

#endif
