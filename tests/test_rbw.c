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


/********* Test RingBufferWindow Class **********/

#define RBWSIZE 10
RingBufferWindow rbw_common;

void rbw_setup(void){
    rbw_init(&rbw_common, RBWSIZE);

    
}

void rbw_teardown(void){
    free(rbw_common);
}

START_TEST (test_ringbuffer)
{
    // Test init class
    ck_assert_int_eq(rbw_common->win_head, 0);
    ck_assert_int_eq(sizeof(rbw_common->buffer), BUFFSIZE * 8);
    ck_assert_int_eq(rbw_common->win_size, RBWSIZE);

    for(int i=0; i < BUFFSIZE; i++){
        ck_assert_int_eq(rbw_common->buffer[i]->seq_num, i);
    }

    // Test rbw_get_packet_n
    GBNPacket p1, p2, p3;
    p1 = rbw_get_packet_n(rbw_common, 5);
    p2 = rbw_get_packet_n(rbw_common, 5);
    ck_assert(p1 == p2);
    p2 = rbw_get_packet_n(rbw_common, 0);
    ck_assert(rbw_common->win_head == p2->seq_num);

    // Test rbw_get_next_packet
    p1 = rbw_get_packet_n(rbw_common, 0);
    p2 = rbw_get_packet_n(rbw_common, 1);
    p3 = rbw_get_next_packet(rbw_common, p1);
    ck_assert(p3 == p2);

    // Test rbw_set_win_size
    rbw_set_win_size(rbw_common, 0);
    ck_assert_int_eq(rbw_common->win_size, 0);

    rbw_set_win_size(rbw_common, -1);
    ck_assert_int_eq(rbw_common->win_size, 0);

    rbw_set_win_size(rbw_common, 5);
    ck_assert_int_eq(rbw_common->win_size, 5);

    rbw_set_win_size(rbw_common, BUFFSIZE / 2);
    ck_assert_int_eq(rbw_common->win_size, 5);
}
END_TEST

Suite * test_ringbuff_create(){
    Suite *s = suite_create("Ringbuffer Class");
    TCase *tc_core = tcase_create("Core");
    tcase_add_checked_fixture (tc_core, rbw_setup, rbw_teardown);
    tcase_add_test(tc_core, test_ringbuffer);
    suite_add_tcase(s, tc_core);

    return s;
}


int main(void){
    int number_failed;
    SRunner *rbfr = srunner_create(test_ringbuffunc_create());
    srunner_add_suite(rbfr, test_ringbuff_create());
    srunner_run_all(rbfr, CK_NORMAL);
    number_failed = srunner_ntests_failed(rbfr);
    srunner_free(rbfr);

    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
