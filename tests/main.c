#include "unity.h"
#include <stdio.h>
#include "utils.h"
#include "ucmd.h"

extern void test_utils_asbytes(void);
extern void test_utils_findch(void);
extern void test_strtou32(void);
extern void test_strtoi32(void);

extern void test__get_param(void);
extern void test__get_cmdinfo(void);
extern void test__parse_string(void);
extern void test__get_arg(void);
extern void test_cmd(void);

void setUp(void){}
void tearDown(void){}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_utils_asbytes);
  RUN_TEST(test_utils_findch);
  RUN_TEST(test_strtou32);
  RUN_TEST(test_strtoi32);
  RUN_TEST(test__get_param);
  RUN_TEST(test__get_cmdinfo);
  RUN_TEST(test__parse_string);
  RUN_TEST(test__get_arg);
  RUN_TEST(test_cmd);
  return UNITY_END();
}
