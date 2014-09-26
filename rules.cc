#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>

#include "convert.h"
#include "rules.h"

using namespace std;

void SplitString(const string& s, const string& delim, vector<string> *result) {
    const bool keep_empty = true;
    if (delim.empty()) {
        result->push_back(s);
        return;
    }
    string::const_iterator substart = s.begin(), subend;
    while (true) {
        subend = search(substart, s.end(), delim.begin(), delim.end());
        string temp(substart, subend);
        if (keep_empty || !temp.empty()) {
            result->push_back(temp);
        }
        if (subend == s.end()) {
            break;
        }
        substart = subend + delim.size();
    }
}

Rules::Rules(const std::string& csv_file) {
  static const int ANIMAL_NUM = TriToCode("END") - 'a';
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

    Action action;
    action.prio = atoi(values[6].c_str());
    const string a1 = values[4];
    const string a2 = values[5];
    if (a1 == "L" || a2 == "L") action.lost = 1;
    if (a1 == "W" || a2 == "W") action.won = 1;
    if (a1 == "---") {
      action.moving_animal_dies = 1;
    } else if (a1.length() == 3) {
      char code = TriToCode(a1.c_str()) - 'a';
      if (code < 0 || code > ANIMAL_NUM) {
        cout << "Wrong animal target1: " << a1.c_str() << '/' << line << endl;
        exit(4);
      }
      action.moving_new_animal = code;
    }
    if (a2 == "---") {
      action.static_animal_dies = 1;
    } else if (a2.length() == 3) {
      char code = TriToCode(a2.c_str()) - 'a';
      if (code < 0 || code > ANIMAL_NUM) {
        cout << "Wrong animal target2: " << a2.c_str() << '/' << line << endl;
        exit(4);
      }
      action.static_new_animal = code;
    }


    if (values[1] == "Moving animal") continue;
    int animal1 = TriToCode(values[1].c_str()) - 'a';
    if (animal1 < 0 || animal1 > ANIMAL_NUM) {
      cout << "Wrong animal1: " << values[1] << '/' << line << endl;
      exit(3);
    }
    int animal2 = TriToCode(values[2].c_str()) - 'a';
    if (animal2 < 0 || animal2 > ANIMAL_NUM) {
      cout << "Wrong animal1: " << values[2] << '/' << line << endl;
      exit(3);
    }

  }
}
