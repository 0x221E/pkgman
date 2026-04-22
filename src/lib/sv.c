#include <lib/sv.h>

#include <stdlib.h>
#include <string.h>
#include <assert.h>

struct string_view sv_create(char *buf, size_t len)
{
    return (struct string_view){.buf = buf, .len = len}; 
}

/**
 * @warning User's responsibility to cleanup memory.
 */
struct string_view sv_copy(struct string_view *sv)
{
    assert(sv != NULL);
    struct string_view copy = { 0 };
    copy.buf = malloc(sv->len);
    copy.len = sv->len;
    memcpy(copy.buf, sv->buf, sv->len);
    return copy;
}
