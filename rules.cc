#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>

#include "convert.h"
#include "rules.h"

using namespace std;

string PrintAction(const Action& a) {
  stringstream ss;
  ss << "exists/won/lost/prio/movingNew/staticNew:"
     << a.exists << "/"
     << a.won << "/"
     << a.lost << "/"
     << a.prio << "/"
     << char('a' + a.moving_new_animal)
     << "(" << CodeToTri(char('a' + a.moving_new_animal)) << ")" << "/"
     << char('a' + a.static_new_animal)
     << "(" << CodeToTri(char('a' + a.static_new_animal)) << ")" << endl;
  return ss.str();
}

void Rules::Initialize() {
  Action action;
  action.won = 0;
  action.lost = 0;
  action.prio = 3;
  action.exists = 1;

  for (int a1 = 0; a1 < NUM_ANIMALS; ++a1) {
    for (int a2 = 0; a2 < NUM_ANIMALS; ++a2) {
      for (int r = 0; r < NUM_RELATIONS; ++r) {
        if (r != AHEAD) {
          // No default rule.
          *((unsigned int *)&(rules[a1][a2][r])) = 0;
        } else {
          // AHEAD
          if (a2 == TriToCode("SWM") - 'a') {
            // SWAMP
            *((unsigned int *)&(rules[a1][a2][r])) = 0;
          } else {
            // Not SWAMP.
            action.moving_new_animal = a1;
            action.static_new_animal = a2;
            rules[a1][a2][r] = action;
          }
        }
      }
    }
  }
}

Rules::Rules() {
  Initialize();
}

Rules::Rules(const std::string& csv_file) {
  Initialize();
  std::string line;
  ifstream file(csv_file);
  if (!file) {
    cout << "Couldn't open file: " << csv_file << endl;
    exit(1);
  }
  while (getline(file, line)) {
    vector<string> values;
    SplitString(line, ",", &values);
    if (values.size() != 8) {
      cout << "Broken line in csv: " << line;
      exit(2);
    }
    if (values[1] == "Moving animal") continue;

    Action action;
    *((int*)&action) = 0;
    action.exists = 1;
    action.prio = atoi(values[6].c_str());

    int animal1 = TriToCode(values[1].c_str()) - 'a';
    if (animal1 < 0 || animal1 > NUM_ANIMALS) {
      cout << "Wrong animal1: " << values[1] << '/' << line << endl;
      exit(3);
    }
    action.moving_new_animal = animal1;

    int animal2 = TriToCode(values[2].c_str()) - 'a';
    if (animal2 < 0 || animal2 > NUM_ANIMALS) {
      cout << "Wrong animal1: " << values[2] << '/' << line << endl;
      exit(3);
    }
    action.static_new_animal = animal2;

    const string a1 = values[4];
    const string a2 = values[5];
    if (a1 == "L" || a2 == "L") action.lost = 1;
    if (a1 == "W" || a2 == "W") action.won = 1;
    if (a1.length() == 3) {
      char code = TriToCode(a1.c_str()) - 'a';
      if (code < 0 || code > NUM_ANIMALS) {
        cout << "Wrong animal target1: " << a1.c_str() << '/' << line << endl;
        exit(4);
      }
      action.moving_new_animal = code;
    }
    if (a2.length() == 3) {
      char code = TriToCode(a2.c_str()) - 'a';
      if (code < 0 || code > NUM_ANIMALS) {
        cout << "Wrong animal target2: " << a2.c_str() << '/' << line << endl;
        exit(4);
      }
      action.static_new_animal = code;
    }

    vector<int> relations;
    if (values[3] == "ON") {
      int a = ON; relations.push_back(a);
    } else if (values[3] == "AHEAD") {
      int a = AHEAD; relations.push_back(a);
    } else if (values[3] == "SIDE") {
      int a = SIDE; relations.push_back(a);
    } else if (values[3] == "DC") {
      int a;
      a = ON; relations.push_back(a);
      a = AHEAD; relations.push_back(a);
      a = SIDE; relations.push_back(a);
    }

    for (vector<int>::const_iterator i = relations.begin();
         i != relations.end(); ++i) {
      rules[animal1][animal2][*i] = action;
    }
    // Symmetry.
    if (values[7] == "Y") {
      {
        // Swap new animals.
        int tmp = action.moving_new_animal;
        action.moving_new_animal = action.static_new_animal;
        action.static_new_animal = tmp;
      }
      for (vector<int>::const_iterator i = relations.begin();
           i != relations.end(); ++i) {
        rules[animal2][animal1][*i] = action;
      }
    } else if (values[7] != "N") {
      cout << "Wrong symmetry: " << values[7] << ':' << line << endl;
      exit(7);
    }
  }
}
