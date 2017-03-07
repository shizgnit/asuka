#include "asuka.hpp"

int main(void) {
  std::cerr<<"starting..."<<std::endl;

  AT::DOM::Document xml;

  xml.loadXML(AT::File("test.xml"));

  std::cerr<<"contents: "<<xml.serialize(true)<<std::endl;

  AT::Field element;
  foreach(element, xml.selectNodes("//wafer[@attr='1' or @attr='2']/checked")) {
    std::cerr<<"wafer: "<<element["nodeName"]<<std::endl;
  }

  AT::Field test;
  test["food"] = "crap";
  test["something"] = "nothing";

  std::cerr<<"values: "<<AT::join(", ", test.values())<<std::endl;
}
