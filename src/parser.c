/**
 * THIS FILE IS PENDING SECONDARY REVIEW. MAY CONTAIN VULNERABILITIES!
 *
 * Parser code is inherently more prone to vulnerabilities. This will have to be checked!!
 */

/**
 * @file parser.c
 * @author 0x221E
 * @brief .
 * @description 
 *
 * @warning User manages the memory of the parser. 
 * @warning Backend manages the memory of itself.
 * @warning Parser is guaranteed to provide safe memory, constrained by the length of string_view.
 */

// TODO: Count newlines, add a new field to parser struct.

#include <parser.h>
#include <err.h>
#include <net.h>
#include <lib/sv.h>

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stddef.h>

int is_alpha(char c)
{
    return (c >= 'a' && c <= 'z') ||
            (c >= 'A' && c <= 'Z');
}

int is_key_char(char c)
{
    return is_alpha(c) || c == '_';
}

int is_whitespace(char c)
{
    return c == ' ' || c == '\r' || c == '\t';
}

char parser_peek(struct parser *parser, size_t i)
{
    size_t npos = parser->pos + i;
    if(npos >= parser->len) 
        return parser->src[parser->pos];
    return parser->src[npos];
}

char parser_advance(struct parser *parser, size_t i)
{
    size_t npos = parser->pos + i;
    if(npos >= parser->len) 
        return parser->src[parser->pos];
    parser->pos = npos;
    return parser->src[npos];
}

/* expects on the current position */
int parser_expect(struct parser *parser, char e)
{
    assert(parser != NULL); 
    char c = parser_peek(parser, 0);
    if(c != e)
        return 1;
    parser_advance(parser, 1);
    return 0;
}

void parser_expect_throw(struct parser *parser, char e, const char *fail)
{
    if(parser_expect(parser, e)) {
        fprintf(stderr, "%s", fail); 
        parser->state = PARSER_STATE_ERROR;
    }
}

void skip_whitespace(struct parser *parser)
{
   while(parser->pos < parser->len &&
         is_whitespace(parser->src[parser->pos]))
       parser->pos++;
}

int parser_init(struct parser *parser, 
                struct memory *mem, 
                struct parser_backend *backend,
                void *userdata)
{
    assert(parser != NULL);
    assert(mem != NULL);
    assert(backend != NULL);

    if ((backend->on_block_start == NULL)   ||
        (backend->on_block_end == NULL)     ||
        (backend->on_kv == NULL)            ||
        (mem->buffer == NULL))              
        return -PARINITERR;

    parser->pos = 0;
    parser->src = mem->buffer;
    parser->len = mem->size;
    parser->state = PARSER_STATE_GRACEFUL;
    parser->backend = backend;
    
    parser->backend->on_init();

    return SUCCESS;
}

int parser_parse(struct parser *parser)
{
    assert(parser != NULL);

    while(parser->pos < parser->len - 1) {         
        char c = parser_peek(parser, 0);
        char nc = parser_peek(parser, 1);

        if (c == '[' && nc == '/')
            parser_parse_block_end(parser);
        else if (c == '[')
            parser_parse_block_start(parser);
        else if (c == '-')
            parser_parse_kv(parser);
        else
            parser_advance(parser, 1);
    }
    
    if (parser->state == PARSER_STATE_ERROR)
        return -PARPARERR;

    return SUCCESS;
}

void parser_parse_block_start(struct parser *parser)
{
    assert(parser != NULL);
    parser_expect_throw(parser, '[', "Block beginning expects a '['.");
    
    size_t spos = parser->pos;
    size_t len = 0;

    while (parser->pos < parser->len && 
           is_alpha(parser_peek(parser, 0))) {
        
        parser_advance(parser, 1);
        len++;

    }

    printf("Block start compiled with name: %.*s\n", (int)len, &parser->src[spos]);

    parser_expect_throw(parser, ']', "Block beginning expects a ']'.");
    parser_expect_throw(parser, '\n', "Block must be on a seperate line!");
    
    struct string_view block_name = sv_create(&parser->src[spos], len);
    parser->backend->on_block_start(&block_name, parser->userdata);
}

void parser_parse_block_end(struct parser *parser)
{
    assert(parser != NULL);
    parser_expect_throw(parser, '[', "Block end expects a '['.");
    parser_expect_throw(parser, '/', "Block end expects a '/'.");
    
    size_t spos = parser->pos;
    size_t len = 0;

    while (parser->pos < parser->len &&
           is_alpha(parser_peek(parser, 0))) {

        parser_advance(parser, 1);
        len++;

    }

    printf("Block end compiled with name: %.*s\n", (int)len, &parser->src[spos]);

    parser_expect_throw(parser, ']', "Block end expects a ']'.");
    parser_expect_throw(parser, '\n', "Block must be on a seperate line!");

    struct string_view block_name = sv_create(&parser->src[spos], len);
    parser->backend->on_block_end(&block_name, parser->userdata);
}

void parser_parse_kv(struct parser *parser)
{
    assert(parser != NULL);
    parser_expect_throw(parser, '-', "key-value expects a '-'.");
    
    size_t keypos = parser->pos;
    size_t keylen = 0;

    while (parser->pos < parser->len &&
           is_key_char(parser_peek(parser, 0))) {

        parser_advance(parser, 1);
        keylen++;

    }

    parser_expect_throw(parser, '=', "key-value expects a '='.\n"); 

    size_t valpos = parser->pos;
    size_t vallen = 0;

    while (parser->pos < parser->len &&
          parser_peek(parser, 0) != '\n') {

        parser_advance(parser, 1);
        vallen++;

    }

    printf("Key: %.*s, value: %.*s.\n", (int)keylen, &parser->src[keypos], (int)vallen, &parser->src[valpos]);
    parser_expect_throw(parser, '\n', "key-value items must be seperated with a newline character");

    struct string_view key = sv_create(&parser->src[keypos], keylen);
    struct string_view value = sv_create(&parser->src[valpos], vallen);
    parser->backend->on_kv(&key, &value, parser->userdata);
}
