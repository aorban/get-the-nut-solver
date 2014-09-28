#define DEBUG false

class Voidify {
 public:
  Voidify() { }
  // This has to be an operator with a precedence lower than << but
  // higher than ?:
  void operator&(std::ostream&) {}
};
#define LOG(x) !DEBUG ? (void)0 : Voidify() & std::cout

