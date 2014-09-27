#include <iostream>
#include <map>

#include "convert.h"
#include "board.h"
#include "rules.h"

#include "gtest/gtest.h"

class MockRules : public Rules {
 public:
  MockRules() : Rules() {
    // b does nothing with a, just stops it.
    // This is default, so we don't need to add.
    {
      // c kills a if a moves near it.
      int a1 = 'a' - 'a';
      int a2 = 'c' - 'a';
      Action a;
      a.moving_animal_dies = 1;
      a.static_animal_dies = 0;
      a.moving_new_animal = a1;
      a.static_new_animal = a2;
      a.won = 0;
      a.lost = 0;
      a.continues = 0;
      a.prio = 2;
      a.exists = 1;
      for (int r = 0; r < NUM_RELATIONS; ++r) {
        rules[a1][a2][r] = a;
      }
    }
    {
      // d turns 'a' into 'e'
      int a1 = 'a' - 'a';
      int a2 = 'd' - 'a';
      Action a;
      a.moving_animal_dies = 0;
      a.static_animal_dies = 0;
      a.moving_new_animal = 'e' - 'a';
      a.static_new_animal = a2;
      a.won = 0;
      a.lost = 0;
      a.continues = 0;
      a.prio = 2;
      a.exists = 1;
      for (int r = 0; r < NUM_RELATIONS; ++r) {
        rules[a1][a2][r] = a;
      }
    }
  }
};

static const Rules *RULES = new MockRules;

static const char B001[] =
    "##########"
    "#        #"
    "#        #"
    "#        #"
    "#        #"
    "#        #"
    "#        #"
    "##########";

static const char B002[] =
    "##########"
    "#        #"
    "#        #"
    "#      # #"
    "#        #"
    "#  #     #"
    "#        #"
    "##########";

static const char B003[] =
    "##########"
    "#    c c #"
    "#        #"
    "#   b  # #"
    "# b  a   #"
    "#  #   c #"
    "#    #   #"
    "##########";

static const char B004[] =
    "##########"
    "#     abc#"
    "#     abc#"
    "#      # #"
    "#        #"
    "#  #     #"
    "# abc    #"
    "##########";

static const char B005[] =
    "##########"
    "#    d   #"
    "#      c #"
    "#   b  # #"
    "# b  a   #"
    "#  #   c #"
    "#    #   #"
    "##########";


////////////////////////////////////////////////////////////////////////////////
// Board
////////////////////////////////////////////////////////////////////////////////

TEST(TestBoard, Construct) {
  Board b001(B001, *RULES);
  Board b002(B002, *RULES);
  Board b003(B003, *RULES);
  Board b004(B004, *RULES);
}

TEST(TestBoard, DebugString) {
  Board b(B004, *RULES);
  State s(B004);
  EXPECT_EQ(
    "##########\n"
    "#        #\n"                   
    "#        #\n"                   
    "#      # #\n"                   
    "#        #\n"                   
    "#  #     #\n"                   
    "#        #\n"                   
    "##########\n", b.DebugString());
  EXPECT_EQ(
    "##########\n"
    "#     abc#\n"                   
    "#     abc#\n"                   
    "#      # #\n"                   
    "#        #\n"                   
    "#  #     #\n"                   
    "# abc    #\n"                   
    "##########\n", b.DebugStringWithState(s));
}

// TEST(TestBoard, MinMovesFromSimple) {
//   Board b003(B003);
//   State s003(B003);
//   EXPECT_EQ(2, b003.MinMovesFrom(s003));
//   {
//     State n(b003, s003, State::DOWN);
//     EXPECT_EQ(3, b003.MinMovesFrom(n));
//   }
//   {
//     State n(b003, s003, State::RIGHT);
//     EXPECT_EQ(3, b003.MinMovesFrom(n));
//   }
//   {
//     State n(b003, s003, State::LEFT);
//     EXPECT_EQ(1, b003.MinMovesFrom(n));
//     State n2(b003, n, State::LEFT);
//     EXPECT_EQ(0, b003.MinMovesFrom(n2));
//   }
// }

