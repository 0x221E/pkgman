#include <lib/sv.h>

#include <stdlib.h>
#include <string.h>
#include <assert.h>

struct string_view sv_create(const char *buf, const size_t len)
{
	return (struct string_view){.buf = buf, .len = len}; 
}

int sv_init(struct string_view *sv, const char *b)
{
	assert(sv != NULL);

	size_t len = strlen(b);
	void *ptr = malloc(len);

	memcpy(ptr, b, len);

	sv->buf = ptr;
	sv->len = len;
	
	return 0;	       
}

void sv_free(struct string_view *sv)
{
	assert(sv != NULL);       
	free(sv->buf);
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

/**
 *  @warning User's responsibility to cleanup memory.
 */
struct string_view sv_concat(struct string_view *a, struct string_view *b)
{
    assert(a != NULL);
    assert(b != NULL);
    size_t nlen = a->len + b->len;
    char *ptr = malloc(nlen);
    return (struct string_view){.buf = ptr, .len = nlen};
}

int sv_concat_cstr(struct string_view *a, const char *cstr)
{
	assert(a != NULL);

	if (a->len > 0 && a->buf[a->len - 1] == '\0')
		a->len--;

	size_t lenb = strlen(cstr);
	size_t nlen = lenb + a->len + 1;

        char *tmp = realloc(a->buf, nlen);

	if (tmp == NULL)
		return -1;	

	a->buf = tmp;
	
	memcpy(a->buf + a->len, cstr, lenb);
	a->len = nlen - 1;
	a->buf[a->len] = '\0';
	
	return 0;
}
