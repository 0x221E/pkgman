#ifndef UM_H
#define UM_H

#include <parser.h>
#include <lib/sv.h>

struct um_block_entry { 
    struct string_view name;
};

struct um_user_data {
    void *test;    
};

struct parser_backend um_backend();

void um_init();
void um_block_start_cb(struct string_view *block, void *userdata);
void um_block_end_cb(struct string_view *block, void *userdata);
void um_kv_cb(struct string_view *key, 
              struct string_view *value,
              void *userdata);

#endif
