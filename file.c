#include <string.h>
#include <FLAC/metadata.h>

#include "db.h"
#include "log.h"

bool process_flac(char *p, struct db_entry *entry);

struct fmap {
	char *suffix;
	bool(*parser)(char *, struct db_entry *);
};

struct fmap f[] = {
	{".flac", &process_flac},
	{0, 0}
};

void process_file(char *p, struct db_entry *entry)
{
	struct fmap *fe = &f[0];
	char *suffix = strrchr(p, '.');
	while(fe->suffix != NULL && fe->parser != NULL){
		if(suffix[0] == '\0' || strcasecmp(suffix, f->suffix) == 0){
			if(fe->parser(p, entry))
				return;
		}
		fe++;
	}
	logmsg(debug, "Unable to find a music format for this file: %s\n", p);
}

bool process_flac(char *p, struct db_entry *entry)
{
	logmsg(debug, "Found %s as probably flac\n", p);
	(void)entry;
	FLAC__StreamMetadata *md;
	if(!FLAC__metadata_get_tags(p, &md)){
		logmsg(warn, "Unable to open %s as flac\n", p);
		return false;
	}

	FLAC__StreamMetadata_VorbisComment c = md->data.vorbis_comment;
	logmsg(debug, "Num comments: %lu\n", c.num_comments);
	for(uint32_t i = 0; i<c.num_comments; i++){
		logmsg(debug, "Tag: %s\n", c.comments[i].entry);
	}

	FLAC__metadata_object_delete(md);
	return true;
}
