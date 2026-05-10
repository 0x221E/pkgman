#include <crypto.h>

#include <gpgme.h>

#include <err.h>

#include <fcntl.h>

int crypto_verify_integrity(const char *sig, const char *file)
{
	int ret = -ERR;
	
	gpgme_check_version(NULL);

	gpgme_ctx_t ctx;

	gpgme_new(&ctx);
	gpgme_set_protocol(ctx, GPGME_PROTOCOL_OPENPGP);
	
	int fd = open(file, O_RDONLY);
	int sigfd = open(sig, O_RDONLY);

	gpgme_data_t gfile;
	gpgme_data_t gsig;
	
	gpgme_data_new_from_fd(&gfile, fd);
	gpgme_data_new_from_fd(&gsig, sigfd);
	
	if (gpgme_op_verify(ctx, gsig, gfile, NULL) == GPG_ERR_NO_ERROR) {
		printf("Signature operation ran successfully.\n");
	} else {
		printf("Integrity verification failed!\n");
		ret = -INTEGRITYERR;
		goto cleanup;
	}
	
	gpgme_verify_result_t integ_res = gpgme_op_verify_result(ctx);

	// Checking the first signature for now should be fine.
	//TODO: Revisit this.
	if ((integ_res->signatures->summary & GPGME_SIGSUM_VALID) &&
	    (integ_res->signatures->status == GPG_ERR_NO_ERROR)) {
		printf("Integrity check successful, "
		       "package can be installed.\n");
		ret = SUCCESS;
		goto cleanup;
	} else {
		printf("Signature invalid!\n");
		ret = -INTEGRITYERR;
		goto cleanup;
	}       
 cleanup:
	gpgme_release(ctx);
	return ret;
}
