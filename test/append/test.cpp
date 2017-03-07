#include "asuka.hpp"

int main(void) {
  std::cerr<<"starting..."<<std::endl;
  AT::Field string;
  for(int i=0; i<40; i++) {
    string.append("..");
    printf("%d: %s\n", i, (char *)string);
  }
}
