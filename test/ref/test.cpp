#include "asuka.hpp"

int main(void) {
  AT::Field foo;

  foo[0] = &foo[1];
  foo[0] = "foo";

  std::cout<<"foo[0]"<<foo[0]<<std::endl;
  std::cout<<"foo[1]"<<foo[1]<<std::endl;

  AT::Field bar = foo;

  std::cout<<"bar[0]"<<bar[0]<<std::endl;
  std::cout<<"bar[1]"<<bar[1]<<std::endl;

  bar[0] = "bar";

  std::cout<<"bar[0]"<<bar[0]<<std::endl;
  std::cout<<"bar[1]"<<bar[1]<<std::endl;

  AT::Field data = "data";

  foo[0] = &data;
  foo[1] = &data;

  std::cout<<"foo[0]"<<foo[0]<<std::endl;
  std::cout<<"foo[1]"<<foo[1]<<std::endl;

  data = "changed";

  std::cout<<"foo[0]"<<foo[0]<<std::endl;
  std::cout<<"foo[1]"<<foo[1]<<std::endl;

  bar = foo;

  std::cout<<"bar[0]"<<bar[0]<<std::endl;
  std::cout<<"bar[1]"<<bar[1]<<std::endl;

  data = "not";

  std::cout<<"bar[0]"<<bar[0]<<std::endl;
  std::cout<<"bar[1]"<<bar[1]<<std::endl;

  bar[0] = "new";

  std::cout<<"bar[0]"<<bar[0]<<std::endl;
  std::cout<<"bar[1]"<<bar[1]<<std::endl;

  exit(0);
}
