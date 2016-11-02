#include <ModelVariables.hpp>
#include <Constants.hpp>
#include <Variable.hpp>
#include <iostream>
#include <RiemannProblem.hpp>

inline real calcNewPressure(real eNew, real gamma, real rho, real dxb, real dxbNew) {
  return eNew*(gamma-1)*rho*dxb/dxbNew;
}

void runPredictorStep(ModelVariables& vars, const Constants& c) {
  Variable &p = vars.pressure;
  Variable &u = vars.velocity;
  Variable &e = vars.energy;
  Variable &rho = vars.density;
  Variable &dxb = vars.dxb;
  real* pHalf = p.get();

  for(int i=1; i<vars.len(); ++i) {
    real energyHalf = e[i] - c.dt/2.0f*p[i]*(u[i] - u[i-1])/(rho[i]*dxb[i]);
    real dxbHalf = dxb[i] + c.dt/2.0f*(u[i] - u[i-1]);
    pHalf[i] = calcNewPressure(energyHalf, c.gamma, rho[i], dxb[i], dxbHalf);
  }
}

void runCorrectorStep(ModelVariables& vars, const Constants& c) {
  Variable &pHalf = vars.pressure; // No getPlus because we've set current pressure to half step
  real* pNew = pHalf.get(); // This is for setting new pressure after we're done with half step

  Variable &u = vars.velocity;
  real* uHalf = u.getPlus(1);
  real* uNew = u.getPlus(2);

  Variable &e = vars.energy;
  real* eNew = e.getPlus(1);

  Variable &rho = vars.density;
  real* rhoNew = rho.getPlus(1);

  Variable &dxb = vars.dxb;
  real* dxbNew = dxb.getPlus(1);

  Variable &dxc = vars.dxc;

  for(int i=1; i<vars.len() - 1; ++i) {
    // Calc density at spatial half step
    real rhoSHalf = (dxb[i]*rho[i] + dxb[i+1]*rho[i+1])/(dxb[i] + dxb[i+1]);
    uNew[i] = u[i] - c.dt*(pHalf[i+1] - pHalf[i])/(rhoSHalf*dxc[i]);
    uHalf[i] = (u[i] + uNew[i])/2.0f;
    eNew[i] = e[i] - c.dt*pHalf[i]*(uHalf[i] - uHalf[i-1])/(rho[i]*dxb[i]);
    dxbNew[i] = dxb[i] + c.dt*(uHalf[i] - uHalf[i-1]);
    rhoNew[i] = rho[i]*dxb[i]/dxbNew[i];
    pNew[i] = calcNewPressure(eNew[i], c.gamma, rho[i], dxb[i], dxbNew[i]);
  }
}

int main(int argc, char** argv) {
  const Constants c(0.0001f, 2.0f, 10, 2.0f, 3.0f);
  ModelVariables vars(c.nGridPoints);

  setupInitialConditions(vars, c);

  return 0;
}
