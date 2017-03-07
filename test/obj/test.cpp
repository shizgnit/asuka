#include "asuka.hpp"

int main(void) {
  AT::Resource::Manager t;

  t.accept("/usr/lib/asuka/resource");

  AT::Field object = t.input("files/cube1.obj");
  t.release(object);

  exit(0);
}
