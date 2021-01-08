#include <loadCellAmp.h>
#include <driver/gpio.h>
#include "unity.h"

TEST_CASE("Test if test framework is working correctly", ""){
  TEST_ASSERT(1);
}

TEST_CASE("RAII - Destruction means resource resignation", ""){
	LoadCellAmp loadCellAmp(GPIO_NUM_16, GPIO_NUM_17);
}

