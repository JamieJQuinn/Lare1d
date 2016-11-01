#include "ModelVariables.hpp"
#include "Constants.hpp"
#include <iostream>

void setupInitialConditions(ModelVariables vars, const Constants c) {
  for(int i = 0; i <= c.nGridPoints*0.5; ++i) {
    vars.pressure.get()[i] = 128.0f;
    vars.density.get()[i] = 256.0f/49.0f;
    vars.velocity.get()[i] = 0.0f;
  }
  for(int i = 0; i > c.nGridPoints*0.5 and i < c.nGridPoints; ++i) {
    vars.pressure.get()[i] = 1.0f;
    vars.density.get()[i] = 2.0f/25.0f;
    vars.velocity.get()[i] = 0.0f;
  }
}

int main(int argc, char** argv) {
  const Constants c(0.0001f, 2.0f, 10, 2.0f, 3.0f);
  ModelVariables vars(c.nGridPoints);

  setupInitialConditions(vars, c);

  vars.pressure.printTo(std::cout);
  vars.density.printTo(std::cout);
  vars.velocity.printTo(std::cout);

  vars.save("test.dat");
  vars.load("test.dat");

  vars.pressure.printTo(std::cout);
  vars.density.printTo(std::cout);
  vars.velocity.printTo(std::cout);
  
  return 0;
}
