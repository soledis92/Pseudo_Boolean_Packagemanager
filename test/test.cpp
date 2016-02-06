#include "gtest/gtest.h"
#include "addieren.h"

TEST(mocktest, mocking)
{
  EXPECT_EQ(1, 1);
}

TEST(add,positive)
{
    Addieren addClassIntance = Addieren();
    EXPECT_EQ(5, addClassIntance.add(3,2));
}

