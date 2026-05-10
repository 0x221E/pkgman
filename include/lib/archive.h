#ifndef ARCHIVE_H
#define ARCHIVE_H

typedef struct url url;

int pkg_extract(char *archive_path, struct url *out_dir);

#endif
