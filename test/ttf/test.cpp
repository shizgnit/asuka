#include "asuka.hpp"

int main(void) {
  std::cout<<"starting up"<<std::endl;

  AT::Resource::Manager t;

  t.accept("/usr/lib/asuka/resource");

  AT::Field font = t.input("test.ttf");
  t.release(font);

  exit(0);
}
