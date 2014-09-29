C      =       gcc
CXX     =       g++
LINK = g++
ECHO := echo
CFLAGS := -x c -std=gnu99 -O3
CCFLAGS := -x c++ -std=gnu++0x -I. -I/usr/include -Wno-write-strings -O3
LDFLAGS := -L/usr/lib -lpthread -lstdc++
GTEST = ./gtest/src/gtest_main.a

.PHONY: all, test, force_look, clean

all: test

test: convert_test rules_test board_test solve_test
	./convert_test
	./rules_test
	./board_test
	./solve_test

RESULTS.csv: run.sh solve
	./run.sh > $@

force_look :
	true

clean:
	rm -f *.o
	rm -f *.s
	rm -f *_test

$(GTEST): force_look
	$(ECHO) looking into gtest/src : $(MAKE) $(MFLAGS)
	cd gtest/src; $(MAKE) $(MFLAGS)

convert_test.o: convert_test.cc convert.h
	$(CXX) $(CCFLAGS) -c $< -o $@
convert_test: convert_test.o $(GTEST)
	$(LINK) -o $@ $^ $(LDFLAGS)

utils.o: utils.cc utils.h convert.h
	$(CXX) $(CCFLAGS) -c $< -o $@

rules.o: rules.cc rules.h convert.h utils.o
	$(CXX) $(CCFLAGS) -c $< -o $@
rules_test.o: rules_test.cc rules.h
	$(CXX) $(CCFLAGS) -c $< -o $@
rules_test: rules_test.o rules.o utils.o $(GTEST)
	$(LINK) -o $@ $^ $(LDFLAGS)

################################################################################
# board
################################################################################

board.o: board.cc board.h rules.o rules.h log.h convert.h
	$(CXX) $(CCFLAGS) -c $< -o $@ 

board_test.o: board_test.cc board.h
	$(CXX) $(CCFLAGS) -c $< -o $@

board_test: board_test.o board.o rules.o utils.o $(GTEST)
	$(LINK) -o $@ $^ $(LDFLAGS)


################################################################################
# Misc
################################################################################

bitfield: bitfield.c
	$(C) $(CFLAGS) $< -o $@

################################################################################
# solve
################################################################################

solve.o: solve.cc solve.h board.h log.h convert.h
	$(CXX) $(CCFLAGS) -c $< -o $@

solve_test.o: solve_test.cc solve.h testboards.h
	$(CXX) $(CCFLAGS) -c $< -o $@

solve_test: solve.o solve_test.o board.o rules.o utils.o $(GTEST)
	$(LINK) -o $@ $^ $(LDFLAGS)

solve: solver_main.o solve.o board.o utils.o rules.o
	$(LINK) -o $@ $^ $(LDFLAGS)
