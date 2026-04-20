#ifndef NET_H
#define NET_H

#include <stddef.h>

struct memory {
    char *buffer;
    size_t size;
};

int net_init();
void net_shutdown();

int net_send_request(char *url, struct memory *mem);

#endif
