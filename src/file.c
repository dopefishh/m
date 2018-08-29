#include <string.h>

#include "../config.h"

#ifdef USE_FLAC
#include "file/flac.h"
#endif
#ifdef USE_OGG
#include "file/ogg.h"
#endif
#ifdef USE_OPUS
#include "file/opus.h"
#endif
#ifdef USE_MP3
#include "file/mp3.h"
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

static int tag_cmp(const void *t1, const void *t2)
{
	return strcasecmp(((struct db_tag *)t1)->key,
		((struct db_tag *)t2)->key);
}

void process_file(char *p, struct db_file *f)
{
	struct fmap *fe = &fmapping[0];
	char *suffix = strrchr(p, '.');
	while(fe->suffix != NULL && fe->parser != NULL){
		if(suffix != NULL && strcasecmp(suffix, fe->suffix) == 0){
			logmsg(debug, "Detected suffix: %s\n", fe->suffix);
			if(fe->parser(p, f)){
				qsort(f->tags, f->ntags, sizeof(struct db_tag),
					tag_cmp);
				logmsg(debug, "%lu tags processed\n",
					f->ntags);

				return;
			}
			logmsg(debug,
				"Unable to open %s as %s\n", p, fe->suffix);
		}
		fe++;
	}
	logmsg(debug, "Unable to find a music format for this file: %s\n", p);
}

void free_file(struct db_file f)
{
	if(f.tags != NULL){
		for(uint64_t i = 0; i<f.ntags; i++)
			safe_free(2, f.tags[i].value, f.tags[i].key);
		free(f.tags);
	}
	free(f.path);
}

void file_tag_init(struct db_file *f, uint64_t numtags)
{
	f->ntags = numtags;
	f->tags = safe_malloc(numtags*sizeof(struct db_tag));
}

bool file_tag_split_eq(char *k, char **key, char **value)
{
	char *v = strchr(k, '=');
	if(v[0] == '\0'){
		logmsg(debug, "Malformed comment\n");
		return false;
	}
	v++[0] = '\0';
	*key = safe_strdup(k);
	*value = safe_strdup(v);
	return true;
}

char *file_tag_find(struct db_file *f, char *key)
{
	if(f->tags == NULL)
		return NULL;
	struct db_tag t = {.key=key, .value=NULL};
	struct db_tag *r =
		bsearch(&t, f->tags, f->ntags, sizeof(struct db_tag), tag_cmp);
	return r != NULL ? ((struct db_tag *)r)->value : NULL;
}
