#ifndef PKGMAN_H
#define PKGMAN_H

typedef struct url url;

struct pkgman_config {
	char *dir_tmp;
        char *dir_staging;
        char *upstream;
};

int pkgman_upstream_check(struct pkgman_config *pc, const char *pkg);
int pkgman_install_pkg(struct pkgman_config *pc, const char *pkg);
int pkgman_upstream_integrity_download(struct pkgman_config *pc,
				       const char *pkg);

#endif
