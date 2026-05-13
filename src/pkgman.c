#include <pkgman.h>

#include <net.h>
#include <parser.h>
#include <um.h>
#include <lib/sv.h>
#include <err.h>
#include <lib/url.h>
#include <lib/archive.h>
#include <cookbook.h>
#include <crypto.h>

#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <gpgme.h>
#include <archive.h>
#include <archive_entry.h>
#include <errno.h>

#define PKGMAN_CONFIG_CHECK(x)							\
	do {									\
		TRY(pc->dir_tmp != NULL, PKGCONFERR);				\
		TRY(pc->dir_staging != NULL, PKGCONFERR);			\
		TRY(pc->upstream != NULL, PKGCONFERR);				\
	} while(0)								

int pkgman_upstream_check(struct pkgman_config *pc, const char *pkg)
{
	assert(pkg != NULL);
	PKGMAN_CONFIG_CHECK(pc);

	int ret = -ERR;

	struct url upstream;
	url_init(&upstream, pc->upstream);
	url_append_path(&upstream, "list");
	
	struct net_write_data mem = { 0 };
	ret = net_send_request(upstream.buffer,
			       WRITE_OPT_MEMORY,
			       (void*)&mem);
	
	if (ret != SUCCESS)
		goto cleanup;
	
	struct um_user_data userdata = { 0 };
	struct parser parser;
	struct parser_backend backend = um_backend();
	struct string_view parser_src = {0};
	parser_src.buf = mem.buffer;
	parser_src.len = mem.size;
	
	parser_init(&parser, &parser_src, &backend, (void*)&userdata);
	parser_parse(&parser);
	
	struct string_view sv_pkg = sv_create(pkg, strlen(pkg)); 
	
	int found = 0;
	
	LL_FOREACH(manifest, &userdata.manifest) {
		if (!current->data.key.buf)
			continue;
		
		if (sv_equal(&sv_pkg, &current->data.key)) {
			found = 1;
			printf("Package '%s' found on upstream!\n", pkg);
		        ret = SUCCESS;
		}
	}
	
	if (!found) {
		printf("Package '%s' not found!\n", pkg);
		ret =  -PKGNOTFND;
	}
	

	LL_FOREACH(manifest, &userdata.manifest) {
		if (current->data.key.buf == NULL)
		    continue;
		
		free(current->data.key.buf);
		free(current->data.value.buf);
	}
	
	ll_manifest_free(&userdata.manifest);

 cleanup:	
	free(mem.buffer);
	url_free(&upstream);

	return ret;
}

int pkgman_install_pkg(struct pkgman_config *pc, const char *pkg)
{
	assert(pc != NULL);
	PKGMAN_CONFIG_CHECK(pc);
	
	int ret = -ERR;
	
	struct url path = {0};
	url_init(&path, pc->dir_tmp);
	url_append_path(&path, pkg);
	
	struct url dst_path = {0};
	url_init(&dst_path, pc->dir_tmp);
	url_append_path(&dst_path, pkg);
	url_append(&dst_path, "-extract/");

	if (mkdir(dst_path.buffer, 0777) != 0 && errno != EEXIST) {
		ret = -ERR;
		goto cleanup;			
	}

        pkg_extract(path.buffer, &dst_path);

	struct string_view recipe_out = {0};
	sv_init(&recipe_out, "");

	int cret = cookbook_recipe_run(&dst_path, "artifacts", &recipe_out);
	
	if (cret != 0) {
		printf("Artifacts recipe failed with error code: %d", cret);
		ret = -ERR;
		goto cleanup;
	}

	printf("Not displaying artifacts...\n");

	sv_free(&recipe_out);
	sv_init(&recipe_out, "");

        cret = cookbook_recipe_run(&dst_path, "install", &recipe_out);
	
	if (cret != 0) {
		printf("Install recipe failed with error code: %d", cret);
		ret = -ERR;
		goto cleanup;
	}

	printf("Build script response: %s\n", recipe_out.buf);

	ret = SUCCESS;
	
 cleanup:      
        sv_free(&recipe_out);
        url_free(&dst_path);
	url_free(&path);
	return ret;
}

int pkgman_upstream_integrity_download(struct pkgman_config *pc,
				       const char *pkg)
{
	assert(pc != NULL);
	PKGMAN_CONFIG_CHECK(pc);

	int ret = -ERR;
	struct url url_pkg = {0};
	struct url url_sig = {0};

	url_init(&url_pkg, pc->upstream);
	url_append_path(&url_pkg, pkg);
	url_append(&url_pkg, ".tar.zstd");

	url_copy(&url_pkg, &url_sig);
	url_append(&url_sig, ".sig");

	struct url path_pkg = {0};
	struct url path_sig = {0};

	url_init(&path_pkg, "/tmp/pkgman");
	url_append_path(&path_pkg, pkg);

	url_copy(&path_pkg, &path_sig);
	url_append(&path_sig, ".sig");
	
	///// I need to abstract away the cache filemgmt feature in the future.

	mkdir("/tmp/pkgman/", 0777);
	
	////

	if(net_download(url_pkg.buffer, path_pkg.buffer) != SUCCESS) {
		ret = -PKGNOTFND;
		goto cleanup;
	}
	
	printf("Package '%s' is downloaded from upstream.\n", pkg);
	
	// Download signature file
	if(net_download(url_sig.buffer, path_sig.buffer) != SUCCESS) {
		ret = -PKGNOTFND;
		goto cleanup;
	}
	
	printf("Signature for '%s' downloaded.\n", url_sig.buffer);
	
	if(crypto_verify_integrity(path_sig.buffer, path_pkg.buffer)
	   != SUCCESS) {
		ret = -INTEGRITYERR;
		goto cleanup;
	}

	ret = SUCCESS;

 cleanup:
	url_free(&url_pkg);
	url_free(&url_sig);

	url_free(&path_pkg);
	url_free(&path_sig);
	
	return ret;
}
