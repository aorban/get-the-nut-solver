#include <stdio.h>

typedef struct {
  unsigned int A : 8;
  unsigned int B : 8;
  unsigned int C : 8;
  unsigned int D : 8;
} BitField;


int main(int argc, char **argv) {
  BitField bf;
  bf.A = 0x11;
  bf.B = 0x22;
  bf.C = 0x33;
  bf.D = 0x44;

  int *p = (int*)&bf;

  printf("%x\n", *p);
  return 0;
}
