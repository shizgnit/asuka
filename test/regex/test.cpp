#include "asuka.hpp"

int main(void) {
  AT::Field foo;
  
  AT::Field list;
  AT::Field optional;
  AT::Field requirement;
  AT::Field type;
  
  (list, optional, requirement, type) = AT::rx("^(.+?)((=|:)(\\w))?$", "Folder=s");
  
  std::cout<<"list:        "<<list<<std::endl;
  std::cout<<"requirement: "<<requirement<<std::endl;
  std::cout<<"type:        "<<type<<std::endl;
  
  exit(0);
}
