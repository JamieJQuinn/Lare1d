#include <ModelVariables.hpp>
#include <Constants.hpp>
#include <Variable.hpp>
#include <FluxLimiter.hpp>
#include <iostream>
#include <cmath>
#include <assert.h>
#include <json.hpp>
#include <unistd.h> 

using namespace std;

void log(std::string str) {
  std::cout << str << std::endl;
}

real calcHeatingCoeff(real sigma1, real sigma2, real rho, real cs, real du) {
  // Calculates heating from artificial shock viscosity
  return (sigma1*cs + sigma2*std::abs(du))*rho*std::abs(du)*int(du<0);
}

real calcPressure(real energy, real gamma, real rho, real du, const Constants& c) {
  // Actual pressure
  real pressure = energy*(gamma-1)*rho;
  // Shock viscosity
  pressure += calcHeatingCoeff(c.sigma1, c.sigma2, rho, std::sqrt(c.gamma*pressure/rho), du);
  return pressure;
}

real calcSHalfDensityAt(int i, real* dxBound, real* rho) {
  // interpolate density from i & i+1 to i+1/2 conserving mass
  return (dxBound[i]*rho[i] + dxBound[i+1]*rho[i+1])/(dxBound[i] + dxBound[i+1]);
}

real mean(real a, real b) {
  return (a+b)/2.0f;
}

void runPredictorStep(ModelVariables& vars, const Constants& c) {
  Variable &p = vars.pressure;
  Variable &u = vars.velocity;
  Variable &e = vars.energy;
  Variable &rho = vars.density;
  Variable &dxBound = vars.dxBound;
  real* pHalf = p.get();

  for(int i=1; i<vars.len()-1; ++i) {
    real pressureCurrent = calcPressure(e[i], c.gamma, rho[i], u[i]-u[i-1], c);
    real energyHalf = e[i] - c.dt/2.0f*pressureCurrent*(u[i] - u[i-1])/(rho[i]*dxBound[i]);
    real dxBoundHalf = dxBound[i] + c.dt/2.0f*(u[i] - u[i-1]);
    real rhoHalf = rho[i]*dxBound[i]/dxBoundHalf;
    pHalf[i] = calcPressure(energyHalf, c.gamma, rhoHalf, u[i]-u[i-1], c);
  }
}

void runCorrectorStep(ModelVariables& vars, const Constants& c) {
  Variable &pHalf = vars.pressure;
  Variable &u = vars.velocity;
  real* uNew = u.getPlus(1);
  Variable &e = vars.energy;
  real* eNew = e.getPlus(1);
  Variable &rho = vars.density;
  real* rhoNew = rho.getPlus(1);
  Variable &dxBound = vars.dxBound;
  real* dxBoundNew = dxBound.getPlus(1);
  Variable &dxCell = vars.dxCell;
  real* dxCellNew = dxCell.getPlus(1);

  real uHalf, uHalfPrev;
  for(int i=1; i<vars.len() - 1; ++i) {
    // interpolate density between grid points for u calculation
    real rhoSHalf = calcSHalfDensityAt(i, dxBound.get(), rho.get());
    uNew[i] = u[i] - c.dt*(pHalf[i+1] - pHalf[i])/(rhoSHalf*dxCell[i]);
    uHalfPrev = uHalf;
    uHalf = mean(u[i], uNew[i]);
    dxBoundNew[i] = dxBound[i] + c.dt*(uHalf - uHalfPrev);
    dxCellNew[i-1] = mean(dxBoundNew[i-1], dxBoundNew[i]);
    rhoNew[i] = rho[i]*dxBound[i]/dxBoundNew[i];
    eNew[i] = e[i] - c.dt*pHalf[i]*(uHalf - uHalfPrev)/(rho[i]*dxBound[i]);
  }
}

