#include <ModelVariables.hpp>
#include <Constants.hpp>
#include <Variable.hpp>
#include <RiemannProblem.hpp>
#include <SodProblem.hpp>
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
  return (sigma1*cs + sigma2*std::abs(du))*rho*std::abs(du)*int(du<0);
}

real calcPressure(real energy, real gamma, real rho, real du, const Constants& c) {
  real pressure = energy*(gamma-1)*rho;
  pressure += calcHeatingCoeff(c.sigma1, c.sigma2, rho, std::sqrt(c.gamma*pressure/rho), du);
  return pressure;
}

real calcSHalfDensityAt(int i, real* dxBound, real* rho) {
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
    // Calc density at spatial half step
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

    dMPrev = dM;
    real D = FluxLimiter::calcAt(i, phi, rhoNew, uBar, dxCellNew, dxBoundNew);
    dM = (rhoNew[i] + dxBoundNew[i]/2.0f*D*(1-phi))*std::abs(uBar)*c.dt;
    rhoNew[i] = rho[i] + (dMPrev - dM)/dxBound[i];

    dePrev = de;
    real dedx = FluxLimiter::calcAt(i, phi, e.get(), uBar, dxCell.get(), dxBound.get());
    de = (eNew[i] + dxBound[i]/2.0f*dedx*(1 - dM/(rho[i]*dxBound[i])))*dM;
    eNew[i] = (eNew[i]*dxBound[i]*rho[i] + dePrev - de)/(dxBound[i]*rhoNew[i]);

    duPrev = du;
    real rhoSHalf = calcSHalfDensityAt(i-1, dxBound.get(), rho.get());
    real dudx = FluxLimiter::calcAt(i-1, phiPrevSHalf, u.get(), uBarPrevSHalf, dxBound.get(), dxCell.get());
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

void simulateRiemannProblem() {
  real totalTime = 0.0324;
  real dt = 5e-6;
  real gamma = 2.0f;
  real nGridPoints = 800;
  real nTimeSteps = int(totalTime/dt);
  real sigma1 = 2.0f;
  real sigma2 = 3.0f;
  const Constants c(dt, gamma, nGridPoints, nTimeSteps, sigma1, sigma2);
  ModelVariables vars(c);

  RiemannProblem::setupAnalyticalSolution(vars, c);
  vars.save("analyticalSoln.dat");

  RiemannProblem::setupInitialConditions(vars, c);
  for(int n=0; n<=c.nTimeSteps; ++n) {
    //vars.save("RiemannSoln"+std::to_string(n)+".dat");
    runPredictorStep(vars, c);
    runCorrectorStep(vars, c);
    runRemapStep(vars, c);
    vars.nextTimestep();
  }
  vars.save("RiemannSoln.dat");
}

void simulateSodProblem() {
  real totalTime = 0.14154;
  real dt = 1e-5;
  real gamma = 1.4f;
  real nGridPoints = 500;
  real nTimeSteps = int(totalTime/dt);
  real sigma1 = 2.0f;
  real sigma2 = 3.0f;
  const Constants c(dt, gamma, nGridPoints, nTimeSteps, sigma1, sigma2);
  ModelVariables vars(c);

  //SodProblem::setupAnalyticalSolution(vars, c);
  //vars.save("analyticalSoln.dat");

  SodProblem::setupInitialConditions(vars, c);
  for(int n=0; n<=c.nTimeSteps; ++n) {
    //vars.save("RiemannSoln"+std::to_string(n)+".dat");
    runPredictorStep(vars, c);
    runCorrectorStep(vars, c);
    runRemapStep(vars, c);
    vars.nextTimestep();
  }
  vars.save("SodSoln.dat");
}

void simulate(ModelVariables vars, const Constants c) {
  for(int n=0; n<=c.nTimeSteps; ++n) {
    vars.save("num"+to_string(n)+".dat");
    runPredictorStep(vars, c);
    runCorrectorStep(vars, c);
    runRemapStep(vars, c);
    vars.nextTimestep();
  }
}

int main(int argc, char** argv) {
  // parse args
  string optionsStr = "c:i:";
  string constantsFilePath;
  string ICFilePath;

  int opt;
  while( (opt = getopt(argc, argv, optionsStr.c_str())) != -1) {
    switch(opt) {
      case 'c':
        constantsFilePath = optarg;
        break;
      case 'i':
        ICFilePath = optarg;
    }
  }

  Constants c;
  c.readJson(constantsFilePath);
  ModelVariables vars(c);
  vars.load(ICFilePath, c);

  simulate(vars, c);

  return 0;
}
