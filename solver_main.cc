#include <iostream>
#include <string>

#include "board.h"
#include "convert.h"
#include "rules.h"
#include "utils.h"
#include "solve.h"

using namespace std;

static Rules REAL_RULES = Rules("rules-real.csv");

int main(int argc, char **argv) {
  char bc[] =
    "##########"
    "#        #"
    "#        #"
    "#        #"
    "#        #"
    "#        #"
    "#        #"
    "##########";
  for (int i = 0; i < 6; ++i) {
    std::string st;
    std::cin >> st;
    vector<string> values;
    SplitString(st, ",", &values);
    if (values.size() != 8) {
      cout << "Broken line in csv: " << st;
      exit(2);
    }
    for (int x = 0; x < 8; ++x) {
      if (values[x].find("TRE") != string::npos) {
        bc[(i+1)*10+(x+1)] = '#';
      } else {
        bc[(i+1)*10+(x+1)] = TriToCode(values[x].substr(0, 3).c_str());
      }
    }
  }
  Board b(bc, REAL_RULES);
  State s(bc);
  const SolveResult res = solve(b, s);
  const std::string sol = ReplaySolution(b,
                                         s,
                                         res.end_state.GetHistory(),
                                         res.end_state.GetHistoryLen());
  if (argc > 1) {
    std::cout
      << sol.length() << ", "
      << sol << ", "
      << res.num_moves << ", "
      << res.max_mem_state << ", "
      << res.num_visited_states << ", "
      << res.num_visited_hit_improve << ", "
      << res.num_visited_hit_drop << ", "
      << std::endl;
    return 0;
  }
  std::cout << "SOLUTION:" << (res.success ? "Y" : "N") << " "
            <<"(" << res.end_state.GetHistoryLen() << ") " << std::endl;
  vector<string> sol_moves;
  SplitString(sol, "/", &sol_moves);
  for (int i = 0; i < sol_moves.size(); ++i) {
    cout << sol_moves[i] << endl;
  }
  std::cout << "num_moves: "
    << res.num_moves << std::endl;
  std::cout << "num_visited_states: "
    << res.num_visited_states << std::endl;
  std::cout << "num_visited_hit_improve: "
    << res.num_visited_hit_improve << std::endl;
  std::cout << "num_visited_hit_drop: "
    << res.num_visited_hit_drop << std::endl;
  std::cout << "max_mem_state: "
    << res.max_mem_state << std::endl;

  if (!res.success) {
    return 0;
  }
  // std::cout << b.DebugStringWithState(s);
  // State curr_state = s;
  // for (int i = 0; i < sol.size(); ++i) {
  //   State new_state;
  //   curr_state.Move(b, MoveFromChar(sol[i]), &new_state);
  //   std::cout << b.DebugStringWithState(new_state);
  //   curr_state = new_state;
  // }
}
