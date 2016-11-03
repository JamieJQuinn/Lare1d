#include "catch.hpp"
#include <Variable.hpp>

TEST_CASE( "Variables initialize correctly", "[variables]") {
  int length = 10;
  int steps = 2;
  Variable a(length, steps);

  CHECK(a.len() == length);

  real* data = a.get();
  for(int i=0; i<a.len(); ++i) {
    CHECK(data[i] == Approx(0.0f));
  }
}

TEST_CASE( "Variables save and load correctly", "[variables]") {
  int length = 10;
  int steps = 1;
  std::string filePath = "VariableSaveTest.dat";

  std::remove(filePath.c_str());

  Variable a(length, steps);
  Variable b(length, steps);

  real* data = a.get();
  for(int i=0; i<3; ++i) {
    data[i] = 1.0f;
  }
  
  FILE* outFile = fopen(filePath.c_str(), "wb");
  a.writeToFile(outFile, 1);
  fclose(outFile);

  FILE* inFile = fopen(filePath.c_str(), "rb");
  b.readFromFile(inFile, 1);
  fclose(inFile);

  for(int i=0; i<length; ++i) {
    CHECK(a[i] == b[i]);
  }
}

TEST_CASE( "[] operator working correctly" ) {
  int length = 10;
  int steps = 2;
  Variable a(length, steps);

  real* data = a.get();
  data[3] = 1.0f;
  CHECK(data[3] == Approx(a[3]));
  data[3] = 0.0f;
  
  data = a.getPlus(1);
  data[3] = 1.0f;
  a.nextTimestep();
  CHECK(data[3] == Approx(a[3]));
}
