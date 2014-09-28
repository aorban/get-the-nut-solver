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

test: convert_test rules_test
	./convert_test
	./rules_test

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

rules.o: rules.cc rules.h convert.h
	$(CXX) $(CCFLAGS) -c $< -o $@
rules_test.o: rules_test.cc rules.h
	$(CXX) $(CCFLAGS) -c $< -o $@
rules_test: rules_test.o rules.o $(GTEST)
	$(LINK) -o $@ $^ $(LDFLAGS)

################################################################################
# board
################################################################################

board.o: board.cc board.h rules.o rules.h log.h
	$(CXX) $(CCFLAGS) -c $< -o $@ 

board_test.o: board_test.cc board.h
	$(CXX) $(CCFLAGS) -c $< -o $@

board_test: board_test.o board.o rules.o $(GTEST)
	$(LINK) -o $@ $^ $(LDFLAGS)


################################################################################
# Misc
################################################################################

bitfield: bitfield.c
	$(C) $(CFLAGS) $< -o $@
