#ifndef GTN_LOG_H_
#define GTN_LOG_H_

#define DEBUG -2

class Voidify {
 public:
  Voidify() { }
  // This has to be an operator with a precedence lower than << but
  // higher than ?:
  void operator&(std::ostream&) {}
};
#define LOG(x) (x > DEBUG) ? (void)0 : Voidify() & std::cout

#endif
