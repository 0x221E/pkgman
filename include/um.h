#ifndef UM_H
#define UM_H

#include <parser.h>
#include <lib/sv.h>
#include <lib/ll.h>

struct pair {
   struct string_view key;
   struct string_view value;
};

LL_DEFINE(manifest, struct pair);

struct um_user_data {
    struct ll_manifest manifest;
};

struct parser_backend um_backend();

void um_init(void *userdata);
void um_block_start_cb(struct string_view *block, void *userdata);
void um_block_end_cb(struct string_view *block, void *userdata);
void um_kv_cb(struct string_view *key, 
              struct string_view *value,
              void *userdata);

#endif
