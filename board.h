#ifndef _GTN_BOARD_H__
#define _GTN_BOARD_H__

#include <string>
 
static const int BOARD_X = 10;
static const int BOARD_Y = 8;

static const int BOARD_SIZE = BOARD_X * BOARD_Y;

static const int MAX_TILES = 12;

static const int MAX_HISTORY = 40;

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
  unsigned int pos : 8;
  unsigned int type : 8;
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
  static const char DIRNAME[];
  static const int DIR_LOOKUP[][4][2];
  
  static const int LOSE = 1;
  static const int WIN = 2;

  struct HistoryItem {
    unsigned char tile_index : 6;
    unsigned char dir : 2;
  };

  static const int HASH_SIZE = 2;
  typedef unsigned long long *HashValue;
  struct CmpByHash {
    bool operator()(const HashValue& a, const HashValue& b) const {
      for (int i = 0; i < HASH_SIZE; ++i) {
        if (a[i] != b[i]) {
          return a[i] < b[i];
        }
      }
      return false;
    }
  };

  State();

  explicit State(const char *p);
  // Make a move on a board.
  // dir is the direction (0, 1, 2, 3) or (UP, LEFT, RIGHT, DOWN)
  // n is the new state.
  // Return value: 0 if we are alive, 1 if dead, 2 if won.
  int Move(const Board &board, int tile_index, int dir, State *n) const;
  // Move based constructor.
  State(const Board &board, const State &old_state, int tile_index, int move);

  void Hash(HashValue) const;

  int NumTiles() const { return num_tiles; }
  const Tile& GetTile(int index) const { return t[index]; }

  unsigned int GetSquirrelPos() const { return t[1].pos; }

  const HistoryItem *GetHistory() const;
  int GetHistoryLen() const;

 protected:
  void Initialize(const char *p);
  // --------------------------------------------------
  // Visible for testing

  // Find the tile occupying a position. Return index or -1.
  int Find(int pos) const;

  inline void Sort() {
    qsort((void *)t, num_tiles, sizeof(int), MY_intcmp);
  }

 private:
  friend class Board;
  int num_tiles;
  Tile t[MAX_TILES];      // Tiles.
  unsigned char history_len;
  HistoryItem history[MAX_HISTORY];
};

typedef int Cell;
static const Cell BLANK = ' ';

class Board {
 public:
  explicit Board(const char *i, const Rules& rules);
  std::string DebugString() const;
  std::string DebugStringWithState(const State &s) const;
  std::string DebugStringNice(const State &state) const;
  std::string DebugStringNiceWithMove(const State &state,
                                      int tile_index,
                                      int dir) const;
  std::string DebugStringWithDistance() const;

  // The minimum number of moves required from state.
  int MinMovesFrom(const State &state) const;

 private:
  friend class State;
  Cell b[BOARD_Y * BOARD_X];  // Board.
  const Rules& rules;

  int dist[BOARD_X * BOARD_Y];

  // Compute minimum number of moves to reash golden acorn.
  void ComputeDistances(int acg_pos);

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
