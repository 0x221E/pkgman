#ifndef PARSER_H
#define PARSER_H

#include <stddef.h>

typedef struct memory memory;
typedef struct string_view string_view;

#define PARSER_BACKEND_UMANIFEST    1
#define PARSER_BACKEND_PKG          2

#define PARSER_STATE_GRACEFUL   1
#define PARSER_STATE_ERROR      2

typedef void (*parser_block_start_fn)(struct string_view*, void *userdata);
typedef void (*parser_block_end_fn)(struct string_view*, void *userdata);
typedef void (*parser_kv_fn)(struct string_view*, struct string_view*, void *userdata);
typedef void (*parser_init_fn)();

struct parser_backend {
    parser_block_start_fn   on_block_start;
    parser_block_end_fn     on_block_end;
    parser_kv_fn            on_kv;
    parser_init_fn          on_init;
};

struct parser {
    struct parser_backend *backend;
    char *src;
    size_t pos;
    size_t len;
    int state;
    void *userdata;
};

int parser_init(struct parser *parser, 
                struct memory *mem, 
                struct parser_backend *backend,
                void *userdata);
int parser_parse(struct parser *parser);
void parser_parse_block_start(struct parser *parser);
void parser_parse_block_end(struct parser *parser);
void parser_parse_kv(struct parser *parser);

#endif
