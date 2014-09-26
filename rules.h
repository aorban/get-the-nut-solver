#ifndef GTN_RULES_H_
#define GTN_RULES_H_

#include <string>
#include <vector>

struct Action {
  unsigned int moving_new_animal : 8;
  unsigned int static_new_animal : 8;
  unsigned int moving_animal_dies : 1;
  unsigned int static_animal_dies : 1;
  unsigned int won : 1;
  unsigned int lost : 1;
  unsigned int continues : 1;
  unsigned int prio : 2;
};

class Rules {
 public:
  static const int NUM_ANIMALS = 20;
  static const int NUM_RELATIONS = 3;
  static const int ON = 0;
  static const int AHEAD = 1;
  static const int SIDE = 2;
  // Reads the rules from a csv file:
  // TODO(aorban): format desc
  explicit Rules(const std::string& csv_file);
 
  Action GetAction(int moving_animal, int static_animal, int relation) {
    return rules[moving_animal][static_animal][relation];
  }

 private:  
  // dim: allat1 x allat2 x relation
  Action rules[NUM_ANIMALS][NUM_ANIMALS][NUM_RELATIONS];
};

void SplitString(
    const std::string& s,
    const std::string& delim,
    std::vector<std::string> *result);

#endif
