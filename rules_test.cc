#include "gtest/gtest.h"

#include "rules.h"

#include <string>
#include <vector>

using namespace std;

TEST(SplitTest, Space) {
  vector<string> r;
  SplitString("This is a sentence.", " ", &r);
  ASSERT_EQ(4, r.size());
  EXPECT_EQ("This", r[0]);
  EXPECT_EQ("sentence.", r[3]);
}

TEST(SplitTest, Comma) {
  vector<string> r;
  SplitString("Alpha, beta,   gamma,", ",", &r);
  ASSERT_EQ(4, r.size());
  EXPECT_EQ("Alpha", r[0]);
  EXPECT_EQ("   gamma", r[2]);
  EXPECT_EQ("", r[3]);
}

TEST(RulesTest, Real) {
  Rules r("rules-real.csv");
}

