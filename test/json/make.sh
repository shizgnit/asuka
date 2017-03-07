cd ../..
scons
./install
cd test/json
g++ -lasuka test.cpp 
./a.out
