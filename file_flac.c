#include <stdbool.h>

#include <FLAC/metadata.h>

#include "util.h"
#include "db.h"
#include "log.h"

bool process_flac(char *p, struct db_file *f)
{
	logmsg(debug, "Found %s, as probably flac\n", p);

	FLAC__StreamMetadata *md;
	if(!FLAC__metadata_get_tags(p, &md)){
		logmsg(warn, "Unable to open %s as flac\n", p);
		f->tags = NULL;
		return false;
	}

	f->tags = safe_malloc(sizeof(struct db_tags));
	FLAC__StreamMetadata_VorbisComment c = md->data.vorbis_comment;
	f->tags->ntags = c.num_comments;
	f->tags->keys = safe_malloc(c.num_comments*sizeof(char *));
	f->tags->values = safe_malloc(c.num_comments*sizeof(char *));

	char *key;
	for(uint32_t i = 0; i<c.num_comments; i++){
		key = (char *)c.comments[i].entry;
		char *val = strchr(key, '=');
		if(val[0] == '\0'){
			logmsg(debug, "Malformed comment\n");
			f->tags->ntags--;
			continue;
		}
		val++[0] = '\0';
		f->tags->keys[i] = safe_strdup(key);
		f->tags->values[i] = safe_strdup(val);
	}
	logmsg(debug, "%lu comments processed\n", c.num_comments);

	FLAC__metadata_object_delete(md);
	return true;
}
