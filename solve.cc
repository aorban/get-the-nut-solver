#include <cstdlib>

#include <assert.h>
#include <iostream>
#include <sstream>
#include <set>
#include <vector>

#include "convert.h"
#include "log.h"
#include "solve.h"

#define STATS true

using namespace std;

static const int MOVEABLE[] = {
  0,  // "ACG";
  1,  // "SQR";
  1,  // "HOG";
  1,  // "MSE";
  0,  // "ACN";
  1,  // "WLF";
  1,  // "BER";
  0,  // "TPP";
  1,  // "ALG";
  0,  // "SWM";
  0,  // "MSS";
  0,  // "HOS";
  1,  // "SNK";
  1,  // "PRC";
  0,  // "FSN";
};

typedef std::map<State::HashValue, int, State::CmpByHash> VisitedMap;
//typedef boost::asio::detail::hash_map<long long, int> VisitedMap;

std::string ReplaySolution(const Board& b, const State& start_state,
                           const State::HistoryItem* history, int history_len) {
  std::stringstream ss;
  State* states = new State[history_len + 1];
  states[0] = start_state;
  for (int i = 0; i < history_len; ++i) {
    const Tile& moving_tile = states[i].GetTile(history[i].tile_index);
    int pos = moving_tile.pos;
    int type = moving_tile.type;
    std::stringstream move;
    move << "(" << pos / BOARD_X << "," << pos % BOARD_X << ")-"
       << CodeToTri('a' + type) << "-"
         << State::DIRNAME[history[i].dir];
    ss << move.str() << "/";
    string board_str = b.DebugStringNiceWithMove(states[i], 
                                                 history[i].tile_index,
                                                 history[i].dir);
    LOG(0) << move.str() << endl;
    LOG(0) << board_str << endl;

    states[i].Move(b, history[i].tile_index, history[i].dir, &states[i + 1]);
  }
  LOG(0) <<  b.DebugStringNice(states[history_len]);
  delete states;
  return ss.str();
}

// Returns whether the puzzle can be solved. Sets the number of moves and the
// direction of the moves in the output args.
SolveResult solve(const Board& board, const State& start_state) {
  int ss = 0;  // state_size. How many states we store in memory.
  SolveResult res;
  // if (board.IsGoal(start_state)) {
  //   res.end_state = start_state;
  //   res.success = true;
  //   return res;
  // }

  // state hash -> min_moves
  VisitedMap visited_states;

  // Indexed by min_moves. min_moves is: Lower bound on the number of moves
  // needed to solve the puzzle through any states in group. The lower bound is
  // the sum of the number of moves needed from start state to current state +
  // the lower bound on the number of moves needed from the current state to
  // reach the end state. All states in the group have the same lower
  // bound of min_moves.
  vector<StateGroup> state_groups;
  int min_moves = board.MinMovesFrom(start_state);
  LOG(1) << "min:" << min_moves << " "
         << "num states: " << visited_states.size() << endl;
  state_groups.resize(MAX_MOVES);
  state_groups[min_moves].AddState(new State(start_state));
  State::HashValue start_hash = new unsigned long long[State::HASH_SIZE];
  start_state.Hash(start_hash);
  visited_states.insert(make_pair(start_hash, min_moves));
  if (STATS) { ++ss; if (ss > res.max_mem_state) res.max_mem_state = ss; }
  while(true) {
    while (state_groups[min_moves].Empty()) {
      ++min_moves;
      LOG(1) << "min:" << min_moves << " " << "num states: "
             << visited_states.size() << endl;
      if (min_moves >= MAX_MOVES) {
        LOG(1) << "num states: " << visited_states.size() << endl;
        return res;
      }
    }
    // Handle next state in group.
    const State* curr_state = state_groups[min_moves].Pop();
    LOG(1) << "curr state : " << min_moves << "\n" 
           << board.DebugStringWithState(*curr_state) << endl;
    if (STATS) { --ss; }
    const int history_len = curr_state->GetHistoryLen();
    for (int tile_index = 0; tile_index < curr_state->NumTiles(); ++tile_index) {
      if (!MOVEABLE[curr_state->GetTile(tile_index).type]) continue;
      for (int di = 0; di < 4; ++di) {
        // Put it on the heap.
        State* new_state = new State();
        int move_res = curr_state->Move(board, tile_index, di, new_state);
        if (STATS) ++res.num_moves;
        if (move_res == State::LOSE) {
          continue;
        }
        if (move_res == State::WIN) {
          res.success = true;
          res.end_state = *new_state;
          delete new_state;
          delete curr_state;
          LOG(1) << "num states: " << visited_states.size() << endl;
          return res;
        }
        // Compute min_moves
        int new_min_moves =
          history_len + 1 + board.MinMovesFrom(*new_state);
        LOG(1) << "new min moves:" << new_min_moves << endl;
        //assert(new_min_moves >= min_moves);
        // Check if the new state has already been seen.
        State::HashValue new_hash = new unsigned long long[State::HASH_SIZE];
        new_state->Hash(new_hash);
        LOG(1) << "new hash:" << new_hash[0] << new_hash[1] << endl;
        VisitedMap::iterator it = visited_states.find(new_hash);
        if (it != visited_states.end()) {
          // state already visited
          LOG(1) << "State visited\n";
          int prev_min_moves = it->second;
          if (prev_min_moves > new_min_moves) {
            // found state is in a bigger group, remove from that group.
            state_groups[prev_min_moves].RemoveState(new_hash);
            if (STATS) ++res.num_visited_hit_improve;
            if (STATS) { --ss; }
          } else {
            // found state is in a smaller or equal group, drop the new state.
            delete new_state;
            if (STATS) ++res.num_visited_hit_drop;
            continue;
          }
        }
        LOG(1) << "NEW State with min moves" << new_min_moves << endl;
        // We need to keep this new state and insert it in its group.
        state_groups[new_min_moves].AddState(new_state);
        if (STATS) { ++ss; if (ss > res.max_mem_state) res.max_mem_state = ss; }
        visited_states.insert(make_pair(new_hash, new_min_moves));
        if (STATS) ++res.num_visited_states;
      }
    }
    delete curr_state;
  }
}
