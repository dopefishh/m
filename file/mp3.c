#include <stdbool.h>

#include <id3tag.h>

#include "../config.h"
#include "../file.h"
#include "../util.h"
#include "../db.h"
#include "../log.h"
#include "../id3map.h"

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

char *latin1_to_utf8(unsigned char *l)
{
	uint32_t len = 0;
	for(uint32_t i = 0; i<strlen((char *)l); i++){
		len++;
		if(l[i] >= 0x80)
			len++;
	}
	char *r = safe_malloc(len+1);
	uint32_t j = 0;
	for(uint32_t i = 0; i<len; i++){
		if(l[i] < 0x80){
			r[j++] = l[i];
		} else {
			r[j++] = 0xc0 | (l[i] & 0xc0) >> 6;
			r[j++] = 0x80 | (l[i] & 0x3f);
		}
	}
	r[len] = '\0';
	return r;
}

void process_frame(struct id3_frame *fr, struct db_tag *tags, uint32_t *ti)
{
	uint32_t oldti = *ti;

	//If it is a user tag, we assume the first value to be the tagname
	bool st = strcmp(fr->id, "TXXX") == 0;
	char *oldkey = NULL, *key = NULL;

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
			if(st && key == NULL)
				key = latin1_to_utf8(f.latin1.ptr);
			else
				tags[(*ti)++].value
					= latin1_to_utf8(f.latin1.ptr);
			break;
		case ID3_FIELD_TYPE_LATIN1LIST:
			if(st)
				key = latin1_to_utf8(f.latin1list.strings[0]);
			for(unsigned int j = 0; j<f.latin1list.nstrings; j++)
				tags[(*ti)++].value = latin1_to_utf8(
					f.latin1list.strings[j]);
			break;
		case ID3_FIELD_TYPE_STRING:
		case ID3_FIELD_TYPE_STRINGFULL:
			if(st && key == NULL)
				key = (char *)id3_ucs4_utf8duplicate(
					f.string.ptr);
			else
				tags[(*ti)++].value
					= (char *)id3_ucs4_utf8duplicate(
						f.string.ptr);
			break;
		case ID3_FIELD_TYPE_STRINGLIST:
			if(st)
				key = (char *)id3_ucs4_utf8duplicate(
					f.stringlist.strings[0]);
			for(unsigned int j = 0; j<f.stringlist.nstrings; j++)
				tags[(*ti)++].value
					= (char *)id3_ucs4_utf8duplicate(
						f.stringlist.strings[j]);
			break;
		case ID3_FIELD_TYPE_LANGUAGE:
		case ID3_FIELD_TYPE_DATE:
			tags[(*ti)++].value = safe_strdup(f.immediate.value);
			break;
		case ID3_FIELD_TYPE_INT8:
		case ID3_FIELD_TYPE_INT16:
		case ID3_FIELD_TYPE_INT24:
		case ID3_FIELD_TYPE_INT32:
		case ID3_FIELD_TYPE_INT32PLUS:
			tags[*ti].value = safe_malloc(25);
			sprintf(tags[(*ti)++].value, "%ld", f.number.value);
			break;
		}
	}

	//Lookup key
	oldkey = key;
	key = safe_strdup(id3map_get(fr->id, st ? key : NULL));
	free(oldkey);
//	logmsg(debug, "Found lookup from id3map: %s\n", key);

	for(uint32_t i = oldti; i<*ti; i++)
		tags[i].key = safe_strdup(key);
	free(key);
}

bool process_mp3(char *p, struct db_file *f)
{
	struct id3_file *mf = id3_file_open(p, ID3_FILE_MODE_READONLY);
	if(mf == NULL)
		return false;

	struct id3_tag *mt = id3_file_tag(mf);
	file_tag_init(f, process_ntags(mt));

	uint32_t tagindex = 0;
	for(unsigned int i = 0; i<mt->nframes; i++)
		process_frame(mt->frames[i], f->tags, &tagindex);
	f->ntags = tagindex;
	f->tags = safe_realloc(f->tags, f->ntags*sizeof(struct db_tag));
	return id3_file_close(mf) == 0;
}
