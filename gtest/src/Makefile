CC = g++
CFLAGS := -I../.. -I. -I..

.PHONY: all, clean

all: gtest.a gtest_main.a

gtest.a: gtest-all.o
	ar rcs $@ $^

gtest_main.a: gtest-all.o gtest_main.o
	ar rcs $@ $^

gtest-all.o: gtest-all.cc
	$(CC) -c $(CFLAGS) $< -o $@

gtest_main.o: gtest_main.cc
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f *o *a
