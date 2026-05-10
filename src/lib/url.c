#include <lib/url.h>

#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <err.h>

//TODO: Audit this implementation. Potentially an arena alloc?

int url_init(struct url *url, char *base)
{
	assert(url != NULL);

	size_t len = strlen(base) + 1;
	void *ptr = malloc(len);
	
	if (ptr == NULL)
		return -URLINITERR;

	url->buffer = ptr;
	memcpy(url->buffer, base, len); 
	url->len = len;
	url->buffer[url->len - 1] = '\0';
	
	return SUCCESS;
}

void url_free(struct url *url)
{
	assert(url != NULL); 
	free(url->buffer);
}


int url_copy(struct url *from, struct url *to)
{
	assert(from != NULL);
	assert(to != NULL);

	to->buffer = malloc(from->len);
	if(to->buffer == NULL)
		return -ERR;
	
	memcpy(to->buffer, from->buffer, from->len);
		
	to->len = from->len;
	return SUCCESS;
}

int url_append(struct url *url, const char *add)
{
	assert(url != NULL);
	assert(add != NULL);
	
	if (url->buffer == NULL || url->len < 8) {
	    return -URLPATHERR;
	}

	url->len--;
	size_t addlen = strlen(add);
	size_t nlen = url->len + addlen + 1;

	url->buffer = realloc(url->buffer, nlen);

	memcpy(url->buffer + url->len, add, addlen);

	url->len = nlen;
	url->buffer[url->len - 1] = '\0';
}

int url_append_path(struct url *url, const char *add)
{
	assert(url != NULL);
	assert(add != NULL);
    
	if (url->buffer == NULL || url->len < 8) {
		return -URLPATHERR;
	}
	
	url->buffer[url->len - 1] = '/'; //TODO: Check existence of "/"
	size_t addlen = strlen(add);
	size_t nlen = url->len + addlen + 1;

	url->buffer = realloc(url->buffer, nlen);

	memcpy(url->buffer + url->len, add, addlen);

	url->len = nlen;
	url->buffer[url->len - 1] = '\0';	
}
