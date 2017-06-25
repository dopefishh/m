#include <stdio.h>
#include <stdint.h>

#include "util.h"

uint64_t parse_int64(FILE *f)
{
	uint64_t b;
	if(fread((void *)&b, 8, 1, f) != 1)
		die("Expected 64bit Integer");
	return b;
}

void write_int64(FILE *f, uint64_t i)
{
	if(fwrite((void *)&i, 8, 1, f) != 1)
		die("Unable to write 64bit Integer");
}

char *get_line(FILE *f)
{
	size_t total = 16, read = 0;
	char *b = NULL;
	while(true){
		b = (char *)safe_realloc(b, total*=2);
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
