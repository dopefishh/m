#include <stdbool.h>

#include <opus/opusfile.h>

#include "../file.h"
#include "../util.h"
#include "../db.h"
#include "../log.h"

bool process_opus(char *p, struct db_file *f)
{
	int r;
	OggOpusFile *of = op_open_file (p, &r);
	if(r != 0)
		return false;

	const OpusTags* c = op_tags(of, -1);
	file_tag_init(f, c->comments);

	for(int i = 0; i<c->comments; i++)
		if(file_tag_split_eq((char *)c->user_comments[i],
				&f->tags->keys[i], &f->tags->values[i]))
			f->tags->ntags--;

	op_free(of);
	return true;
}
