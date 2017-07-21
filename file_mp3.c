#include <stdbool.h>

#include <id3tag.h>

#include "config.h"
#include "file.h"
#include "util.h"
#include "db.h"
#include "log.h"

uint32_t process_ntags(struct id3_tag *t)
{
	uint32_t count = 0;
	for(unsigned int i = 0; i<t->nframes; i++){
		struct id3_frame *frame = t->frames[i];
		for(unsigned int j = 0; j<frame->nfields; j++){
			union id3_field field = frame->fields[j];
			switch(field.type){
			//Not parsed
			case ID3_FIELD_TYPE_BINARYDATA:
			case ID3_FIELD_TYPE_FRAMEID:
			case ID3_FIELD_TYPE_TEXTENCODING:
				break;
			//Possibly multiple items
			case ID3_FIELD_TYPE_LATIN1LIST:
				count += field.latin1list.nstrings;
				break;
			case ID3_FIELD_TYPE_STRINGLIST:
				count += field.stringlist.nstrings;
				break;
			//All others are single items
			default:
				count++;
				break;
			}
		}
	}
	return count;
}

void process_frame(struct id3_frame *fr, char **ks, char **vs, uint32_t *ti)
{
	uint32_t oldti = *ti;

	//If it is a user tag, we assume the first value to be the tagname
	unsigned int st = strcmp(fr->id, "TXXX") == 0 ? 1 : 0;
	char *key = NULL;

	//Extract fields
	for(unsigned int i = 0; i<fr->nfields; i++){
		union id3_field f = fr->fields[i];
		switch(f.type){
		//Skip
		case ID3_FIELD_TYPE_BINARYDATA:
		case ID3_FIELD_TYPE_TEXTENCODING:
		case ID3_FIELD_TYPE_FRAMEID:
			break;
		case ID3_FIELD_TYPE_LATIN1:
		case ID3_FIELD_TYPE_LATIN1FULL:
			if(st == 1 && key == NULL)
				key = safe_strdup((char *)f.latin1.ptr);
			else
				vs[(*ti)++] = safe_strdup((char *)f.latin1.ptr);
			break;
		case ID3_FIELD_TYPE_LATIN1LIST:
			if(st == 1)
				key = safe_strdup(
					(char *)f.latin1list.strings[0]);
			for(unsigned int j = 0; j<f.latin1list.nstrings; j++)
				vs[(*ti)++] = safe_strdup(
					(char *)f.latin1list.strings[j]);
			break;
		case ID3_FIELD_TYPE_STRING:
		case ID3_FIELD_TYPE_STRINGFULL:
			if(st == 1 && key == NULL)
				key = (char *)id3_ucs4_utf8duplicate(
					f.string.ptr);
			else
				vs[(*ti)++] = (char *)id3_ucs4_utf8duplicate(
					f.string.ptr);
			break;
		case ID3_FIELD_TYPE_STRINGLIST:
			if(st == 1)
				key = (char *)id3_ucs4_utf8duplicate(
					f.stringlist.strings[0]);
			for(unsigned int j = 0; j<f.stringlist.nstrings; j++)
				vs[(*ti)++] = (char *)id3_ucs4_utf8duplicate(
					f.stringlist.strings[j]);
			break;
		case ID3_FIELD_TYPE_LANGUAGE:
		case ID3_FIELD_TYPE_DATE:
			vs[(*ti)++] = safe_strdup(f.immediate.value);
			break;
		case ID3_FIELD_TYPE_INT8:
		case ID3_FIELD_TYPE_INT16:
		case ID3_FIELD_TYPE_INT24:
		case ID3_FIELD_TYPE_INT32:
		case ID3_FIELD_TYPE_INT32PLUS:
			vs[*ti] = safe_malloc(25);
			sprintf(vs[(*ti)++], "%ld", f.number.value);
			break;
		}
	}

	//Determine frame type
	//If not a user tag, we look it up
	if(key == NULL){
		//Lookup key
		key = safe_strdup(id3map_get(fr->id));
		logmsg(debug, "Found lookup from id3map: %s\n", key);
	}
	for(uint32_t i = oldti; i<*ti; i++){
		ks[i] = safe_strdup(key);
	}
	free(key);
}

bool process_mp3(char *p, struct db_file *f)
{
	struct id3_file *mf = id3_file_open(p, ID3_FILE_MODE_READONLY);
	if(mf == NULL)
		return false;

	struct id3_tag *mt = id3_file_tag(mf);
	logmsg(info, "Found %lu tags\n", process_ntags(mt));
	file_tag_init(f, process_ntags(mt));

	uint32_t tagindex = 0;
	for(unsigned int i = 0; i<mt->nframes; i++){
		process_frame(mt->frames[i],
			f->tags->values, f->tags->keys, &tagindex);
	}
	logmsg(info, "Really only written up to %d tags\n", tagindex-1);
	f->tags->ntags = tagindex;
	f->tags->keys = safe_realloc(f->tags->keys,
		f->tags->ntags*sizeof(char *));
	f->tags->values = safe_realloc(f->tags->values,
		f->tags->ntags*sizeof(char *));
	return id3_file_close(mf) == 0;
}
