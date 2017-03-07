#include "asuka.hpp"

int main(void) {
  std::cout<<"starting up"<<std::endl;

  AT::Resource::Manager t;

  t.accept("/usr/lib/asuka/resource");

  AT::Field cache;
  cache["material"] = undef;
  cache["map"] = undef;
  
  AT::Field object = t.input("obj/apple.obj", cache);

  std::cout<<"=[RESULTS]==================="<<std::endl;
  std::cout<<"vertices:  "<<object["mesh"]["vertex"].size()<<std::endl;
  std::cout<<"faces:     "<<object["mesh"]["face"].size()<<std::endl;
  AT::Field label;
  foreach(label, object["mesh"]["material"].keys()) {
    std::cout<<"- "<<label<<", "<< object["mesh"]["material"][label].size() <<std::endl;
  }

  t.release(object);

  exit(0);
}
