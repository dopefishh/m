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

START_TEST(test_util_rtrimc)
{
	str_eq_msg("",   rtrimc(safe_strdup(""), ' '));
	str_eq_msg("",   rtrimc(safe_strdup(""), '\0'));
	str_eq_msg("",   rtrimc(safe_strdup(""), 'c'));
	str_eq_msg("ab", rtrimc(safe_strdup("abc"), 'c'));
	str_eq_msg("ab", rtrimc(safe_strdup("abcc"), 'c'));
	str_eq_msg("ab", rtrimc(safe_strdup("abccc"), 'c'));
	str_eq_msg("abcd", rtrimc(safe_strdup("abcdc"), 'c'));

}
END_TEST

#define TEST_GET_LINE(str, targ, fn, nl) {\
	FILE *_f = fopen(fn, "w");\
	fputs(str, _f);\
	fclose(_f);\
	_f = fopen(fn, "r");\
	char *_r = get_line(_f, nl);\
	ck_assert_msg(strcmp(_r, targ) == 0, "get_line(nl=%s) failed got '%s' expected '%s'\n", nl ? "true" : "false", _r, targ);\
	free(_r);\
	fclose(_f);\
}
START_TEST(test_util_get_line)
{
	char fn[] = "/tmp/m.XXXXXX";
	int fd;
	if((fd = mkstemp(fn)) == -1)
		perrordie("mkstemp");
	if(close(fd) == -1)
		perrordie("close");

	//empty line
	TEST_GET_LINE("", "", fn, true);
	TEST_GET_LINE("", "", fn, false);
	TEST_GET_LINE("abc", "abc", fn, true);
	TEST_GET_LINE("abc", "abc", fn, false);
	TEST_GET_LINE("\n", "", fn, true);
	TEST_GET_LINE("\n", "", fn, false);
	TEST_GET_LINE("\nabc", "", fn, true);
	TEST_GET_LINE("\nabc", "", fn, false);
	TEST_GET_LINE("abc\n", "abc", fn, true);
	TEST_GET_LINE("abc\n", "abc", fn, false);
	TEST_GET_LINE("abc\ndef", "abc", fn, true);
	TEST_GET_LINE("abc\ndef", "abc", fn, false);
	TEST_GET_LINE("abc\\\ndef", "abc\ndef", fn, true);
	TEST_GET_LINE("abc\\\ndef", "abcdef", fn, false);
	TEST_GET_LINE("abc\\\ndef\n", "abc\ndef", fn, true);
	TEST_GET_LINE("abc\\\ndef\n", "abcdef", fn, false);
	TEST_GET_LINE("abc\\\ndef\nghi", "abc\ndef", fn, true);
	TEST_GET_LINE("abc\\\ndef\nghi", "abcdef", fn, false);

	if(unlink(fn) == -1)
		perrordie("unlink");
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

	TCase *tc_rtrimc = tcase_create("Rtrimc");
	tcase_add_test(tc_rtrimc, test_util_rtrimc);
	suite_add_tcase(s, tc_rtrimc);

	TCase *tc_get_line = tcase_create("Get_line");
	tcase_add_test(tc_rtrimc, test_util_get_line);
	suite_add_tcase(s, tc_get_line);

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
//
//int main(void)
//{
//	return 0;
//}
