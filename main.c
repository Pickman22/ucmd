#include <stdio.h>
#include "utils.h"
#include "cmd.h"

extern void test_utils_asbytes(void);
extern void test_utils_findch(void);
extern void test_strtou32(void);
extern void test_strtoi32(void);

extern void test__get_param(void);
extern void test__get_cmdinfo(void);
extern void test__parse_string(void);
extern void test__get_arg(void);
extern void test_cmd(void);


int main(void) {
	printf("Testing 'utils'...\n\r");
	test_utils_asbytes();
	test_utils_findch();
	test_strtou32();
	test_strtoi32();
	printf("Sucess!\n\r\n\r");

	printf("Testing 'cmd'...\n\r");
	test__get_param();
	test__get_cmdinfo();
	test__get_arg();
	test__parse_string();
   test_cmd();
	printf("Sucess!\n\r\n\r");
	printf("All tests passed. Pres any key to exit.\n\r\n\r");
	getchar();
}
