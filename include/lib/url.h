#ifndef URL_H
#define URL_H

#include <stddef.h>

struct url {
	char *buffer;
    size_t len;
};

int url_init(struct url *url, char *base);
void url_free(struct url *url);
int url_copy(struct url *from, struct url *to);
int url_append(struct url *url, const char *add);
int url_append_path(struct url *url, const char *add);

#endif
