#include "catch.hpp"
#include "ModelVariables.hpp"

TEST_CASE( "Variables save and load correctly", "[variables]" ) {
  ModelVariables vars(10);

  real* data = vars.pressure.get();
  for(int i=0; i<5; ++i) {
    data[i] = 4.0f;
  }

  vars.save("VariableSaveTest.dat");

  ModelVariables vars2(10);

  vars2.load("VariableSaveTest.dat");

  for(int i=0; i<vars.len(); ++i) {
    REQUIRE(vars.pressure[i] == vars2.pressure[i]);
  }
}
