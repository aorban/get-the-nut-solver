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

test: convert_test
	./convert_test

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

