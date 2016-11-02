#include <ModelVariables.hpp>
#include <Constants.hpp>
#include <Variable.hpp>
#include <RiemannProblem.hpp>
#include <FluxLimiter.hpp>
#include <iostream>

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

void runRemapStep(ModelVariables& vars, const Constants& c) {
  real* rho = vars.density.get();
  real* rhoNew = vars.density.getPlus(1);
  real* dxb = vars.dxb.get();
  real* dxc = vars.dxc.get();
  real* dxbNew = vars.dxb.getPlus(1);
  real* uBar = vars.velocity.getPlus(1); // Recall this is the half step velocity
  real* dM = vars.dM.get();
  real* de = vars.de.get();
  real* eNew = vars.energy.getPlus(1);
  
  // Calculate dM and de
  for(int i = 1; i < vars.len(); ++i) {
    real phi = uBar[i]*c.dt/dxbNew[i];
    real D = FluxLimiter::calcAt(i, phi, rhoNew, uBar[i], dxc, dxb);
    real dedx = FluxLimiter::calcAt(i, phi, eNew, uBar[i], dxc, dxb);
    dM[i] = (rhoNew[i] + dxbNew[i]/2.0f*D*(i-phi))*uBar[i]*c.dt;
    de[i] = (eNew[i] + dxb[i]/2.0f*dedx*(1 - dM[i]/(rhoNew[i]*dxbNew[i])))*dM[i];
  }

  // Calculate new density and energy
  for(int i = 1; i < vars.len(); ++i) {
    rhoNew[i] = rho[i] + (dM[i-1] - dM[i])/dxb[i];
    eNew[i] = (eNew[i]*dxb[i]*rho[i] + de[i-1] - de[i])/(dxb[i]*rhoNew[i]);
  }
}

int main(int argc, char** argv) {
  const Constants c(0.0001f, 2.0f, 10, 2.0f, 3.0f);
  ModelVariables vars(c.nGridPoints);

  setupInitialConditions(vars, c);

  return 0;
}