// TEST(TestBoard, MinMovesFromComplex) {
//   {
//     Board b(B008);
//     State s(B008);
//     EXPECT_EQ(8, b.MinMovesFrom(s));
//   }
// }

////////////////////////////////////////////////////////////////////////////////
// State
////////////////////////////////////////////////////////////////////////////////

class TestableState : public State {
 public:
  explicit TestableState(const char *i) : State(i) {}
  using State::Find;
  using State::Sort;
  using State::Erase;
};

TEST(TestState, TestStatic) {
  EXPECT_EQ(8, sizeof(long long));    // For hash.
  EXPECT_LE(6 * State::HASH_SIZE, MAX_TILES) << "Hash won't fit.";
  EXPECT_LE(BOARD_SIZE - BOARD_X - BOARD_X, 64)
      << "Position won't fit on 6 bits in Hash().";
  EXPECT_LE(16, TriToCode("END"))
      << "Type won't fit on 4 bits in hash.";
  EXPECT_EQ(BOARD_SIZE, BOARD_X * BOARD_Y);
  for (int i = 0; i < 4; ++i) {
    EXPECT_EQ(State::DIRECTIONS[i], -State::DIRECTIONS[OPPOSITE(i)]);
  }
}

TEST(TestState, StdHash) {
  std::map<State::HashValue, int, State::CmpByHash> m;

  int N = State::HASH_SIZE;
  unsigned long long h1[N];
  unsigned long long h2[N];
  for(int i = 0; i < N; ++i) {
    h1[i] = i;
    h2[i] = i;
  }
  EXPECT_FALSE(State::CmpByHash()(h1, h2) || State::CmpByHash()(h2, h1));
  m[h1] = 10;
  EXPECT_EQ(1, m.count(h2));
  h2[N-1] = 99;
  EXPECT_TRUE(State::CmpByHash()(h1, h2) || State::CmpByHash()(h2, h1));
  EXPECT_EQ(0, m.count(h2));
  h2[N-1] = N-1;
  EXPECT_EQ(1, m.count(h2));
}

TEST(TestState, Hash) {
  int N = State::HASH_SIZE;
  unsigned long long h1[N];
  unsigned long long h2[N];
  TestableState s(B004);
  s.Hash(h1);
  s.Hash(h2);
  EXPECT_FALSE(State::CmpByHash()(h1, h2) || State::CmpByHash()(h2, h1));
  s.Sort();
  s.Hash(h2);
  EXPECT_FALSE(State::CmpByHash()(h1, h2) || State::CmpByHash()(h2, h1));
  s.Erase(7);
  s.Hash(h2);
  EXPECT_TRUE(State::CmpByHash()(h1, h2) || State::CmpByHash()(h2, h1));
}

// TEST(TestState, ConstructEquality) {
//   State s002(B002);
//   EXPECT_EQ(0, s002.NumTiles());
//   EXPECT_EQ(0, s002.NumGoalTiles());
//   State s004(B004);
//   EXPECT_EQ(3, s004.NumTiles());
//   EXPECT_EQ(3, s004.NumGoalTiles());
//   State s005(B005);
//   EXPECT_EQ(2, s005.NumTiles());
//   EXPECT_EQ(2, s005.NumGoalTiles());
//   State s006(B006);
//   EXPECT_EQ(2, s006.NumTiles());
//   EXPECT_EQ(2, s006.NumGoalTiles());
//   State s007(B007);
//   EXPECT_EQ(4, s007.NumTiles());
//   EXPECT_EQ(3, s007.NumGoalTiles());
//   State s009(B009);
//   EXPECT_EQ(4, s009.NumTiles());
//   EXPECT_EQ(3, s009.NumGoalTiles());
//   EXPECT_EQ(s002, s002);
//   EXPECT_EQ(s002.Hash(), s002.Hash());
//   EXPECT_EQ(s004, s004);
//   EXPECT_EQ(s004, s005);
//   EXPECT_NE(s004.Hash(), s005.Hash());
//   EXPECT_EQ(s005, s004);
//   EXPECT_NE(s004, s006);
//   EXPECT_NE(s004.Hash(), s006.Hash());
//   EXPECT_NE(s006, s004);
//   EXPECT_NE(s005, s006);
//   EXPECT_NE(s005.Hash(), s006.Hash());
//   EXPECT_NE(s006, s005);
//   EXPECT_NE(s004, s007);
//   EXPECT_NE(s004.Hash(), s007.Hash());
//   EXPECT_NE(s007, s004);

