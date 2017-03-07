#include "asuka.hpp"

int main(void) {
  std::cerr<<"starting..."<<std::endl;

  //AT::Field data = AT::JSON::decode("var testing = {\"key\": [\"fish\", \"taco\"], \"another\": [\"dog\", \"taco\"]}; var string = \"value\"; var integer = 123456; var float = 20.01; var negative = -24;");
  //std::cout<<AT::JSON::encode(data)<<std::endl;
  
  AT::Field data;
  
  data["base"][0]["inner 0"] = "value 0";
  data["base"][1]["inner 1"] = "value 1";
  
  std::cout<<AT::JSON::encode(data, true)<<std::endl;
}
