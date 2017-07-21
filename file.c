#include <string.h>

#include "cfg.h"

#ifdef USE_FLAC
#include "file_flac.h"
#endif
#ifdef USE_OGG
#include "file_ogg.h"
#endif
#ifdef USE_OPUS
#include "file_opus.h"
#endif
#ifdef USE_MP3
#include "file_mp3.h"
#endif

#include "db.h"
#include "util.h"
#include "log.h"

struct fmap {
	char *suffix;
	bool(*parser)(char *, struct db_file *);
};

struct fmap fmapping[] = {
#ifdef USE_FLAC
	{".flac", &process_flac},
#endif
#ifdef USE_OGG
	{".ogg", &process_ogg},
#endif
#ifdef USE_OPUS
	{".opus", &process_opus},
#endif
#ifdef USE_MP3
	{".mp3", &process_mp3},
#endif
	{0, 0}
};

void process_file(char *p, struct db_file *f)
{
	struct fmap *fe = &fmapping[0];
	char *suffix = strrchr(p, '.');
	f->tags = safe_malloc(sizeof(struct db_tags));
	while(fe->suffix != NULL && fe->parser != NULL){
		if(suffix != NULL && strcasecmp(suffix, fe->suffix) == 0){
			logmsg(debug, "Detected suffix: %s\n", fe->suffix);
			if(fe->parser(p, f)){
				logmsg(debug, "%lu comments processed\n",
					f->tags->ntags);
				return;
			}
			logmsg(debug,
				"Unable to open %s as %s\n", p, fe->suffix);
		}
		fe++;
	}
	free(f->tags);
	f->tags = NULL;
	logmsg(debug, "Unable to find a music format for this file: %s\n", p);
}

void free_file(struct db_file f)
{
	if(f.tags != NULL){
		for(uint64_t i = 0; i<f.tags->ntags; i++)
			safe_free(2, f.tags->values[i], f.tags->keys[i]);
		safe_free(3, f.tags->values, f.tags->keys, f.tags);
	}
	free(f.path);
}

void file_tag_init(struct db_file *f, uint64_t numtags)
{
	f->tags->ntags = numtags;
	f->tags->keys = safe_malloc(numtags*sizeof(char *));
	f->tags->values = safe_malloc(numtags*sizeof(char *));
}

bool file_tag_split_eq(char *k, char **key, char **value)
{
	char *v = strchr(k, '=');
	if(v[0] == '\0'){
		logmsg(debug, "Malformed comment\n");
		return false;
	}
	v++[0] = '\0';
	*key = safe_strdup(v);
	*value = safe_strdup(k);
	return true;
}
