#include "gtest/gtest.h"

#include "convert.h"

TEST(ConvertTest, Simple) {
  EXPECT_LE('p', TriToCode("END"));
  EXPECT_EQ('a', TriToCode("ACG"));
}

TEST(ConvertTest, Reverse) {
  for (char c = 'a'; c < TriToCode("END"); ++c) {
    const char *tri = CodeToTri(c);
    EXPECT_EQ(3, strlen(tri));
    EXPECT_EQ(c, TriToCode(tri));
  }
}
