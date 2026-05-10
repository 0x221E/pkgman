#include <lib/archive.h>

#include <err.h>
#include <lib/url.h>

#include <assert.h>

#include <archive.h>
#include <archive_entry.h>

int pkg_extract(char *archive_path, struct url *out_dir)
{
	assert(out_dir != NULL);
	
	struct archive *read = archive_read_new();
	struct archive *disk = archive_write_disk_new();
	// Read opts
	archive_read_support_filter_zstd(read);
	archive_read_support_format_tar(read);

	archive_read_open_filename(read, archive_path, 65535);
	// Write opts
	archive_write_disk_set_options(disk, ARCHIVE_EXTRACT_TIME);

	// tmp archive entry (for iterating read)
	struct archive_entry *entry;

	while (archive_read_next_header(read, &entry) == ARCHIVE_OK) {
		const char *entry_path = archive_entry_pathname(entry);

		struct url fullpath = {0};
		url_copy(out_dir, &fullpath);
		url_append(&fullpath, entry_path);

		archive_entry_set_pathname(entry, fullpath.buffer);

		archive_write_header(disk, entry);

		const void *buf;
		size_t len;
		off_t offset;
		
		while(archive_read_data_block(read, &buf, &len, &offset) == ARCHIVE_OK)
			archive_write_data_block(disk, buf, len, offset);

		archive_write_finish_entry(disk);
		url_free(&fullpath);
	}

	archive_free(disk);
	archive_free(read);
	
	return SUCCESS;
}
