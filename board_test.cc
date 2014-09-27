#include <iostream>

#include "board.h"
#include "rules.h"

#include "gtest/gtest.h"

class MockRules : public Rules {
 public:
  MockRules() : Rules() {
    {
      // b does nothing with a, just stops it.
      int a1 = 'a' - 'a';
      int a2 = 'b' - 'a';
      Action a;
      a.moving_animal_dies = 0;
      a.static_animal_dies = 0;
      a.moving_new_animal = a1;
      a.static_new_animal = a2;
      a.won = 0;
      a.lost = 0;
      a.prio = 2;
      a.exists = 1;
      for (int r = 0; r < NUM_RELATIONS; ++r) {
        a.continues = (r != Rules::AHEAD);
        rules[a1][a2][r] = a;
      }
    }

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
  /*
  Action GetAction(int m, int s, int r) const {
    char mo = char(m + 'a');
    char st = char(s + 'a');
    Action a;
    // b does nothing with a, just stops it.
    if (mo == 'a' && st == 'b') {
      a.continues = (r != Rules::AHEAD);
      a.moving_new_animal = m;
      a.static_new_animal = s;
    }
    // c kills a if a moves near it.
    if (mo == 'a' && st == 'c') {
      a.continues = 0;
      a.moving_animal_dies = 1;
      a.static_new_animal = s;
    }
    // d turns 'a' into 'e'
    if (mo == 'a' && st == 'd') {
      a.continues = 0;
      a.moving_animal_dies = 0;
      a.moving_new_animal = ('e' - 'a');
      a.static_new_animal = s;
    }
    return a;
  }
  */
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

// TEST(TestState, TestStatic) {
//   EXPECT_EQ(8, sizeof(long long));  // For hash.
//   EXPECT_LE(10, MAX_TILES);  // If this breaks, Hash() needs to be reconsidered.
//   EXPECT_LT(BOARD_SIZE, 1 << POSITION_BITS);
//   EXPECT_EQ(BOARD_SIZE, BOARD_X * BOARD_Y);
//   for (int i = 0; i < 4; ++i) {
//     EXPECT_EQ(State::DIRECTIONS[i], -State::DIRECTIONS[OPPOSITE(i)]);
//   }
// }

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

// TEST(TestState, Positions) {
//   {
//     TestableState s004(B004);
//     EXPECT_EQ(s004.Find(POS(1, 5)), 0);
//     EXPECT_EQ(s004.Find(POS(1, 6)), 1);
//     EXPECT_EQ(s004.Find(POS(1, 7)), 2);
//     EXPECT_EQ(s004.Find(POS(1, 4)), -1);
//   }
//   {
//     TestableState s007(B007);
//     EXPECT_EQ(s007.Find(POS(1, 5)), 2);
//     EXPECT_EQ(s007.Find(POS(1, 6)), 0);
//     EXPECT_EQ(s007.Find(POS(1, 7)), 1);
//     EXPECT_EQ(s007.Find(POS(1, 4)), -1);
//   }
// }

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
  State n1(b, s, 4, State::DOWN);
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
  State n2(b, s, 4, State::UP);
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
  State n3(b, s, 4, State::LEFT);
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
  State n4(b, s, 4, State::RIGHT);
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
  State n1(b, s, 4, State::UP);
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

