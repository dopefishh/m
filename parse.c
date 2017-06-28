#include <stdio.h>
#include <stdint.h>

#include "util.h"

uint64_t parse_int64(FILE *f)
{
	uint64_t b;
	if(safe_fread((void *)&b, 8, 1, f) != 1)
		die("Expected 64bit Integer");
	return b;
}
char *parse_string(FILE *f)
{
	size_t total = 16, read = 0;
	char *b = (char *)safe_malloc(total);
	while(b[read] != '\0'){
		if(read >= total)
			b = (char *)safe_realloc(b, total*=2);
		b[read++] = safe_getc(f);
		if(b[read] == EOF)
			die("Expected string");
	}
	return b;
}

void write_int64(FILE *f, uint64_t i)
{
	if(safe_fwrite((void *)&i, 8, 1, f) != 1)
		die("Unable to write 64bit Integer");
}

void write_string(FILE *f, char *s)
{
	if(safe_fwrite(s, strlen(s)+1, 1, f) != 1)
		die("Unable to write string");
}

char *get_line(FILE *f)
{
	size_t total = 16, read = 0;
	char *b = NULL;
	while(true){
		b = (char *)safe_realloc(b, total*=2);
		b = b;
		safe_fgets(b+read, total-read, f);
		read = strlen(b);
		if(feof(f)){
			break;
		}
		if(b[read-1] == '\n'){
			if(read > 1 && b[read-2] == '\\'){
				b[read-1] = '\0';
				b[read-2] = '\0';
				read -= 2;
			} else {
				b[read-1] = '\0';
				break;
			}
		}
	}
	return b;
}
