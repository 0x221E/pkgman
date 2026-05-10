#include <pkgman.h>

#include <net.h>
#include <parser.h>
#include <um.h>
#include <lib/sv.h>
#include <err.h>
#include <lib/url.h>
#include <lib/archive.h>
#include <cookbook.h>

#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <gpgme.h>
#include <archive.h>
#include <archive_entry.h>

#define UPSTREAM_URL "https://packages.0xinfinity.dev"

int pkgman_upstream_check(const char *pkg)
{
	assert(pkg != NULL);

	int ret = -ERR;		
	
	struct net_write_data mem = { 0 };
	if ((ret = net_send_request(UPSTREAM_URL "/list",
				    WRITE_OPT_MEMORY,
				    (void*)&mem)) != SUCCESS)
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

	return ret;
}

int pkgman_download(const char *url, const char *dst)
{
	struct net_file_write_data fwdata;

	printf("url: %s, dst: %s\n", url, dst);
	
	fwdata.file = fopen(dst, "w"); // change to tmp dir
	
	if (!fwdata.file) {
		return -PKGNOTFND;
	}
	
	net_send_request(url, WRITE_OPT_FILE, (void*)&fwdata);
	
	fclose(fwdata.file);
	return SUCCESS;
}

int pkgman_install_pkg(const char *pkg)
{
	struct url path = {0};
	url_init(&path, "/tmp/pkgman");
	url_append_path(&path, pkg);
	
	struct url dst_path = {0};
	url_init(&dst_path, "/tmp/pkgman");
	url_append_path(&dst_path, pkg);
	url_append(&dst_path, "-extract/");

	mkdir(dst_path.buffer, 0777);

        pkg_extract(path.buffer, &dst_path);

	cookbook_run(&dst_path, "artifacts");
	cookbook_run(&dst_path, "install");
	
        url_free(&dst_path);
	url_free(&path);
	return SUCCESS;
}

int pkgman_upstream_integrity_download(const char *pkg)
{
	int ret = -ERR;
	struct url url_pkg = {0};
	struct url url_sig = {0};

	url_init(&url_pkg, UPSTREAM_URL);
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

	if(pkgman_download(url_pkg.buffer, path_pkg.buffer) != SUCCESS) {
		ret = -PKGNOTFND;
		goto cleanup;
	}
	
	printf("'%s' package downloaded.\n", pkg);
	
	// Download signature file
	if(pkgman_download(url_sig.buffer, path_sig.buffer) != SUCCESS) {
		ret = -PKGNOTFND;
		goto cleanup;
	}
	
	printf("Signature for '%s' downloaded.\n", url_sig.buffer);
	
	// Verify integrity	
	
	gpgme_check_version(NULL);

	gpgme_ctx_t ctx;

	gpgme_new(&ctx);
	gpgme_set_protocol(ctx, GPGME_PROTOCOL_OPENPGP);
	
	int fd = open(path_pkg.buffer, O_RDONLY);
	int sigfd = open(path_sig.buffer, O_RDONLY);

	gpgme_data_t file;
	gpgme_data_t sig;
	
	gpgme_data_new_from_fd(&file, fd);
	gpgme_data_new_from_fd(&sig, sigfd);
	
	if (gpgme_op_verify(ctx, sig, file, NULL) == GPG_ERR_NO_ERROR) {
		printf("Signature operation ran successfully.\n");
	} else {
		printf("Integrity verification failed!\n");
		ret = -INTEGRITYERR;
		gpgme_release(ctx);
		goto cleanup;
	}

	
	gpgme_verify_result_t integ_res = gpgme_op_verify_result(ctx);

	// Checking the first signature for now should be fine.
	//TODO: Revisit this.
	if ((integ_res->signatures->summary & GPGME_SIGSUM_VALID) &&
	    (integ_res->signatures->status == GPG_ERR_NO_ERROR)) {
		printf("Integrity check successful, "
		       "package can be installed.\n");
	} else {
		printf("Signature invalid!\n");
		ret = -INTEGRITYERR;
		gpgme_release(ctx);
		goto cleanup;
	}       

	ret = SUCCESS;

	gpgme_release(ctx);
	
 cleanup:
	
	url_free(&url_pkg);
	url_free(&url_sig);

	url_free(&path_pkg);
	url_free(&path_sig);
	
	return ret;
}