//   EXPECT_NE(s007.Hash(), s009.Hash());
//   EXPECT_EQ(s007, s009);
// }

TEST(TestState, Positions_Erase) {
  TestableState s004(B004);
  EXPECT_EQ(9, s004.NumTiles());
  EXPECT_EQ(-1, s004.Find(POS(1, 5)));
  EXPECT_EQ(0, s004.Find(POS(1, 6)));
  EXPECT_EQ(3, s004.Find(POS(1, 7)));
  EXPECT_EQ(6, s004.Find(POS(1, 8)));
  EXPECT_EQ(1, s004.Find(POS(2, 6)));
  EXPECT_EQ(4, s004.Find(POS(2, 7)));
  EXPECT_EQ(7, s004.Find(POS(2, 8)));

  s004.Erase(4);
  EXPECT_EQ(8, s004.NumTiles());
  EXPECT_EQ(-1, s004.Find(POS(1, 5)));
  EXPECT_EQ(0, s004.Find(POS(1, 6)));
  EXPECT_EQ(3, s004.Find(POS(1, 7)));
  EXPECT_EQ(5, s004.Find(POS(1, 8)));
  EXPECT_EQ(1, s004.Find(POS(2, 6)));
  EXPECT_EQ(-1, s004.Find(POS(2, 7)));
  EXPECT_EQ(6, s004.Find(POS(2, 8)));

  s004.Erase(1);
  EXPECT_EQ(7, s004.NumTiles());
  EXPECT_EQ(-1, s004.Find(POS(1, 5)));
  EXPECT_EQ(0, s004.Find(POS(1, 6)));
  EXPECT_EQ(2, s004.Find(POS(1, 7)));
  EXPECT_EQ(4, s004.Find(POS(1, 8)));
  EXPECT_EQ(-1, s004.Find(POS(2, 6)));
  EXPECT_EQ(-1, s004.Find(POS(2, 7)));
  EXPECT_EQ(5, s004.Find(POS(2, 8)));
}

// /*
// TEST(TestState, MoveReturnValue) {
//   Board b(B004);
//   State s(B004);
//   {
//     State n(b, s, State::UP);
//     EXPECT_TRUE(s.Move(b, State::UP, &n));
//     EXPECT_EQ(s, n);
//   }
//   {
//     State n1(b, s, State::DOWN);
//     EXPECT_FALSE(s.Move(b, State::DOWN, &n1));
//   }
// }
// */

// static const char B003[] =
//     "##########"
//     "#    c c #"
//     "#        #"
//     "#   b  # #"
//     "# b  a   #"
//     "#  #   c #"
//     "#    #   #"
//     "##########";

