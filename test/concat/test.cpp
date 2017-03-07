#include "asuka.hpp"

int main(void) {
  AT::Field x = 1;
  AT::Field y = 2;
  AT::Field z = 3;
  
  AT::Field foo = AT::Concat() << "testing " << x << ", " << y << ", " << z;
  
  std::cout<<foo<<std::endl;
  
  exit(0);
}
