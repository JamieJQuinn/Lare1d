#include <RiemannProblem.hpp>
#include <Precision.hpp>
#include <cmath>

using std::pow;

void setupInitialConditions(ModelVariables &vars, const Constants &c) {
  real *pressure = vars.pressure.get();
  real *density = vars.density.get();
  real *velocity = vars.velocity.get();
  real *energy = vars.energy.get();

  for(int i = 0; i < c.nGridPoints*0.5f; ++i) {
    pressure[i] = 128.0f;
    density[i] = 256.0f/49.0f;
    velocity[i] = 0.0f;
  }
  for(int i = int(c.nGridPoints*0.5f); i < c.nGridPoints; ++i) {
    pressure[i] = 1.0f;
    density[i] = 2.0f/25.0f;
    velocity[i] = 0.0f;
  }

  for(int i=0; i<vars.len(); ++i) {
    energy[i] = pressure[i]/(density[i]*(c.gamma-1));
  }
}

void setupAnalyticalSolution(ModelVariables &vars, const Constants &c) {
  real t = c.dt*c.nTimeSteps;
  real *pressure = vars.pressure.get();
  real *density = vars.density.get();
  real *velocity = vars.velocity.get();
  for(int i = 0; i < c.nGridPoints*(0.5f - 7.0f*t); ++i) {
    pressure[i] = 128.0f;
    density[i] = 256.0f/49.0f;
    velocity[i] = 0.0f;
  }
  for(int i = int(c.nGridPoints*(0.5f - 7.0f*t)); i < c.nGridPoints*(0.5f + 7.0f/2.0f*t); ++i) {
    real x = real(i)/c.nGridPoints;
    pressure[i] = 128.0f*pow(2.0f/3.0f-1.0f/21.0f*(x-0.5f)/t, 4);
    density[i] = 256.0f/49.0f*pow(2.0f/3.0f-1.0f/21.0f*(x-0.5f)/t, 2);
    velocity[i] = 2.0f/3.0f*(x/t+7.0f-1.0f/(2.0f*t));;
  }
  for(int i = int(c.nGridPoints*(0.5f + 7.0f/2.0f*t)); i < c.nGridPoints*(0.5f + 7.0f*t); ++i) {
    pressure[i] = 8.0f;
    density[i] = 64.0f/49.0f;
    velocity[i] = 7.0f;
  }
  for(int i = int(c.nGridPoints*(0.5f + 7.0f*t)); i < c.nGridPoints*(0.5f + 25.0f/2.0f*t); ++i) {
    pressure[i] = 8.0f;
    density[i] = 2.0f/11.0f;
    velocity[i] = 7.0f;
  }
  for(int i = int(c.nGridPoints*(0.5f + 25.0f/2.0f*t)); i < c.nGridPoints; ++i) {
    pressure[i] = 1.0f;
    density[i] = 2.0f/25.0f;
    velocity[i] = 0.0f;
  }
}
