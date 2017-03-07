#include "asuka.hpp"

int main(void) {
  AT::Field line;
  
  std::cerr<<"list test..."<<std::endl;
  AT::Archive test("files.zip");
  foreach(line, test.list()) {
    std::cerr<<"file: "<<line[12]<<std::endl;
  }
  
  std::cerr<<"file test..."<<std::endl;
  AT::File A = AT::Path::open("/home/codeneko/projects/asuka/test/float/files/A/a.txt");
  foreach(line, A) {
    std::cout<<"A line: "<<line<<std::endl;
  }

  std::cerr<<"archive test..."<<std::endl;
  AT::File B = AT::Path::open("/home/codeneko/projects/asuka/test/float/files/B/b.txt");
  std::cerr<<"back in main "<<B.source()<<std::endl;
  foreach(line, B) {
    std::cout<<"B line: "<<line<<std::endl;
  }
  
  std::cerr<<"relative archive test..."<<std::endl;
  B = AT::Path::open("files/B/b.txt");
  std::cerr<<"back in main "<<B.source()<<std::endl;
  foreach(line, B) {
    std::cout<<"B line: "<<line<<std::endl;
  }
}
