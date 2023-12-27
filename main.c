#include "main.h"

int main(int argc, char *argv[]) {
#ifdef INTERP
  return interp_main(argc, argv);
#else
  return compiler_main(argc, argv);
#endif
}
