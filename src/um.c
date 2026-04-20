#include <um.h>

#include <lib/sv.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct um_context {
    struct string_view *block;
    int block_count;
    int block_cap;
};

static struct um_context context;

struct parser_backend um_backend()
{
    return (struct parser_backend) {
        .on_block_start = um_block_start_cb,
        .on_block_end   = um_block_end_cb,
        .on_kv          = um_kv_cb,
        .on_init        = um_init
    };
}

void um_init()
{
    context.block = realloc(context.block, 10 * sizeof(struct string_view));
    context.block_cap = 10;
    context.block_count = 0;
}

void um_block_start_cb(struct string_view *block, void *userdata)
{
    assert(block != NULL);
    assert(userdata != NULL);

    if(context.block_count + 1 >= context.block_cap) {
        int ncap = context.block_cap * 2;
        context.block = realloc(context.block, ncap * sizeof(struct string_view));
        context.block_cap = ncap;
    }

    context.block[context.block_count].buf = strndup(block->buf, block->len);   
    context.block[context.block_count].len = block->len;   
    context.block_count++;

    printf("block was added to context, name: %.*s\n", (int)context.block[context.block_count - 1].len, context.block[context.block_count - 1].buf); //UNSAFE 
}

void um_block_end_cb(struct string_view *block, void *userdata)
{
    assert(block != NULL);
    assert(userdata != NULL);

    if(context.block_count < 1) {
        fprintf(stderr, "Cannot end a block, if there are no blocks!");
        exit(1);
    }

    struct string_view top = context.block[context.block_count - 1];

    if(strncmp(block->buf, top.buf, top.len) != 0) {
        fprintf(stderr, "Died!");
        exit(1);
    }

    context.block_count--;
    printf("Block was removed from context: %.*s\n", (int)block->len, block->buf);
}

void um_kv_cb(struct string_view *key, 
              struct string_view *value,
              void *userdata)
{
    assert(key != NULL);
    assert(value != NULL);
    assert(userdata != NULL);
}
