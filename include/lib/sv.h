#ifndef SV_H
#define SV_H

#include <stddef.h>

struct string_view {
   char *buf;
   size_t len;
};

struct string_view sv_create(char *buf, size_t len);
struct string_view sv_copy(struct string_view *sv);

#define SV(x) sv_create((x), sizeof(x) - 1)

#endif
