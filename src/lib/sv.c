#include <lib/sv.h>

struct string_view sv_create(char *buf, size_t len)
{
    return (struct string_view){.buf = buf, .len = len}; 
}
