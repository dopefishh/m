#include <stdbool.h>

#include <FLAC/metadata.h>

#include "file.h"
#include "util.h"
#include "db.h"
#include "log.h"

bool process_flac(char *p, struct db_file *f)
{
	FLAC__StreamMetadata *md;
	if(!FLAC__metadata_get_tags(p, &md))
		return false;

	FLAC__StreamMetadata_VorbisComment c = md->data.vorbis_comment;
	file_tag_init(f, c.num_comments);

	for(uint32_t i = 0; i<c.num_comments; i++){
		if(!file_tag_split_eq((char *)c.comments[i].entry,
				&f->tags->keys[i], &f->tags->values[i]))
			f->tags->ntags--;
	}

	FLAC__metadata_object_delete(md);
	return true;
}
