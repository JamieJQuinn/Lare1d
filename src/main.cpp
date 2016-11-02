#include <ModelVariables.hpp>
#include <Constants.hpp>
#include <Variable.hpp>
#include <iostream>
#include <RiemannProblem.hpp>

int main(int argc, char** argv) {
  const Constants c(0.0001f, 2.0f, 10, 2.0f, 3.0f);
  ModelVariables vars(c.nGridPoints);

  setupInitialConditions(vars, c);

  vars.pressure.printTo(std::cout);
  vars.density.printTo(std::cout);
  vars.velocity.printTo(std::cout);

  vars.load("test.dat");

  vars.pressure.printTo(std::cout);
  vars.density.printTo(std::cout);
  vars.velocity.printTo(std::cout);
  
  return 0;
}
