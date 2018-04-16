#include <vorbis/vorbisfile.h>

#include "../file.h"
#include "../db.h"
#include "../util.h"
#include "../log.h"

bool process_ogg(char *p, struct db_file *f)
{
	OggVorbis_File vf;
	if(ov_fopen(p, &vf) != 0)
		return false;

	vorbis_comment *c = ov_comment(&vf, -1);
	file_tag_init(f, c->comments);

	for(int i = 0; i<c->comments; i++)
		if(!file_tag_split_eq((char *)c->user_comments[i],
				&f->tags->keys[i], &f->tags->values[i]))
			f->tags->ntags--;

	ov_clear(&vf);
	return true;
}
