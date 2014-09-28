#include "utils.h"

using namespace::std;

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


