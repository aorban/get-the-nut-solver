#ifndef GTN_CONVERT_H_
#define GTN_CONVERT_H_

static char TriToCode(const char *tri) {
  if (strncmp(tri, "ACG", 3) == 0) return 'a';
  if (strncmp(tri, "SQR", 3) == 0) return 'b';
  if (strncmp(tri, "HOG", 3) == 0) return 'c';
  if (strncmp(tri, "MSE", 3) == 0) return 'd';
  if (strncmp(tri, "ACN", 3) == 0) return 'e';
  if (strncmp(tri, "WLF", 3) == 0) return 'f';
  if (strncmp(tri, "BER", 3) == 0) return 'g';
  if (strncmp(tri, "TRP", 3) == 0) return 'h';
  if (strncmp(tri, "ALG", 3) == 0) return 'i';
  if (strncmp(tri, "SWM", 3) == 0) return 'j';
  if (strncmp(tri, "MSS", 3) == 0) return 'k';  // Mouse in swamp
  if (strncmp(tri, "HOS", 3) == 0) return 'l';  // Hog in swamp
  if (strncmp(tri, "SNK", 3) == 0) return 'm';
  if (strncmp(tri, "PRC", 3) == 0) return 'n';
  if (strncmp(tri, "FSN", 3) == 0) return 'o';  // Full snake
  if (strncmp(tri, "END", 3) == 0) return 'p';  // END
  return '?';
}

static inline const char *CodeToTri(const char c) {
  switch (c) {
    case 'a': return "ACG";
    case 'b': return "SQR";
    case 'c': return "HOG";
    case 'd': return "MSE";
    case 'e': return "ACN";
    case 'f': return "WLF";
    case 'g': return "BER";
    case 'h': return "TRP";
    case 'i': return "ALG";
    case 'j': return "SWM";
    case 'k': return "MSS";
    case 'l': return "HOS";
    case 'm': return "SNK";
    case 'n': return "PRC";
    case 'o': return "FSN";
  }
  return "???";
}

#endif
