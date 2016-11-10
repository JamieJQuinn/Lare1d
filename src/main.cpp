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

real calcPressure(real energy, real gamma, real rho) {
  //if(u[i] - u[i-1] < 0) {
    //pNew[i] += c.sigma1*rho[i]*std::sqrt(c.gamma*pNew[i]/rho[i])*(u[i]-u[i-1])
      //+ c.sigma2*rho[i]*std::pow(u[i]-u[i-1], 2);
  //}
  return energy*(gamma-1)*rho;
}

real calcSHalfDensityAt(int i, real* dxb, real* rho) {
  return (dxb[i]*rho[i] + dxb[i+1]*rho[i+1])/(dxb[i] + dxb[i+1]);
}

real mean(real a, real b) {
  return (a+b)/2.0f;
}

void runPredictorStep(ModelVariables& vars, const Constants& c) {
  Variable &p = vars.pressure;
  Variable &u = vars.velocity;
  Variable &e = vars.energy;
  Variable &rho = vars.density;
  Variable &dxb = vars.dxb;
  real* pHalf = p.get();

  for(int i=1; i<vars.len()-1; ++i) {
    real pressureCurrent = calcPressure(e[i], c.gamma, rho[i]);
    real energyHalf = e[i] - c.dt/2.0f*pressureCurrent*(u[i] - u[i-1])/(rho[i]*dxb[i]);
    real dxbHalf = dxb[i] + c.dt/2.0f*(u[i] - u[i-1]);
    real rhoHalf = rho[i]*dxb[i]/dxbHalf;
    pHalf[i] = calcPressure(energyHalf, c.gamma, rhoHalf);
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
  Variable &dxb = vars.dxb;
  real* dxbNew = dxb.getPlus(1);
  Variable &dxc = vars.dxc;
  real* dxcNew = dxc.getPlus(1);

  real uHalf, uHalfPrev;
  for(int i=1; i<vars.len() - 1; ++i) {
    // Calc density at spatial half step
    real rhoSHalf = calcSHalfDensityAt(i, dxb.get(), rho.get());
    uNew[i] = u[i] - c.dt*(pHalf[i+1] - pHalf[i])/(rhoSHalf*dxc[i]);
    uHalfPrev = uHalf;
    uHalf = mean(u[i], uNew[i]);
    eNew[i] = e[i] - c.dt*pHalf[i]*(uHalf - uHalfPrev)/(rho[i]*dxb[i]);
    dxbNew[i] = dxb[i] + c.dt*(uHalf - uHalfPrev);
    dxcNew[i-1] = mean(dxbNew[i-1], dxbNew[i]);
    rhoNew[i] = rho[i]*dxb[i]/dxbNew[i];
  }
}

void runRemapStep(ModelVariables& vars, const Constants& c) {
  Variable& rho = vars.density;
  real* rhoNew = vars.density.getPlus(1);
  Variable& dxb = vars.dxb;
  real* dxbNew = vars.dxb.getPlus(1);
  Variable& dxc = vars.dxc;
  real* dxcNew = vars.dxc.getPlus(1);
  Variable& u = vars.velocity;
  real* uNew = vars.velocity.getPlus(1);
  Variable& e = vars.energy;
  real* eNew = vars.energy.getPlus(1);

  real dM, dMPrev, de, dePrev, du, duPrev;
  for(int i=2; i<vars.len()-2; ++i) {
    real uBar = mean(u[i], uNew[i]);
    real phi = std::abs(uBar)*c.dt/dxbNew[i];

    real D = FluxLimiter::calcAt(i, phi, rhoNew, uBar, dxcNew, dxbNew);
    dMPrev = dM;
    dM = (rhoNew[i] + dxbNew[i]/2.0f*D*(1-phi))*std::abs(uBar)*c.dt;
    rhoNew[i] = rho[i] + (dMPrev - dM)/dxb[i];

    real dedx = FluxLimiter::calcAt(i, phi, e.get(), uBar, dxc.get(), dxb.get());
    dePrev = de;
    de = (eNew[i] + dxb[i]/2.0f*dedx*(1 - dM/(rho[i]*dxb[i])))*dM;
    eNew[i] = (eNew[i]*dxb[i]*rho[i] + dePrev - de)/(dxb[i]*rhoNew[i]);

    // NB: dudx calc'd over dxc, hence swap dxc & dxb
    //real dudx = FluxLimiter::calcAt(i, phi, uNew, uBar, dxbNew, dxcNew);
    //duPrev = du;
    //real uBarNext = mean(u[i+1], uNew[i+1]);
    //real uBarPrev = mean(u[i-1], uNew[i-1]);
    //real delta = dxcNew[i]/2.0f - (uBar + uBarNext)/4.0f*c.dt;
    //real uc = uNew[i] + delta*dudx;
    //du = uc*(uBarNext + uBar)/2.0f*c.dt;
    ////du = (uNew[i] + (dxb[i]/2.0f - c.dt/4.0f*(uBarPrev + uBar))*dudx)*c.dt/2.0f*(uBarNext + uBar);
    ////printf("%f", du);
    //uNew[i] = u[i] + (duPrev - du)/dxc[i];

    // NB: dudx calc'd over dxc, hence swap dxc & dxb
    real rhoSHalf = calcSHalfDensityAt(i-1, dxb.get(), rho.get());
    real dudx = FluxLimiter::calcAt(i-1, phi, u.get(), uBar, dxb.get(), dxc.get());
    duPrev = du;
    //du = (uNew[i] + dxc[i]/2.0f*dudx*(1 - dM/(rho[i]*dxc[i])))*dM;
    du = (uNew[i-1] + dxc[i-1]/2.0f*dudx*(1 - mean(dM, dMPrev)/(rhoSHalf*dxc[i-1])))*mean(dM, dMPrev);
    uNew[i-1] = (uNew[i-1]*dxc[i-1]*rhoSHalf + duPrev - du)/(dxc[i-1]*calcSHalfDensityAt(i-1, dxbNew, rhoNew));
    
    //real phiHalf = std::abs()
    //real DHalf = FluxLimiter::calcAt(i, )
    //real dMHalf = (calcSHalfDensityAt(i, dxb.get(), rhoNew) + dxcNew[i]/2.0f*)
  }

  // Remap grid
  for(int i=0; i < vars.len(); ++i) {
    dxbNew[i] = dxb[i];
    dxcNew[i] = dxc[i];
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
