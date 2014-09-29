#include <cstdlib>
#include <iostream>
#include <sstream>
#include <stddef.h>
#include <stdlib.h>

#include "board.h"
#include "convert.h"
#include "log.h"
#include "rules.h"

using namespace std;

static const int INFINITY = 1000;

////////////////////////////////////////////////////////////////////////////////
// Board
////////////////////////////////////////////////////////////////////////////////

Board::Board(const char *p, const Rules& rules_) : rules(rules_) {
  for (int i = 0; i < BOARD_Y * BOARD_X; ++i) {
    const char c = p[i];
    b[i] = c == '#' ? '#' : BLANK;
  }
  // ComputeDistances(i, dir);
}

std::string Board::DebugString() const {
  return DebugString(NULL);
}

std::string Board::DebugStringWithState(const State &s) const {
  return DebugString(&s);
}

// std::string Board::DebugStringWithDistance(int tile, int dir) const {
//   std::string s;
//   int pos = 0;
//   for (int y = 0; y < BOARD_Y; ++y) {
//     for (int x = 0; x < BOARD_X; ++x, ++pos) {
//       char c = b[pos];
//       if (c == BLANK) {
//         int d = dist[tile][pos][dir];
//         if (d == INFINITY) {
//           c = '-';
//         } else {
//           c = char(d) + '0';
//         }
//       }
//       s += c;
//     }
//     s += '\n';
//   }
//   return s;
// }

std::string Board::DebugString(const State *state) const {
  std::string s;
  int pos = 0;
  for (int y = 0; y < BOARD_Y; ++y) {
    for (int x = 0; x < BOARD_X; ++x, ++pos) {
      char c = b[pos];
      if (c == BLANK) c = ' ';
      if (state) {
        int index = state->Find(pos);
        if (index != -1) {
          c = 'a' + state->t[index].type;
        }
      }
      s += c;
    }
    s += '\n';
  }
  if (state) {
    for (int i = 0; i < state->num_tiles; ++i) {
      LOG(1) << i << ": '" << char('a' + state->t[i].type) << "' "
             << state->t[i].pos << endl;
    }
  }
  return s;
}

// void Board::FloodFrom(int tile, int pos, int nodir) {
//   int value = dist[tile][pos][nodir];
//   //printf("pos: %d\n", pos);
//   //printf("value:%d\n", value);
//   for (int di = 0; di < 4; ++di) {
//     if (di == OPPOSITE(nodir)) continue;
//     int d = State::DIRECTIONS[di];
//     //printf("pos: %d, nodir: %d, dir: %d\n", pos, nodir, d);
//     int next_pos = pos + d;
//     if (b[next_pos] == BLANK && dist[tile][next_pos][nodir] == INFINITY) {
//       dist[tile][next_pos][nodir] = value;
//       FloodFrom(tile, next_pos, nodir);
//     }
//   }
// }

// int Board::FindMinimum(int tile, int dir) {
//   int min = INFINITY;
//   int minpos = -1;
//   for (int pos = BOARD_X; pos < BOARD_SIZE - BOARD_X; ++pos) {
//     const int nextpos = pos + State::DIRECTIONS[dir];
//     if (b[pos] == BLANK && b[nextpos] == BLANK &&
//         dist[tile][pos][dir] == INFINITY &&
//         dist[tile][nextpos][dir] < min) {
//       min = dist[tile][nextpos][dir];
//       minpos = pos;
//     }
//   }
//   if (minpos >= 0) dist[tile][minpos][dir] = min + 1;
//   return minpos;
// }

// void Board::ComputeDistances(int tile, int dir) {
//   // int dist[MAX_TILES][BOARD_Y * BOARD_X][4];  // Minimum distances.
//   for (int i = 0; i < BOARD_SIZE; ++i) {
//     dist[tile][i][dir] = INFINITY;
//   }
//   int pos = goal.t[tile];
//   dist[tile][pos][dir] = 0;
//   //printf("----------------------tile: %d, pos: %d, dir:%d\n", tile, pos, dir);
//   do {
//     FloodFrom(tile, pos, dir);
//     pos = FindMinimum(tile, dir);
//   } while (pos >= 0);
// }

