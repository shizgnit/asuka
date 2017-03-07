cd ../..; scons; cd Linux_2.6.0/ ; cp -r include /usr ; cp -r lib /usr ; cd ../test/xpath ; g++ -lasuka test.cpp ; ./a.out
