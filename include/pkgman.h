#ifndef PKGMAN_H
#define PKGMAN_H

int pkgman_upstream_check(const char *pkg);
int pkgman_install_pkg(const char *pkg);
int pkgman_upstream_integrity_download(const char *pkg);

#endif
