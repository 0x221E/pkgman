#ifndef SV_H
#define SV_H

#include <stddef.h>

struct string_view {
   char *buf;
   size_t len;
};

struct string_view sv_create(const char *buf, const size_t len);
int sv_init(struct string_view *sv, const char *buf);
void  sv_free(struct string_view *sv);
struct string_view sv_copy(struct string_view *sv);
int sv_equal(struct string_view *a, struct string_view *b);
struct string_view sv_concat(struct string_view *a, struct string_view *b);
int sv_concat_cstr(struct string_view *sv, const char *cstr);

#define SV(x) (struct string_view){(x), sizeof(x) - 1}

#endif
