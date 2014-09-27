#include <cstdlib>
#include <iostream>
#include <stddef.h>
#include <stdlib.h>

#include "board.h"
#include "rules.h"
//#include "network_sort.h"

#define DEBUG true

class Voidify {
 public:
  Voidify() { }
  // This has to be an operator with a precedence lower than << but
  // higher than ?:
  void operator&(std::ostream&) {}
};
#define LOG(x) !DEBUG ? (void)0 : Voidify() & std::cout

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
//     int newpos = pos + d;
//     if (b[newpos] == BLANK && dist[tile][newpos][nodir] == INFINITY) {
//       dist[tile][newpos][nodir] = value;
//       FloodFrom(tile, newpos, nodir);
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

const int State::DIRECTIONS[] = {-BOARD_X, -1, +1, +BOARD_X};  // 3-dir must work!
const int State::SIDE[][2] = {
    {State::LEFT, State::RIGHT},  // up
    {State::UP, State::DOWN},  // left
    {State::UP, State::DOWN},  // right
    {State::LEFT, State::RIGHT},  // down
  };

const int State::DIR_LOOKUP[][4][2] = {
  {  // up
    {0, Rules::ON},  // on
    {State::UP, Rules::AHEAD},  // ahead
    {State::LEFT, Rules::SIDE}, // side
    {State::RIGHT, Rules::SIDE}, // side
  },
  {  // left
    {0, Rules::ON},  // on
    {State::LEFT, Rules::AHEAD},  // ahead
    {State::UP, Rules::SIDE}, // side
    {State::DOWN, Rules::SIDE}, // side
  },
  {  // right
    {0, Rules::ON},  // on
    {State::RIGHT, Rules::AHEAD},  // ahead
    {State::UP, Rules::SIDE}, // side
    {State::DOWN, Rules::SIDE}, // side
  },
  {  // down
    {0, Rules::ON},  // on
    {State::DOWN, Rules::AHEAD},  // ahead
    {State::LEFT, Rules::SIDE}, // side
    {State::RIGHT, Rules::SIDE}, // side
  },
};


State::State() {
  for (int i = 0; i < MAX_HISTORY; ++i) history[i] = 0;
  history[0] = 1;
}

State::State(const Board &board, const State &old_state, int tile_index, int move) {
  old_state.Move(board, tile_index, move, this);
}

State::State(const char *p) {
  Initialize(p);
}

void State::Initialize(const char *p) {
  for (int i = 0; i < MAX_HISTORY; ++i) history[i] = 0;
  history[0] = 1;
  num_tiles = 0;
  for (int i = 0; i < MAX_TILES; ++i) { t[i].pos = 0; t[i].type = 0; }
  for (int i = 0; i < BOARD_Y * BOARD_X; ++i) {
    const char c = p[i];
    if (c >= 'a' && c <= 'a' + ('Z'-'A')) {
      const int type = c - 'a';
      if (num_tiles + 1 >= MAX_TILES) {
        printf("Tile too high: '%c'. Increase MAX_TILES.", c);
        exit(10);
      }
      t[num_tiles].pos = i;
      t[num_tiles].type = type;
      ++num_tiles;
    }
  }
  Sort();
  // network_sort(&t[num_tiles], num_tiles);
}

int State::Find(int pos) const {
  for (int i = 0; i < num_tiles; ++i) {
    if (t[i].pos == pos) {
      return i;
    }
  }
  return -1;
}

