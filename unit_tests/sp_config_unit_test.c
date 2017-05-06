#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "unit_test_util.h" //SUPPORTING MACROS ASSERT_TRUE/ASSERT_FALSE etc..
#include "../SPConfig.h"



static bool goodConfigTest() {
	SP_CONFIG_MSG msg;
	spConfigCreate("./unit_tests/goodconfig.config", &msg);
    ASSERT_TRUE(msg == SP_CONFIG_SUCCESS);
	return true;
}

static bool badConfigTest() {
	SP_CONFIG_MSG msg;
	spConfigCreate("./unit_tests/badconfig.config", &msg);
    ASSERT_TRUE(msg != SP_CONFIG_SUCCESS);
	return true;
}

int main() {
	RUN_TEST(goodConfigTest);
	RUN_TEST(badConfigTest);
	return 0;
}
