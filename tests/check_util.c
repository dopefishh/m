#include <check.h>

#include "../src/util.h"

#define str_eq_msg(a,b) ck_assert_msg(strcmp(a, b) == 0, "strcmp(\"%s\", \"%s\") != 0", a, b);

START_TEST(test_util_safe_strcat)
{
	str_eq_msg("", safe_strcat(0));
	str_eq_msg("", safe_strcat(1, ""));
	str_eq_msg("", safe_strcat(0, ""));
	str_eq_msg("a", safe_strcat(1, "a"));
	str_eq_msg("aab", safe_strcat(2, "a", "ab"));
}
END_TEST

START_TEST(test_util_trim)
{
	str_eq_msg("",    trim(safe_strdup("")));
	str_eq_msg("",    trim(safe_strdup(" ")));
	str_eq_msg("",    trim(safe_strdup("\t")));
	str_eq_msg("",    trim(safe_strdup("\n")));
	str_eq_msg("abc", trim(safe_strdup(" abc")));
	str_eq_msg("abc", trim(safe_strdup("abc ")));
	str_eq_msg("abc", trim(safe_strdup(" abc ")));
	str_eq_msg("abc", trim(safe_strdup("  abc")));
	str_eq_msg("abc", trim(safe_strdup("abc  ")));
	str_eq_msg("abc", trim(safe_strdup("  abc  ")));
}
END_TEST

Suite *util_suite(void)
{
	Suite *s = suite_create("Util");

	TCase *tc_strcat = tcase_create("Strcat");
	tcase_add_test(tc_strcat, test_util_safe_strcat);
	suite_add_tcase(s, tc_strcat);

	TCase *tc_trim = tcase_create("Trim");
	tcase_add_test(tc_trim, test_util_trim);
	suite_add_tcase(s, tc_trim);

	return s;
}

int main(void)
{
	int failed;
	Suite *s;
	SRunner *sr;

	s = util_suite();
	sr = srunner_create(s);

	srunner_run_all(sr, CK_NORMAL);
	failed = srunner_ntests_failed(sr);
	srunner_free(sr);
	return (failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
//
//#define TEST(name, str, targ, fn, nl) {\
//	FILE *_f = fopen(fn, "w");\
//	fputs(str, _f);\
//	fclose(_f);\
//	_f = fopen(fn, "r");\
//	char *_r = get_line(_f, nl);\
//	if(strcmp(_r, targ) != 0)\
//		die("util:get_line(%s, %s) failed got '%s' expected '%s'\n", name, nl ? "true" : "false", _r, targ);\
//	free(_r);\
//	fclose(_f);\
//}
//
//int main(void)
//{
//	char fn[] = "/tmp/m.XXXXXX";
//	int fd;
//	if((fd = mkstemp(fn)) == -1)
//		perrordie("mkstemp");
//	if(close(fd) == -1)
//		perrordie("close");
//
//	//empty line
//	TEST("eof", "", "", fn, true);
//	TEST("eof", "", "", fn, false);
//	TEST("abc_eof", "abc", "abc", fn, true);
//	TEST("abc_eof", "abc", "abc", fn, false);
//	TEST("nl", "\n", "", fn, true);
//	TEST("nl", "\n", "", fn, false);
//	TEST("nl_bog", "\nabc", "", fn, true);
//	TEST("nl_bog", "\nabc", "", fn, false);
//	TEST("bog_nl", "abc\n", "abc", fn, true);
//	TEST("bog_nl", "abc\n", "abc", fn, false);
//	TEST("bog_nl_bog", "abc\ndef", "abc", fn, true);
//	TEST("bog_nl_bog", "abc\ndef", "abc", fn, false);
//	TEST("bog_nle_bog_eof", "abc\\\ndef", "abc\ndef", fn, true);
//	TEST("bog_nle_bog_eof", "abc\\\ndef", "abcdef", fn, false);
//	TEST("bog_nle_bog_nl", "abc\\\ndef\n", "abc\ndef", fn, true);
//	TEST("bog_nle_bog_nl", "abc\\\ndef\n", "abcdef", fn, false);
//	TEST("bog_nle_bog_nl_bog", "abc\\\ndef\nghi", "abc\ndef", fn, true);
//	TEST("bog_nle_bog_nl_bog", "abc\\\ndef\nghi", "abcdef", fn, false);
//
//	if(unlink(fn) == -1)
//		perrordie("unlink");
//	return 0;
//}
