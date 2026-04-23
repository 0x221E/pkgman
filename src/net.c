#include <net.h>
#include <err.h>

#include <assert.h>
#include <string.h>
#include <curl/curl.h>
#include <stdlib.h>

size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata)
{
    assert(ptr != NULL);
    assert(userdata != NULL);
    
    struct memory *chunk = (struct memory*)userdata;  
    size_t nsize = chunk->size + nmemb;
    
    chunk->buffer = realloc(chunk->buffer, nsize);
    
    if (nsize > chunk->size)
        memcpy(&chunk->buffer[chunk->size], ptr, nmemb);

    chunk->size = nsize;

    return nmemb;
}

size_t write_file_cb(char *ptr, size_t size, size_t nmemb, void *userdata)
{
    assert(ptr != NULL);
    assert(ptr != NULL);

    struct net_fwrite_userdata *user = (struct net_fwrite_userdata*)userdata;

    if (user->file == NULL) {
        fprintf(stderr, "net: Write failed, file stream could not be found!\n");
        return 0;
    }

    return fwrite(ptr, nmemb, 1, user->file); 
}

int net_init()
{
   CURLcode result = curl_global_init(CURL_GLOBAL_ALL);
   if (result != CURLE_OK)
       return -NETINITERR; 
    return 0;
}

void net_shutdown()
{
    curl_global_cleanup();
}

int net_send_request(char *url, struct memory *mem, int write_opts)
{
    assert(mem != NULL);
    CURL *curl;
    curl = curl_easy_init();
    if (!curl)
        return -CONINITERR; 

    curl_easy_setopt(curl, CURLOPT_URL, url);
    
    if (write_opts == WRITE_OPT_MEMORY)
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    else if (write_opts == WRITE_OPT_FILE)
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_file_cb);
    else {
        fprintf(stderr, "net: a valid write_opts must be specified! current: %d", write_opts);
    }

    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)mem); 
    curl_easy_setopt(curl, CURLOPT_CA_CACHE_TIMEOUT, 604800L);

    CURLcode result;
    result = curl_easy_perform(curl);

    if (result != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(result));
        return -(CONREQERR + result);
    }

    curl_easy_cleanup(curl);

    return 0;
}
