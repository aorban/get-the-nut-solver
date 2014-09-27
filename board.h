#ifndef _GTN_BOARD_H__
#define _GTN_BOARD_H__

#include <string>
 
static const int BOARD_X = 10;
static const int BOARD_Y = 8;

static const int BOARD_SIZE = BOARD_X * BOARD_Y;
static const int TILE_BITS = 16;  // 2^TILE_BITS > BOARD_SIZE

static const int MAX_TILES = 16;

static const int MAX_HISTORY = 2;  // times 32 moves.

static inline int POS(int Y, int X) {
  return Y * BOARD_X + X;
}

static inline int OPPOSITE(int dir) {
  return 3-dir;
}

static inline int MY_intcmp(const void *aa, const void *bb) {
  return ( *(int*)aa - *(int*)bb );
}

struct Tile {
  unsigned int dummy_: 16;
  unsigned int type : 8;
  unsigned int pos : 8;
};

class Action;
class Board;
class Rules;

class State {
 public:
  static const int UP = 0;
  static const int LEFT = 1;
  static const int RIGHT = 2;
  static const int DOWN = 3;
  static const int DIRECTIONS[];
  static const int SIDE[][2];
  
  static const int LOSE = 1;
  static const int WIN = 2;

  State();

  explicit State(const char *p);
  // Make a move on a board.
  // dir is the direction (0, 1, 2, 3) or (UP, LEFT, RIGHT, DOWN)
  // n is the new state.
  // Return value: 0 if we are alive, 1 if dead, 2 if won.
  int Move(const Board &board, int tile_index, int dir, State *n) const;
  // Move based constructor.
  State(const Board &board, const State &old_state, int tile_index, int move);

  void Hash(char* hash) const;

  int NumTiles() const { return num_tiles; }

  unsigned int GetSquirrelPos() const { return t[0].pos; }

  std::string GetHistory() const;
  int GetHistoryLen() const;

 protected:
  void Initialize(const char *p);
  // --------------------------------------------------
  // Visible for testing

  // Find the tile occupying a position. Return index or -1.
  int Find(int pos) const;
  
  int ApplyAction(int moving_tile_index, int static_tile_index, const Action& a);
  void Erase(int tile_index);

  inline void Sort() {
    qsort((void *)t, num_tiles, sizeof(int), MY_intcmp);
  }

 private:
  friend class Board;
  int num_tiles;
  unsigned long long history[MAX_HISTORY];
  Tile t[MAX_TILES];      // Tiles.
};

typedef int Cell;
static const Cell BLANK = ' ';

class Board {
 public:
  explicit Board(const char *i, const Rules& rules);
  std::string DebugString() const;
  std::string DebugStringWithState(const State &s) const;
  std::string DebugStringWithDistance(int pos) const;

  // The minimum number of moves required from state.
  int MinMovesFrom(const State &state) const;

 protected:
  int ***Dist() { return (int***)dist; }

 private:
  friend class State;
  Cell b[BOARD_Y * BOARD_X];  // Board.
  const Rules& rules;

  // Info used for lower bound on remaining steps to goal.
  struct MinMoveInfo {
    // Minimum steps for the squirrel if it could move as a rook.
    int squirrel_step;
    // 0-1 valued. Indicates cells that block all paths to goal.
    int is_blocking[BOARD_Y * BOARD_X];
  };
  // For each possible squirrel position.
  MinMoveInfo dist[BOARD_Y * BOARD_X];

  /* void ComputeDistances(int tile, int dir); */
  /* void FloodFrom(int tile, int pos, int dir); */
  /* int FindMinimum(int tile, int dir); */

  std::string DebugString(const State *state) const;
};

inline int MoveFromChar(char di) {
  switch(di) {
    case 'U': return 0;
    case 'L': return 1;
    case 'R': return 2;
    default: return 3;
  }
}

#endif
