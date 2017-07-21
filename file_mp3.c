#include <stdbool.h>

#include <id3tag.h>

#include "file.h"
#include "util.h"
#include "db.h"
#include "log.h"

void process_tag(unsigned int nfields, union id3_field *f, char **key, char **value)
{
	char buf[25];
	for(unsigned int i = 0; i<nfields; i++){
		switch(id3_field_type(&f[i])){
		case ID3_FIELD_TYPE_TEXTENCODING:
			logmsg(debug, "Textencoding\n");
			break;
		case ID3_FIELD_TYPE_LATIN1:
			logmsg(debug, "Latin1\n");
			break;
		case ID3_FIELD_TYPE_LATIN1FULL:
			logmsg(debug, "Latin1full\n");
			break;
		case ID3_FIELD_TYPE_LATIN1LIST:
			logmsg(debug, "Latin1list\n");
			break;
		case ID3_FIELD_TYPE_STRING:
			logmsg(debug, "string\n");
			break;
		case ID3_FIELD_TYPE_STRINGFULL:
			logmsg(debug, "stringfull\n");
			break;
		case ID3_FIELD_TYPE_STRINGLIST:
			logmsg(debug, "stringlist\n");
			break;
		case ID3_FIELD_TYPE_LANGUAGE:
			logmsg(debug, "language\n");
			break;
		case ID3_FIELD_TYPE_FRAMEID:
			logmsg(debug, "frameid\n");
			break;
		case ID3_FIELD_TYPE_DATE:
			logmsg(debug, "date\n");
			break;
		case ID3_FIELD_TYPE_INT8:
		case ID3_FIELD_TYPE_INT16:
		case ID3_FIELD_TYPE_INT24:
		case ID3_FIELD_TYPE_INT32:
		case ID3_FIELD_TYPE_INT32PLUS:
			sprintf(buf, "%ld", f[i].number.value);
			*value = safe_strdup(buf);
			break;
		case ID3_FIELD_TYPE_BINARYDATA:
			logmsg(debug, "Binary tags not supported, images?\n");
//			*value = safe_strdup(id3_field_getbinarydata(
//				&f[i], NULL);
			break;
		}
	}
}

bool process_mp3(char *p, struct db_file *f)
{
	struct id3_file *mf = id3_file_open(p, ID3_FILE_MODE_READONLY);
	if(mf == NULL)
		return false;

	struct id3_tag *mt = id3_file_tag(mf);
	file_tag_init(f, mt->nframes);

	for(unsigned int i = 0; i<mt->nframes; i++){
		struct id3_frame *fr = mt->frames[i];
		logmsg(debug, "frame %d: %s\n", i, fr->description);
		process_tag(fr->nfields, fr->fields,
			&f->tags->keys[i], &f->tags->values[i]);
	}
	return id3_file_close(mf) == 0;
}
