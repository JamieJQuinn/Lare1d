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
  real* dxcNew = dxc.getPlus(1);

  for(int i=1; i<vars.len() - 1; ++i) {
    // Calc density at spatial half step
    real rhoSHalf = (dxb[i]*rho[i] + dxb[i+1]*rho[i+1])/(dxb[i] + dxb[i+1]);
    uNew[i] = u[i] - c.dt*(pHalf[i+1] - pHalf[i])/(rhoSHalf*dxc[i]);
    uHalf[i] = (u[i] + uNew[i])/2.0f;
    eNew[i] = e[i] - c.dt*pHalf[i]*(uHalf[i] - uHalf[i-1])/(rho[i]*dxb[i]);
    dxbNew[i] = dxb[i] + c.dt*(uHalf[i] - uHalf[i-1]);
    dxcNew[i] = dxc[i] + c.dt*(uHalf[i+1] - uHalf[i-1])/2.0f;
    rhoNew[i] = rho[i]*dxb[i]/dxbNew[i];
    pNew[i] = calcNewPressure(eNew[i], c.gamma, rho[i], dxb[i], dxbNew[i]);
  }
}

void runRemapStep(ModelVariables& vars, const Constants& c) {
  real* rho = vars.density.get();
  real* rhoNew = vars.density.getPlus(1);
  real* dxb = vars.dxb.get();
  real* dxbNew = vars.dxb.getPlus(1);
  real* dxc = vars.dxc.get();
  real* dxcNew = vars.dxc.getPlus(1);
  real* u = vars.velocity.get();
  real* uBar = vars.velocity.getPlus(1); // Recall this is the half step velocity
  real* uNew = vars.velocity.getPlus(2);
  real* du = vars.du.get();
  real* dM = vars.dM.get();
  real* e = vars.energy.get();
  real* eNew = vars.energy.getPlus(1);
  real* de = vars.de.get();
  
  // Calculate dM and de
  for(int i = 1; i < vars.len()-2; ++i) {
    real phi = uBar[i]*c.dt/dxbNew[i];
    real D = FluxLimiter::calcAt(i, phi, rhoNew, uBar[i], dxcNew, dxbNew);
    real dedx = FluxLimiter::calcAt(i, phi, e, uBar[i], dxc, dxb);
    // NB: dudx calc'd over dxc, hence swap dxc & dxb
    real dudx = FluxLimiter::calcAt(i, phi, u, uBar[i], dxb, dxc);
    dM[i] = (rhoNew[i] + dxbNew[i]/2.0f*D*(i-phi))*uBar[i]*c.dt;
    de[i] = (eNew[i] + dxb[i]/2.0f*dedx*(1 - dM[i]/(rho[i]*dxb[i])))*dM[i];
    du[i] = (uNew[i] + dxc[i]/2.0f*dudx*(1 - dM[i]/(rho[i]*dxc[i])))*dM[i];
  }

  // Calculate new density, energy and velocity
  for(int i = 1; i < vars.len(); ++i) {
    // Important! Density is remapped first
    rhoNew[i] = rho[i] + (dM[i-1] - dM[i])/dxb[i];
    eNew[i] = (eNew[i]*dxb[i]*rho[i] + de[i-1] - de[i])/(dxb[i]*rhoNew[i]);
    uNew[i] = (uNew[i]*dxc[i]*rho[i] + du[i-1] - du[i])/(dxc[i]*rhoNew[i]);
  }
  // Remap grid
  for(int i=0; i < vars.len(); ++i) {
    dxbNew[i] = dxb[i];
    dxcNew[i] = dxc[i];
  }
}

int main(int argc, char** argv) {
  const Constants c(
      0.0001f,
      2.0f,
      300,
      324,
      2.0f,
      3.0f
  );
  ModelVariables vars(c.nGridPoints);

  setupInitialConditions(vars, c);
  for(int n=0; n<c.nTimeSteps; ++n) {
    runPredictorStep(vars, c);
    runCorrectorStep(vars, c);
    runRemapStep(vars, c);
    vars.nextTimestep();
  }

  vars.save("RiemannSoln0.0324.dat");

  return 0;
}