int Board::MinMovesFrom(const State &state) const {
  return 0;
  int squirrel_pos = state.GetSquirrelPos();
  int num_blocking = 0;
  for (int i = 0; i < state.NumTiles(); ++i) {
    num_blocking += dist[squirrel_pos].is_blocking[state.t[i].pos];
  }
  return num_blocking + dist[squirrel_pos].squirrel_step;
}

////////////////////////////////////////////////////////////////////////////////
// State
////////////////////////////////////////////////////////////////////////////////

const int State::UP;
const int State::LEFT;
const int State::RIGHT;
const int State::DOWN;

const char State::DIRNAME[] = "ULRD";
const int State::DIRECTIONS[] = {-BOARD_X, -1, +1, +BOARD_X};  // 3-dir must work!

// ON must be first, after that starting from UP and then clockwise.
const int State::DIR_LOOKUP[][4][2] = {
  {  // up
    {0, Rules::ON},  // on
    {DIRECTIONS[State::UP], Rules::AHEAD},  // ahead
    {DIRECTIONS[State::RIGHT], Rules::SIDE}, // side
    {DIRECTIONS[State::LEFT], Rules::SIDE}, // side
  },
  {  // left
    {0, Rules::ON},  // on
    {DIRECTIONS[State::UP], Rules::SIDE}, // side
    {DIRECTIONS[State::DOWN], Rules::SIDE}, // side
    {DIRECTIONS[State::LEFT], Rules::AHEAD},  // ahead
  },
  {  // right
    {0, Rules::ON},  // on
    {DIRECTIONS[State::UP], Rules::SIDE}, // side
    {DIRECTIONS[State::RIGHT], Rules::AHEAD},  // ahead
    {DIRECTIONS[State::DOWN], Rules::SIDE}, // side
  },
  {  // down
    {0, Rules::ON},  // on
    {DIRECTIONS[State::RIGHT], Rules::SIDE}, // side
    {DIRECTIONS[State::DOWN], Rules::AHEAD},  // ahead
    {DIRECTIONS[State::LEFT], Rules::SIDE}, // side
  },
};


State::State() {
  history_len = 0;
}

State::State(const Board &board, const State &old_state, int tile_index, int move) {
  old_state.Move(board, tile_index, move, this);
}

State::State(const char *p) {
  Initialize(p);
}

void State::Initialize(const char *p) {
  history_len = 0;
  num_tiles = 0;
  for (int i = 0; i < MAX_TILES; ++i) { t[i].pos = 0; t[i].type = 0; }
  for (int i = 0; i < BOARD_Y * BOARD_X; ++i) {
    const char c = p[i];
    if (c >= 'a' && c <= 'a' + ('Z'-'A')) {
      const int type = c - 'a';
      if (type == TriToCode("---") - 'a') continue;
      if (num_tiles + 1 >= MAX_TILES) {
        printf("Tile too high: '%c'. Increase MAX_TILES.\n", c);
        exit(10);
      }
      t[num_tiles].pos = i;
      t[num_tiles].type = type;
      ++num_tiles;
    }
  }
  Sort();
}

int State::Find(int pos) const {
  for (int i = 0; i < num_tiles; ++i) {
    if (t[i].pos == pos) {
      return i;
    }
  }
  return -1;
}

struct ActionInfo {
  Action action;
  int static_tile_index;
};

inline int prio_cmp(const void* a, const void* b) {
  return ((ActionInfo*)a)->action.prio - ((ActionInfo*)b)->action.prio 
    ? ((ActionInfo*)a)->action.prio - ((ActionInfo*)b)->action.prio
    // Makes it stable.
    : ((ActionInfo*)a-(ActionInfo*)b);
}



