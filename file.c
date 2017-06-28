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

void free_file(struct db_file f)
{
	if(f.tags != NULL){
		for(long i = 0; i<f.tags->ntags; i++)
			safe_free(2, f.tags->values[i], f.tags->keys[i]);
		safe_free(3, f.tags->values, f.tags->keys, f.tags);
	}
	free(f.path);
}