TEST(TestState, MoveSimple) {
  Board b(B003, *RULES);
  State s(B003);
  // Stops at wall.
  State n1(b, s, 0, State::DOWN);
  EXPECT_EQ(
    "##########\n"
    "#    c c #\n"
    "#        #\n"
    "#   b  # #\n"
    "# b      #\n"
    "#  # a c #\n"
    "#    #   #\n"
    "##########\n",
    b.DebugStringWithState(n1));

  // c kills it.
  State n2(b, s, 0, State::UP);
  EXPECT_EQ(
    "##########\n"
    "#    c c #\n"
    "#        #\n"
    "#   b  # #\n"
    "# b      #\n"
    "#  #   c #\n"
    "#    #   #\n"
    "##########\n",
    b.DebugStringWithState(n2));

  // Stops at b.
  State n3(b, s, 0, State::LEFT);
  EXPECT_EQ(
    "##########\n"
    "#    c c #\n"
    "#        #\n"
    "#   b  # #\n"
    "# ba     #\n"
    "#  #   c #\n"
    "#    #   #\n"
    "##########\n",
    b.DebugStringWithState(n3));

  // c kills it.
  State n4(b, s, 0, State::RIGHT);
  EXPECT_EQ(
    "##########\n"
    "#    c c #\n"
    "#        #\n"
    "#   b  # #\n"
    "# b      #\n"
    "#  #   c #\n"
    "#    #   #\n"
    "##########\n",
    b.DebugStringWithState(n4));
}

TEST(TestState, MoveSimple2) {
  Board b(B005, *RULES);
  State s(B005);
  // 'd' turns 'a' into 'e', hence 'c' doesn't kill it.
  State n1(b, s, 0, State::UP);
  EXPECT_EQ(
    "##########\n"
    "#    d   #\n"
    "#    e c #\n"
    "#   b  # #\n"
    "# b      #\n"
    "#  #   c #\n"
    "#    #   #\n"
    "##########\n",
    b.DebugStringWithState(n1));
}

// TEST(TestState, MoveHorizontalFixed) {
//   {
//     static const char STABLE[] =
//         "#########"
//         "#ABC#FED#"
//         "#IGH#   #"
//         "#   #   #"
//         "#       #"
//         "#       #"
//         "#########";
//     Board b(STABLE);
//     State s(STABLE);
//     {
//       State n(b, s, State::RIGHT);
//       EXPECT_EQ(s, n);
//     }
//     {
//       State n(b, s, State::LEFT);
//       EXPECT_EQ(s, n);
//     }
//   }
//   {
//     static const char STABLE[] =
//         "#########"
//         "#   #   #"
//         "#   #BAC#"
//         "#DFE#HIG#"
//         "#       #"
//         "#       #"
//         "#########";
//     Board b(STABLE);
//     State s(STABLE);
//     {
//       State n(b, s, State::RIGHT);
//       EXPECT_EQ(s, n);
//     }
//     {
//       State n(b, s, State::LEFT);
//       EXPECT_EQ(s, n);
//     }
//   }
// }

// TEST(TestState, MoveHorizontalSlack) {
//   static const char SLACK1[] =
//       "#########"
//       "#    #@@#"
//       "#       #"
//       "#       #"
//       "#       #"
//       "#DCABEF #"
//       "#########";
//   static const char SLACK2[] =
//       "#########"
//       "#    #@@#"
//       "#       #"
//       "#       #"
//       "#       #"
//       "# DCABEF#"
//       "#########";
//   Board b(SLACK1);
//   ASSERT_EQ(Board(SLACK2), b);

//   State s1(SLACK1);
//   State s2(SLACK2);
//   {
//     State n(b, s1, State::LEFT);
//     EXPECT_EQ(s1, n);
//   }
//   {
//     State n(b, s1, State::RIGHT);
//     EXPECT_EQ(s2, n);
//   }
//   {
//     State n(b, s2, State::LEFT);
//     EXPECT_EQ(s1, n);
//   }
//   {
//     State n(b, s2, State::RIGHT);
//     EXPECT_EQ(s2, n);
//   }
// }

// TEST(TestState, MoveVerticalFixed) {
//   static const char STABLE[] =
//       "#########"
//       "#D  # J #"
//       "#E  # G #"
//       "#C  # F #"
//       "#B    H #"
//       "#A    I #"
//       "#########";
//   Board b(STABLE);
//   State s(STABLE);
//   {
//     State n(b, s, State::UP);
//     EXPECT_EQ(s, n);
//   }
//   {
//     State n(b, s, State::DOWN);
//     EXPECT_EQ(s, n);
//   }
// }

