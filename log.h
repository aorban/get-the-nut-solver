#ifndef DEBUG
#define DEBUG -1
#endif

namespace {
class Voidify {
 public:
  Voidify() { }
  // This has to be an operator with a precedence lower than << but
  // higher than ?:
  void operator&(std::ostream&) {}
};
}  // namespace

#define LOG(x) (x > DEBUG) ? (void)0 : Voidify() & std::cout
