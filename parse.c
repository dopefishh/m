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
