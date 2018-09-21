#include <check.h>

#include "../src/util.h"
#include "../src/list.h"

bool find_eq(void *a, void *b)
{
	return a == b;
}

START_TEST(test_list)
{
	struct listitem *head = NULL, *last;
	ck_assert_msg(list_length(head) == 0, "empty list has length 0");

	//Prepend
	head = list_prepend(head, (void *)42);
	ck_assert_msg(list_length(head) == 1, "1 prepend length 1");

	head = list_prepend(head, (void *)43);
	ck_assert_msg(list_length(head) == 2, "2 prepend length 2");

	head = list_prepend(head, (void *)44);
	ck_assert_msg(list_length(head) == 3, "3 prepend length 3");

	list_free(head, &list_free_ignore);

	//Append
	head = NULL;
	head = list_append(head, (void *)42);
	ck_assert_msg(list_length(head) == 1, "1 append length 1");

	last = list_append(head, (void *)43);
	ck_assert_msg(list_length(head) == 2, "2 append length 2");
	ck_assert_msg(list_length(last) == 1, "2 append tail length 1");

	last = list_append(head, (void *)44);
	ck_assert_msg(list_length(head) == 3, "3 append length 3");
	ck_assert_msg(list_length(last) == 1, "3 append tail length 1");

	//Find
	uint32_t *idx;
	void *data = list_find(head, (void *)42, &find_eq, idx);

//void  *list_find(struct listitem *, bool(*)(void *), uint32_t *);
//struct listitem *list_delete(struct listitem *, uint32_t, void **);
//size_t list_length(struct listitem *);
//void   list_iterate(struct listitem *, void (*)(void *));
//void   list_free(struct listitem *, void(*)(void *));
//void   list_free_ignore(void *);
}
END_TEST

Suite *util_suite(void)
{
	Suite *s = suite_create("List");

	TCase *tc_list = tcase_create("Strcat");
	tcase_add_test(tc_list, test_list);
	suite_add_tcase(s, tc_list);

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
