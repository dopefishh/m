#include <vorbis/vorbisfile.h>

#include "db.h"
#include "util.h"
#include "log.h"

bool process_ogg(char *p, struct db_file *f)
{
	logmsg(debug, "Found %s, as probably ogg\n", p);

	OggVorbis_File vf;
	if(ov_fopen(p, &vf) != 0){
		logmsg(warn, "Unable to open %s as ogg\n", p);
		f->tags = NULL;
		return false;
	}

	f->tags = safe_malloc(sizeof(struct db_tags));
	vorbis_comment *c = ov_comment(&vf, -1);
	f->tags->ntags = c->comments;
	f->tags->keys = safe_malloc(c->comments*sizeof(char *));
	f->tags->values = safe_malloc(c->comments*sizeof(char *));
	char *key;
	for(int i = 0; i<c->comments; i++){
		key = c->user_comments[i];
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
	logmsg(debug, "%lu comments processed\n", c->comments);
	ov_clear(&vf);
	return true;
}

