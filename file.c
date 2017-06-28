#include <string.h>
#include <FLAC/metadata.h>

#include "db.h"
#include "util.h"
#include "log.h"

bool process_flac(char *p, struct db_file *f);

struct fmap {
	char *suffix;
	bool(*parser)(char *, struct db_file *);
};

struct fmap fmapping[] = {
	{".flac", &process_flac},
	{0, 0}
};

void process_file(char *p, struct db_file *f)
{
	struct fmap *fe = &fmapping[0];
	char *suffix = strrchr(p, '.');
	while(fe->suffix != NULL && fe->parser != NULL){
		if(suffix[0] == '\0' || strcasecmp(suffix, fe->suffix) == 0){
			if(fe->parser(p, f))
				return;
		}
		fe++;
	}
	logmsg(debug, "Unable to find a music format for this file: %s\n", p);
}

bool process_flac(char *p, struct db_file *f)
{
	logmsg(debug, "Found %s as probably flac\n", p);
	(void)f;
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

void free_file(struct db_file *f)
{
	if(f == NULL)
		return;
	if(f->tags != NULL){
		safe_free(2, f->tags->values, f->tags->keys);
	}
	safe_free(2, f->tags, f);
}