void runRemapStep(ModelVariables& vars, const Constants& c) {
  Variable& rho = vars.density;
  real* rhoNew = vars.density.getPlus(1);
  Variable& dxBound = vars.dxBound;
  real* dxBoundNew = vars.dxBound.getPlus(1);
  Variable& dxCell = vars.dxCell;
  real* dxCellNew = vars.dxCell.getPlus(1);
  Variable& u = vars.velocity;
  real* uNew = vars.velocity.getPlus(1);
  Variable& e = vars.energy;
  real* eNew = vars.energy.getPlus(1);

  real dM, dMPrev, de, dePrev, du, duPrev;
  for(int i=2; i<vars.len()-2; ++i) {
    real uBar = mean(u[i], uNew[i]);
    real phi = std::abs(uBar)*c.dt/dxBoundNew[i];
    real uBarPrevSHalf = mean(uBar, mean(u[i-1], uNew[i-1]));
    real phiPrevSHalf = std::abs(uBarPrevSHalf)*c.dt/dxCellNew[i-1];

    // Remap density
    dMPrev = dM;
    real D = FluxLimiter::calcAt(i, phi, rhoNew, uBar, dxCellNew, dxBoundNew);
    dM = (rhoNew[i] + dxBoundNew[i]/2.0f*D*(1-phi))*std::abs(uBar)*c.dt;
    rhoNew[i] = rho[i] + (dMPrev - dM)/dxBound[i];

    // Remap energy
    dePrev = de;
    real dedx = FluxLimiter::calcAt(i, phi, e.get(), uBar, dxCell.get(), dxBound.get());
    de = (eNew[i] + dxBound[i]/2.0f*dedx*(1 - dM/(rho[i]*dxBound[i])))*dM;
    eNew[i] = (eNew[i]*dxBound[i]*rho[i] + dePrev - de)/(dxBound[i]*rhoNew[i]);

    // Remap velocity  
    // u calc'd at i-1 because it relies on ith calculation of rho
    duPrev = du;
    real rhoSHalf = calcSHalfDensityAt(i-1, dxBound.get(), rho.get());
    real dudx = FluxLimiter::calcAt(i-1, phiPrevSHalf, u.get(), uBarPrevSHalf, dxBoundNew, dxCellNew;
    real dMHalf = mean(dM, dMPrev);
    du = (uNew[i-1] + dxCell[i-1]/2.0f*dudx*(1 - dMHalf/(rhoSHalf*dxCell[i-1])))*dMHalf;
    uNew[i-1] = (uNew[i-1]*dxCell[i-1]*rhoSHalf + duPrev - du)/(dxCell[i-1]*calcSHalfDensityAt(i-1, dxBoundNew, rhoNew));
  }

  // Remap grid
  for(int i=0; i < vars.len(); ++i) {
    dxBoundNew[i] = dxBound[i];
    dxCellNew[i] = dxCell[i];
  }
}

void simulate(ModelVariables& vars, const Constants& c) {
  vars.save("numerical0.dat");
  for(int n=0; n<c.nTimeSteps; ++n) {
    runPredictorStep(vars, c);
    runCorrectorStep(vars, c);
    runRemapStep(vars, c);
    vars.save("numerical"+to_string(n+1)+".dat");
    vars.nextTimestep();
  }
}

int main(int argc, char** argv) {
  // parse args
  string optionsStr = "ci";

  // Default location is current dir
  string constantsFilePath = "constants.json";
  string ICFilePath = "initial_conditions.dat";

  int opt;
  while( (opt = getopt(argc, argv, optionsStr.c_str())) != -1) {
    switch(opt) {
      case 'c':
        constantsFilePath = optarg;
        break;
      case 'i':
        ICFilePath = optarg;
        break;
    }
  }

  // Load constants & initial conditions
  Constants c;
  c.readJson(constantsFilePath);
  ModelVariables vars(c);
  vars.load(ICFilePath, c);

  simulate(vars, c);

  return 0;
}
