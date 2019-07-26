
#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include <wrappy/wrappy.h>

#include "stdlib.cpp"
#include "sugar.cpp"

int main(int argc, char *argv[]) {
  ::testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}
