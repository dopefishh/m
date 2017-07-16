#include <string.h>

#include "cfg.h"

#ifdef USE_FLAC
#include "file_flac.h"
#endif
#ifdef USE_OGG
#include "file_ogg.h"
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
	{0, 0}
};

void process_file(char *p, struct db_file *f)
{
	struct fmap *fe = &fmapping[0];
	char *suffix = strrchr(p, '.');
	f->tags = NULL;
	while(fe->suffix != NULL && fe->parser != NULL){
		if(suffix != NULL && strcasecmp(suffix, fe->suffix) == 0){
			if(fe->parser(p, f))
				return;
		}
		fe++;
	}
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
