#include <iostream>
#include <sstream>
#include <set>

#include "board.h"
#include "convert.h"
#include "log.h"
#include "rules.h"
#include "solve.h"

#include "gtest/gtest.h"

using namespace std;

#include "testboards.h"

static Rules REAL_RULES = Rules("rules-real.csv");

void CheckSolution(const char* bs, const string& exp_sol) {
  Board b(bs, REAL_RULES);
  State s(bs);
  const SolveResult res = solve(b, s);
  int num_moves = res.end_state.GetHistoryLen();
  bool success = res.success;
  const std::string sol = ReplaySolution(b,
                                         s,
                                         res.end_state.GetHistory(),
                                         num_moves);
  EXPECT_EQ(!exp_sol.empty(), success);
  EXPECT_EQ(count(exp_sol.begin(), exp_sol.end(), '/'), num_moves);
  EXPECT_EQ(exp_sol, sol);
}

TEST(TestSolve, Solve) {
  CheckSolution(B001, "(3,2)-SQR-U/");
  CheckSolution(B002, 
                "(3,2)-SQR-D/"
                "(6,2)-SQR-R/"
                "(6,8)-SQR-U/"
                "(1,8)-SQR-L/");
  CheckSolution(B003, 
                "(2,3)-MSE-D/"
                "(3,2)-SQR-R/"
                "(3,8)-SQR-D/"
                "(6,8)-SQR-L/"
                "(6,4)-SQR-U/"
                "(1,4)-SQR-L/");
  CheckSolution(B004, 
                "(1,6)-MSE-L/"
                "(1,4)-HOG-R/"
                "(1,8)-HOG-D/"
                "(3,2)-SQR-R/"
                "(3,8)-SQR-U/"
                "(1,8)-SQR-L/"); 
}
