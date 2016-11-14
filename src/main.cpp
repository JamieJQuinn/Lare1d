#include <ModelVariables.hpp>
#include <Constants.hpp>
#include <Variable.hpp>
#include <RiemannProblem.hpp>
#include <FluxLimiter.hpp>
#include <iostream>
#include <cmath>
#include <assert.h>

void log(std::string str) {
  std::cout << str << std::endl;
}

real calcViscosityCoeff(real sigma1, real sigma2, real rho, real cs, real du) {
  return (sigma1*cs + sigma2*std::abs(du))*rho*std::abs(du);
}

real calcPressureAt(int i, real energy, real gamma, real rho, real* u, const Constants& c) {
  real pressure = energy*(gamma-1)*rho;
  if(u[i] - u[i-1] < 0) {
    pressure += calcViscosityCoeff(c.sigma1, c.sigma2, rho, std::sqrt(c.gamma*pressure/rho), u[i]-u[i-1]);
  }
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
  Variable &dxCell = vars.dxCell;
  real* pHalf = p.get();

  for(int i=1; i<vars.len()-1; ++i) {
    real pressureCurrent = calcPressureAt(i, e[i], c.gamma, rho[i], u.get(), c);
    real energyHalf = e[i] - c.dt/2.0f*pressureCurrent*(u[i] - u[i-1])/(rho[i]*dxBound[i]);
    if(u[i] - u[i-1] < 0) {
      //energyHalf -= calcViscosityCoeff(c.sigma1, c.sigma2, rho[i], std::sqrt(c.gamma*pressureCurrent/rho[i]), u[i]-u[i-1])*(u[i]-u[i-1])/(dxCell[i]*rho[i]);
    }
    real dxBoundHalf = dxBound[i] + c.dt/2.0f*(u[i] - u[i-1]);
    real rhoHalf = rho[i]*dxBound[i]/dxBoundHalf;
    pHalf[i] = calcPressureAt(i, energyHalf, c.gamma, rhoHalf, u.get(), c);
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
    if(uNew[i] - uNew[i-1] < 0) {
      real pressureNew = calcPressureAt(i, eNew[i], c.gamma, rhoNew[i], u.get(), c);
      // TODO
      //eNew[i] -= calcViscosityCoeff(c.sigma1, c.sigma2, rhoNew[i], std::sqrt(c.gamma*pressureNew/rhoNew[i]), uNew[i]-uNew[i-1])*(uNew[i]-uNew[i-1])/(dxCellNew[i]*rhoNew[i]);
    }
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
  const Constants c(
      0.0001f,
      2.0f,
      300,
      324,
      2.0f,
      3.0f
  );
  ModelVariables vars(c);

  setupAnalyticalSolution(vars, c);
  vars.save("analyticalSoln"+std::to_string(c.nTimeSteps)+".dat");

  setupInitialConditions(vars, c);
  for(int n=0; n<=c.nTimeSteps; ++n) {
    vars.save("RiemannSoln"+std::to_string(n)+".dat");
    //vars.pressure.print();
    //vars.density.print();
    //vars.velocity.print();
    //vars.energy.print();
    runPredictorStep(vars, c);
    runCorrectorStep(vars, c);
    runRemapStep(vars, c);
    vars.nextTimestep();
  }
}

int main(int argc, char** argv) {
  simulateRiemannProblem();

  return 0;
}
