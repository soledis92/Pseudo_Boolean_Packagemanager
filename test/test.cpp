#include "gtest/gtest.h"
#include "addieren.h"

TEST(mocktest, mocking)
{
  EXPECT_EQ(1, 1);
}

TEST(add, positive)
{
    Addieren addClassIntance = Addieren();
    int a = addClassIntance.add(3,2);
    EXPECT_EQ(5, a);
}

