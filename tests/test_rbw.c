#include <stdlib.h>
#include <check.h>

#include "../src/ringbufferwindow.h"

START_TEST (test_ringbuffunc)
{
    //Test get_seq_num
    ck_assert_int_eq(get_seq_num(5), 5);
    ck_assert_int_eq(get_seq_num(-1), BUFFSIZE -1);
    ck_assert_int_eq(get_seq_num(BUFFSIZE + 12), 12);

    //Test get_seq_diff
    ck_assert_int_eq(get_seq_diff(15, 20), 5);
    ck_assert_int_eq(get_seq_diff(20,15), BUFFSIZE - 5);
    ck_assert_int_eq(get_seq_diff(BUFFSIZE -5, 1), 6);

    //Test seq_add
    ck_assert_int_eq(seq_add(1, 6), 7);
    ck_assert_int_eq(seq_add(BUFFSIZE - 5, 10), 5);
    ck_assert_int_eq(seq_add(-5, 5), 0);
    ck_assert_int_eq(seq_add(1, 6), 7);

}
END_TEST

Suite * test_ringbuffunc_create(){
    Suite *s = suite_create("Ringbuffer Functions");
    TCase *tc_core = tcase_create("Core");
    tcase_add_test(tc_core, test_ringbuffunc);
    suite_add_tcase(s, tc_core);

    return s;
}

int main(void){
    int number_failed;
    Suite *rbf = test_ringbuffunc_create();
    SRunner *rbfr = srunner_create(rbf);
    srunner_run_all(rbfr, CK_NORMAL);
    number_failed = srunner_ntests_failed(rbfr);
    srunner_free(rbfr);

    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
