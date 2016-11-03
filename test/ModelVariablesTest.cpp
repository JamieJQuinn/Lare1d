#include "catch.hpp"
#include <ModelVariables.hpp>
#include <Constants.hpp>
#include <iostream>

TEST_CASE( "ModelVariables save and load correctly", "[variables]" ) {
  int N = 10;
  std::string filePath = "ModelVariableSaveTest.dat";
  std::remove(filePath.c_str());
  const Constants c(0.0001f, 2.0f, N, 10, 2.0f, 3.0f);
  ModelVariables vars(N);

  real* data = vars.pressure.get();
  for(int i=0; i<5; ++i) {
    data[i] = 4.0f;
  }

  vars.save(filePath.c_str());

  ModelVariables vars2(N);

  vars2.load(filePath.c_str(), c);

  for(int i=0; i<vars.len(); ++i) {
    CHECK(vars.pressure[i] == vars2.pressure[i]);
    CHECK(vars.density[i] == vars2.density[i]);
    CHECK(vars.velocity[i] == vars2.velocity[i]);
  }
}
