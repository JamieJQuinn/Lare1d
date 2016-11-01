#include "RiemannProblem.hpp"

void setupInitialConditions(ModelVariables &vars, const Constants &c) {
  real *pressure = vars.pressure.get();
  real *density = vars.density.get();
  real *velocity = vars.velocity.get();
  for(int i = 0; i < c.nGridPoints*0.5; ++i) {
    pressure[i] = 128.0f;
    density[i] = 256.0f/49.0f;
    velocity[i] = 0.0f;
  }
  for(int i = int(c.nGridPoints*0.5f); i < c.nGridPoints; ++i) {
    pressure[i] = 1.0f;
    density[i] = 2.0f/25.0f;
    velocity[i] = 0.0f;
  }
}
