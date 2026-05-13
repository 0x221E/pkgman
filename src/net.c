#include <net.h>
#include <err.h>

#include <assert.h>
#include <string.h>
#include <curl/curl.h>
#include <stdlib.h>

size_t write_cb(char *ptr, size_t size, size_t nmemb, void *userdata)
{
	assert(ptr != NULL);
	assert(userdata != NULL);
    
	struct net_write_data *chunk = (struct net_write_data*)userdata;  
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
	
	struct net_file_write_data * user =
		(struct net_file_write_data*)userdata;
	
	if (user->file == NULL) {
		fprintf(stderr, "net: Write failed, "
			"file stream could not be found!\n");
		return 0;
	}

	return fwrite(ptr, size, nmemb, user->file); 
}

int net_init()
{
	CURLcode result = curl_global_init(CURL_GLOBAL_ALL);
	if (result != CURLE_OK)
		return -NETINITERR; 
	return SUCCESS;
}

void net_shutdown()
{
	curl_global_cleanup();
}

int net_send_request(char *url, int write_opts, void* userdata)
{
	assert(url != NULL);
	assert(userdata != NULL);
	CURL *curl;
	curl = curl_easy_init();
	
	if (!curl)
		return -CONINITERR;
    
	curl_easy_setopt(curl, CURLOPT_URL, url);
	
	if (write_opts == WRITE_OPT_MEMORY) {
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_cb);
	} else if (write_opts == WRITE_OPT_FILE) {
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_file_cb);
	} else {
		fprintf(stderr, "net: a valid write_opts must be specified! "
			"current: %d",
		write_opts);
		return -CONINITERR;
	}
    
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, userdata);
	curl_easy_setopt(curl, CURLOPT_CA_CACHE_TIMEOUT, 604800L);
	
	CURLcode result;
	result = curl_easy_perform(curl);

	if (result != CURLE_OK) {
		fprintf(stderr, "curl_easy_perform() failed: %s\n",
			curl_easy_strerror(result));
		return -(CONREQERR + result);
	}

	curl_easy_cleanup(curl);

	return SUCCESS;
}

int net_download(const char *url, const char *dst)
{
	struct net_file_write_data fwdata;

	fwdata.file = fopen(dst, "w"); // change to tmp dir
	
	if (!fwdata.file) {
		return -PKGNOTFND;
	}
	
	net_send_request(url, WRITE_OPT_FILE, (void*)&fwdata);
	
	fclose(fwdata.file);
	return SUCCESS;
}
