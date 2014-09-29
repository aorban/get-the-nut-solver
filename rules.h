#ifndef GTN_RULES_H_
#define GTN_RULES_H_

#include <string>
#include <vector>

struct Action {
  unsigned int dummy_ : 11;
  unsigned int moving_new_animal : 8;
  unsigned int static_new_animal : 8;
  unsigned int won : 1;
  unsigned int lost : 1;
  unsigned int prio : 2;
  unsigned int exists : 1;  // Prio+exists has to be last for sort to work.
};

std::string PrintAction(const Action& a);

std::string PrintActions(const Action *a, int num_action);

class Rules {
 public:
  static const int NUM_ANIMALS = 20;
  static const int NUM_RELATIONS = 3;
  static const int ON = 0;
  static const int AHEAD = 1;
  static const int SIDE = 2;

  // Simple rules without interactions.
  Rules();

  // Reads the rules from a csv file:
  explicit Rules(const std::string& csv_file);
 
  virtual Action GetAction(int moving_animal, int static_animal, int relation) const {
    return rules[moving_animal][static_animal][relation];
  }

 protected:
  // dim: allat1 x allat2 x relation
  Action rules[NUM_ANIMALS][NUM_ANIMALS][NUM_RELATIONS];

  void Initialize();
};

void SplitString(
    const std::string& s,
    const std::string& delim,
    std::vector<std::string> *result);

#endif