// TEST(TestState, MoveVerticalSlack) {
//   static const char SLACK1[] =
//       "#########"
//       "#A  H   #"
//       "#B  F  ##"
//       "#C  E  I#"
//       "#D  G  J#"
//       "#       #"
//       "#########";
//   static const char SLACK2[] =
//       "#########"
//       "#       #"
//       "#A  H  ##"
//       "#B  F   #"
//       "#C  E  I#"
//       "#D  G  J#"
//       "#########";
//   Board b(SLACK1);
//   ASSERT_EQ(Board(SLACK2), b);

//   State s1(SLACK1);
//   State s2(SLACK2);
//   {
//     State n(b, s1, State::UP);
//     EXPECT_EQ(s1, n);
//   }
//   {
//     State n(b, s1, State::DOWN);
//     EXPECT_EQ(s2, n);
//   }
//   {
//     State n(b, s2, State::UP);
//     EXPECT_EQ(s1, n);
//   }
//   {
//     State n(b, s2, State::DOWN);
//     EXPECT_EQ(s2, n);
//   }
// }

// TEST(TestState, Hash) {
//   static const char MANY[] =
//       "#########"
//       "#D  # J #"
//       "#E  # G #"
//       "#C  # F #"
//       "#B    H #"
//       "#A    I #"
//       "#########";
//   State s(MANY);
//   EXPECT_NE(s.Hash(), State(
//       "#########"
//       "#D  # J #"
//       "#E  # G #"
//       "#C  # F #"
//       "#B    H #"
//       "# A   I #"
//       "#########").Hash());
//   EXPECT_NE(s.Hash(), State(
//       "#########"
//       "#D  #  J#"
//       "#E  # G #"
//       "#C  # F #"
//       "#B    H #"
//       "#A    I #"
//       "#########").Hash());
// }

// TEST(TestState, GetHistory) {
//   Board b(B003);
//   State s(B003);
//   EXPECT_EQ("", s.GetHistory());
//   EXPECT_EQ(0, s.GetHistoryLen());

//   State s1(b, s, State::DOWN);
//   EXPECT_EQ("D", s1.GetHistory());
//   EXPECT_EQ(1, s1.GetHistoryLen());

//   State s2(b, s1, State::RIGHT);
//   EXPECT_EQ("DR", s2.GetHistory());
//   EXPECT_EQ(2, s2.GetHistoryLen());
// }

// TEST(TestState, GetHistory_30) {
//   Board b(B003);
//   State s(B003);
//   State ss[30];
//   s.Move(b, State::LEFT, &ss[0]);
//   EXPECT_EQ("L", ss[0].GetHistory());
//   EXPECT_EQ(1, ss[0].GetHistoryLen());
//   for(int i = 1; i < 30; ++i) {
//     ss[i-1].Move(b, i%4, &ss[i]);
//   }
//   EXPECT_EQ("LLRDULRDULRDULRDULRDULRDULRDUL", ss[29].GetHistory());
//   EXPECT_EQ(30, ss[29].GetHistoryLen());
// }

// TEST(TestState, GetHistory_60) {
//   Board b(B008);
//   State s(B008);
//   State ss[60];
//   s.Move(b, State::LEFT, &ss[0]);
//   EXPECT_EQ("L", ss[0].GetHistory());
//   EXPECT_EQ(1, ss[0].GetHistoryLen());
//   for(int i = 1; i < 60; ++i) {
//     ss[i-1].Move(b, i%4, &ss[i]);
//   }
//   EXPECT_EQ("LLRDULRDULRDULRDULRDULRDULRDULRDULRDULRDULRDULRDULRDULRDULRD", ss[59].GetHistory());
//   EXPECT_EQ(60, ss[59].GetHistoryLen());
// }