int State::Move(const Board &board, int moving_tile_index, int dir, State *n) const {
  // TODO: history
  // for (int i = MAX_HISTORY - 1; i > 0; --i) {
  //   unsigned long long carry = (history[i-1] & (3llu << 62)) >> 62;
  //   n->history[i] = (history[i] << 2) | carry;
  // }
  // n->history[0] = history[0] << 2 | dir;

  const int move = DIRECTIONS[dir];
  const State *o = this;  // "old"
  // Copy state.
  *n = *o;
  Tile* moving_tile = &(n->t[moving_tile_index]);
  int curr_pos = moving_tile->pos;
  bool can_move = true;
  int num_steps = 0;
  int final_res = 0;
  // TODO(prio)??
  LOG(INFO) << board.DebugStringWithState(*n);
  Action actions[4];
  while (can_move) {
    // We just landed on curr_pos. We check all the possible rules.
    int num_actions = 0;
    for (int i = 0; i < 4; ++i) {
      const int lookup_dir = DIRECTIONS[DIR_LOOKUP[dir][i]][0];
      const int relation = DIRECTIONS[DIR_LOOKUP[dir][i]][1];
      int lookup_pos = curr_pos + lookup_dir;
      int static_tile_index = n->Find(lookup_pos);
      Action a = board.rules.GetAction(moving_tile->type, 
                                       n->t[static_tile_index].type,
                                       relation);
      if (a.lost) {
        return LOSE;
      }
      if (a.exists) {
        actions[num_actions++] = a;
      }
      if (a.moving_animal_dies || 
          !a.continues ||
          a.moving_new_animal != moving_tile->type) {
        // End of move if we die, change or stop.
        can_move = false;
      }
    }

    // boardon ha fal van, akkor nem kell rule lookup.

    // AHEAD
    int newpos = curr_pos + move;
    int ahead_tile_index = -1;
    if (board.b[newpos] != BLANK) {
      // Wall ahead
      can_move = false;
      if (!num_steps) {
        // Immediately blocked on wall, we don't need to check the rules.
        return 0;
      }
    } else {
      ahead_tile_index = n->Find(newpos);
      if (ahead_tile_index != -1) {
        LOG(1) << "tile ahead " << char('a' + n->t[ahead_tile_index].type) << std::endl;
        // Another tile ahead. Check the rules.
        Action a = board.rules.GetAction(moving_tile->type, 
                                         n->t[ahead_tile_index].type,
                                         Rules::AHEAD);
        can_move = a.continues;
        LOG(INFO) << "B" << board.DebugStringWithState(*n);
        int res = n->ApplyAction(moving_tile_index, ahead_tile_index, a);
        LOG(INFO) << "A" << board.DebugStringWithState(*n);
        if (res == LOSE) {
          return LOSE;
        }
        if (res == WIN) {
          final_res = WIN;
        }
      }
    }
    // SIDE
    for (int i = 0; i < 2; i++) {
      int sidepos = curr_pos + DIRECTIONS[SIDE[dir][i]];
      int static_tile_index = n->Find(sidepos);
      if (static_tile_index != -1) {
        LOG(1) << "tile at " << SIDE[dir][i] <<  " " << sidepos << 
          " |" << char('a' + n->t[static_tile_index].type) << "|" << std::endl;
        // Another tile at the side. Check the rules.
        Action a = board.rules.GetAction(moving_tile->type,
                                         n->t[static_tile_index].type,
                                         Rules::SIDE);
        can_move = a.continues;
        LOG(INFO) << "B" << board.DebugStringWithState(*n);
        int res = n->ApplyAction(moving_tile_index, static_tile_index, a); 
        LOG(INFO) << "A" << board.DebugStringWithState(*n);
        if (res == LOSE) {
          return LOSE;
        }
        if (res == WIN) {
          final_res = WIN;
        }
      }
    }
    // We will move to newpos.
    // But before that we check the rules with "ON" relationship.
    if (can_move && ahead_tile_index != -1) {
      LOG(1) << "on check\n";
      Action a = board.rules.GetAction(moving_tile->type,
                                       n->t[ahead_tile_index].type,
                                       Rules::ON);
      can_move = a.continues;
      LOG(INFO) << "B" << board.DebugStringWithState(*n);
      int res = n->ApplyAction(moving_tile_index, ahead_tile_index, a);
      LOG(INFO) << "A" << board.DebugStringWithState(*n);
      if (res == LOSE) {
        return LOSE;
      }
      if (res == WIN) {
        final_res = WIN;
      }
    }

    if (can_move) {
      // Make the actual move
      LOG(INFO) << "Moving to " << newpos << std::endl;
      LOG(INFO) << board.DebugStringWithState(*n);
      moving_tile->pos = newpos;
      LOG(INFO) << board.DebugStringWithState(*n);
      curr_pos = newpos;
      ++num_steps;
    }
  }
  n->Sort();
  return final_res;
}

int State::ApplyAction(int moving_tile_index, int static_tile_index,
                       const Action& a) {
  LOG(1) << "ApplyAction: " << moving_tile_index << " " << static_tile_index
         << " " << a.continues << "," << a.lost << "," << a.won << "--"
         << a.moving_new_animal << ","
         << a.static_new_animal << "," << a.moving_animal_dies << ","
         << a.static_animal_dies << std::endl;
  if (a.lost) return LOSE;
  t[moving_tile_index].type = a.moving_new_animal;
  t[static_tile_index].type = a.static_new_animal;
  if (a.moving_animal_dies) Erase(moving_tile_index);
  if (a.static_animal_dies) Erase(static_tile_index);
  return a.won;
}

void State::Erase(int tile_index) {
  for (int i = tile_index + 1; i < num_tiles; ++i) {
    t[i - 1] = t[i];
  }
  --num_tiles;
}

void State::Hash(char* hash) const {
  // long long h = 0;
  // for (int i = 0; i < num_tiles; ++i) {
  //   h <<= POSITION_BITS;
  //   h += t[i];
  // }
  // return h;
}

std::string State::GetHistory() const {
  return "";
  // static const char DIRNAME[] = "ULRD";
  // std::string ret;

  // int idx = MAX_HISTORY - 1;
  // while(history[idx] == 0) --idx;

  // // First element.
  // int shift = 62;
  // unsigned long long mask = 3ull << shift;
  // while (!((history[idx] & mask) >> shift)) {
  //   shift -= 2;
  //   mask >>= 2;
  //   //printf ("%llx - %d\n", mask, shift);
  // }
  // while(shift > 0) {
  //   shift -= 2;
  //   mask >>= 2;
  //   ret += DIRNAME[(history[idx] & mask) >> shift];
  // }
  // // Other elements.
  // while(--idx >= 0) {
  //   int shift = 62;
  //   unsigned long long mask = 3ull << shift;
  //   while(shift >= 0) {
  //     ret += DIRNAME[(history[idx] & mask) >> shift];
  //     shift -= 2;
  //     mask >>= 2;
  //   }
  // }
  // return ret;
}

int State::GetHistoryLen() const {
    int idx = MAX_HISTORY - 1;
    while(history[idx] == 0) --idx;

    int ret = sizeof(unsigned long long) * 4 * idx;

    unsigned long long h = history[idx];

    while (h >>= 2)	ret += 1;
    return ret;
}
