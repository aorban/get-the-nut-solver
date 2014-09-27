#include "gtest/gtest.h"

#include "convert.h"
#include "rules.h"

#include <string>
#include <vector>

using namespace std;

#define TILE(X) (TriToCode(X) - 'a')

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

static inline int MY_intcmp(const void *aa, const void *bb) {
  return ( *(int*)aa - *(int*)bb );
}

TEST(ActionTest, Sortint) {
  static const int N = 200;
  srand(117);
  Action actions[N];
  for (int i = 0; i < N; ++i) {
    int *p = (int *)&actions[i];
    *p = rand() + rand() + rand();
    actions[i].exists = 1;
  }
  qsort((void *)actions, N, sizeof(int), MY_intcmp);
  for (int i = 1; i < N; ++i) {
    EXPECT_GE(actions[i].prio, actions[i-1].prio);
  }
  EXPECT_EQ(0, actions[0].prio);
  EXPECT_EQ(3, actions[N-1].prio);
}

TEST(RulesTest, Static) {
  const int a = Rules::NUM_ANIMALS;
  EXPECT_GE(a, TriToCode("END") - 'a');
}

TEST(RulesTest, Default) {
  Rules r;
  {
    Action a = r.GetAction(TILE("SQR"), TILE("HOG"), Rules::ON);
    EXPECT_EQ(0, *((int*)&a));
    EXPECT_FALSE(a.exists);
  }
  {
    Action a = r.GetAction(TILE("SQR"), TILE("HOG"), Rules::AHEAD);
    EXPECT_TRUE(a.exists);
    EXPECT_EQ(TILE("SQR"), a.moving_new_animal);
    EXPECT_EQ(TILE("HOG"), a.static_new_animal);
    EXPECT_FALSE(a.moving_animal_dies);
    EXPECT_FALSE(a.static_animal_dies);
    EXPECT_FALSE(a.won);
    EXPECT_FALSE(a.lost);
    EXPECT_FALSE(a.continues);
    EXPECT_EQ(3, a.prio);
  }
  {
    Action a = r.GetAction(TILE("SQR"), TILE("SWM"), Rules::AHEAD);
    EXPECT_EQ(0, *((int*)&a));
    EXPECT_FALSE(a.exists);
  }
  {
    // This makes no sense, the swamp is never moving.
    Action a = r.GetAction(TILE("SWM"), TILE("HOG"), Rules::AHEAD);
    EXPECT_TRUE(a.exists);
    EXPECT_EQ(TILE("SWM"), a.moving_new_animal);
    EXPECT_EQ(TILE("HOG"), a.static_new_animal);
    EXPECT_FALSE(a.moving_animal_dies);
    EXPECT_FALSE(a.static_animal_dies);
    EXPECT_FALSE(a.won);
    EXPECT_FALSE(a.lost);
    EXPECT_FALSE(a.continues);
    EXPECT_EQ(3, a.prio);
  }
}

class RealRulesTest : public ::testing::Test {
 public:
   static void SetUpTestCase() {
     r = new Rules("rules-real.csv");
   }
   static void TearDownTestCase() {
     delete r;
   }
 protected:
  static Rules *r;
};
Rules *RealRulesTest::r;

TEST_F(RealRulesTest, Hog) {
  {
    // Hogs pass each other.
    Action a = r->GetAction(TILE("HOG"), TILE("HOG"), Rules::SIDE);
    EXPECT_EQ(0, *((int*)&a));
    EXPECT_FALSE(a.exists);
  }
  {
    // Hog eats squirell if it runs into it.
    Action a = r->GetAction(TILE("SQR"), TILE("HOG"), Rules::AHEAD);
    EXPECT_TRUE(a.exists);
    EXPECT_TRUE(a.lost);
  }
  {
    // Also from the side.
    Action a = r->GetAction(TILE("SQR"), TILE("HOG"), Rules::SIDE);
    EXPECT_TRUE(a.exists);
    EXPECT_TRUE(a.lost);
  }
}

