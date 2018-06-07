#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>

#include "../src/util.h"
#include "../src/log.h"

#define TEST(name, str, targ, fn, nl) {\
	FILE *_f = fopen(fn, "w");\
	fputs(str, _f);\
	fclose(_f);\
	_f = fopen(fn, "r");\
	char *_r = get_line(_f, nl);\
	if(strcmp(_r, targ) != 0)\
		die("util:get_line(%s, %s) failed got '%s' expected '%s'\n", name, nl ? "true" : "false", _r, targ);\
	free(_r);\
	fclose(_f);\
}

int main(void)
{
	char fn[] = "/tmp/m.XXXXXX";
	int fd;
	if((fd = mkstemp(fn)) == -1)
		perrordie("mkstemp");
	if(close(fd) == -1)
		perrordie("close");

	//empty line
	TEST("eof", "", "", fn, true);
	TEST("eof", "", "", fn, false);
	TEST("abc_eof", "abc", "abc", fn, true);
	TEST("abc_eof", "abc", "abc", fn, false);
	TEST("nl", "\n", "", fn, true);
	TEST("nl", "\n", "", fn, false);
	TEST("nl_bog", "\nabc", "", fn, true);
	TEST("nl_bog", "\nabc", "", fn, false);
	TEST("bog_nl", "abc\n", "abc", fn, true);
	TEST("bog_nl", "abc\n", "abc", fn, false);
	TEST("bog_nl_bog", "abc\ndef", "abc", fn, true);
	TEST("bog_nl_bog", "abc\ndef", "abc", fn, false);
	TEST("bog_nle_bog_eof", "abc\\\ndef", "abc\ndef", fn, true);
	TEST("bog_nle_bog_eof", "abc\\\ndef", "abcdef", fn, false);
	TEST("bog_nle_bog_nl", "abc\\\ndef\n", "abc\ndef", fn, true);
	TEST("bog_nle_bog_nl", "abc\\\ndef\n", "abcdef", fn, false);
	TEST("bog_nle_bog_nl_bog", "abc\\\ndef\nghi", "abc\ndef", fn, true);
	TEST("bog_nle_bog_nl_bog", "abc\\\ndef\nghi", "abcdef", fn, false);

	if(unlink(fn) == -1)
		perrordie("unlink");
	return 0;
}
