#include <um.h>

#include <lib/sv.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

LL_DEFINE_ADD(manifest, struct pair)

struct um_context {
    int in_manifest;
};

static struct um_context context;

static struct string_view manifest_sv = SV("MANIFEST");

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
    context.in_manifest = 0;
}

void um_block_start_cb(struct string_view *block, void *userdata)
{
    assert(block != NULL);
    assert(userdata != NULL);
    
    if (sv_equal(&manifest_sv, block)) {
        context.in_manifest = 1;
    } else {
        fprintf(stderr, "[UPSTREAM ERROR] Unknown block: %.*s\n", 
                (int)block->len, block->buf); // (unsafe)
        exit(1);
    }
    printf("[UPSTREAM] Entered block %.*s.\n",
            (int)block->len, block->buf);
}

void um_block_end_cb(struct string_view *block, void *userdata)
{
    assert(block != NULL);
    assert(userdata != NULL);

    if (sv_equal(&manifest_sv, block)) {
        context.in_manifest = 0;
    } else {
        fprintf(stderr, "[UPSTREAM ERROR] Unknown block: %.*s\n", 
                (int)block->len, block->buf); // (unsafe)
        exit(1);
    }
    printf("[UPSTREAM] Left block %.*s.\n",
            (int)block->len, block->buf);
}

void um_kv_cb(struct string_view *key, 
              struct string_view *value,
              void *userdata)
{
    assert(key != NULL);
    assert(value != NULL);
    assert(userdata != NULL);

    if (context.in_manifest == 0) {
        fprintf(stderr, "[UPSTREAM ERROR] You can only define packages inside a manifest block.\n"); 
    }
    
    struct um_user_data *user = (struct um_user_data*)userdata;
    struct string_view ckey = sv_copy(key);
    struct string_view cval = sv_copy(value);

    ll_manifest_add(&user->manifest, (struct pair){.key = ckey, .value = cval});
}