int State::Move(
    const Board &board, int moving_tile_index, int dir, State *n) const {
  LOG(1) << "\n\nMove start: " << char(t[moving_tile_index].type + 'a')
         << " " << DIRNAME[dir] << endl;
  const int move = DIRECTIONS[dir];
  const State *o = this;  // "old"
  // Copy state.
  *n = *o;
  {
    HistoryItem hist;
    hist.tile_index = moving_tile_index;
    hist.dir = dir;
    n->history[n->history_len++] = hist;
  }
  Tile* moving_tile = &(n->t[moving_tile_index]);
  int curr_pos = moving_tile->pos;
  LOG(1) << board.DebugStringWithState(*n);
  int num_actions = 0;
  ActionInfo action_infos[4];
  // The (other) tile we are stepping on.
  int tile_on_index = -1;
  while (true) {
    // We just landed on curr_pos. We check all the possible rules.
    LOG(2) << "Landed on tile " << curr_pos << endl;
    for (int i = 0; i < 4; ++i) {
      const int lookup_dir = DIR_LOOKUP[dir][i][0];
      const int relation = DIR_LOOKUP[dir][i][1];
      int lookup_pos = curr_pos + lookup_dir;
      // Don't lookup rule if there is a wall there
      if (board.b[lookup_pos] != BLANK) {
        continue;
      }
      LOG(2) << "checking tile at " << lookup_pos <<  " dir: " << lookup_dir
             << " rel: " << relation << std::endl;
      int static_tile_index = -1;
      if (relation == Rules::ON) {
        // Reuse what we have computed before. It is guaranteed to happen before
        // the Rules::AHEAD case by the order in DIR_LOOKUP, otherwise we would
        // overwrite tile_on_index.
        static_tile_index = tile_on_index;
      } else {
        static_tile_index = n->Find(lookup_pos);
      }
      if (relation == Rules::AHEAD) {
        tile_on_index = static_tile_index;
      }
      if (static_tile_index == -1) continue;
      LOG(2) << "tile at " << lookup_pos <<  " dir: " << lookup_dir
             << " |" << char('a' + n->t[static_tile_index].type) << "|"
             << " rel: " << relation << std::endl;
      Action a = board.rules.GetAction(moving_tile->type,
                                       n->t[static_tile_index].type,
                                       relation);
      if (a.exists) {
        LOG(2) << "Added new action" << endl;
        action_infos[num_actions].action = a;
        action_infos[num_actions].static_tile_index = static_tile_index;
        ++num_actions;
      }
    }
    if (num_actions) break;

    int next_pos = curr_pos + move;
    if (board.b[next_pos] != BLANK) break;

    LOG(2) << "Moving to " << next_pos << std::endl;
    moving_tile->pos = next_pos;
    LOG(2) << board.DebugStringWithState(*n);
    curr_pos = next_pos;
  }

  LOG(2) << "Before sort\n";
  for (int i = 0; i < num_actions; ++i) {
    LOG(2) << PrintAction(action_infos[i].action);
  }
  qsort(action_infos, num_actions, sizeof(ActionInfo), prio_cmp);
  LOG(2) << "After sort\n";
  for (int i = 0; i < num_actions; ++i) {
    LOG(2) << PrintAction(action_infos[i].action);
  }

  for (int i = 0; i < num_actions; ++i) {
    const Action& a = action_infos[i].action;
    const int &static_tile_index = action_infos[i].static_tile_index;
    LOG(2) << board.DebugStringWithState(*n);
    if (a.lost) return LOSE;
    if (a.won) return WIN;
    LOG(1) << "ApplyAction: " << moving_tile_index << " " << static_tile_index << endl;
    LOG(1) << PrintAction(a);
    int orig_moving_type = moving_tile->type;
    n->t[moving_tile_index].type = a.moving_new_animal;
    n->t[static_tile_index].type = a.static_new_animal;
    LOG(2) << board.DebugStringWithState(*n);
    if (a.moving_new_animal != orig_moving_type) {
      // Stop applying the rest of the action_infos.
      break;
    }
  }
  n->Sort();
  while (n->t[n->num_tiles-1].type == TriToCode("---") - 'a') --n->num_tiles;
  LOG(1) << board.DebugStringWithState(*n);
  LOG(2) << "end of move\n";
  return 0;
}

void State::Hash(HashValue hash) const {
  for (int i = 0; i < HASH_SIZE; ++i) hash[i] = 0;
  int idx_tile = 0;
  int idx_hash = 0;
  while (idx_tile < num_tiles) {
    for (int i = 0; i < 6; ++i) {
      int local_hash = (t[idx_tile].type << 4) + t[idx_tile].pos - BOARD_X;
      hash[idx_hash] <<= 10;
      hash[idx_hash] += local_hash;
      if (++idx_tile >= num_tiles) break;
    }
    ++idx_hash;
  }
}

const State::HistoryItem *State::GetHistory() const {
  return history;
}

int State::GetHistoryLen() const {
  return history_len;
}
