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

int sv_equal(struct string_view *a, struct string_view *b)
{
    assert(a != NULL);
    assert(b != NULL);

    if (a->buf == NULL || b->buf == NULL)
        return 0;

    if (a->len != b->len)
        return 0;

    if (memcmp(a->buf, b->buf, a->len) != 0)
        return 0;

    return 1;
}
