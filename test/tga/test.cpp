#include "asuka.hpp"

int main(void) {
  cout<<"starting up"<<endl;

  AT::Resource::Manager t;

  t.accept("/usr/lib/asuka/resource");
  for(int i=0; i<10; i++) {
    AT::Field image = t.input("a24bit.tga");
    cout<<"=["<<i<<"]============================"<<endl; 
    cout<<"width:  "<<image["header"]["width"]<<endl;
    cout<<"height: "<<image["header"]["height"]<<endl;
    cout<<"bpp:    "<<image["header"]["bpp"]<<endl;
    t.release(image);
  }

  exit(0);
}
