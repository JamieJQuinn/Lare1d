#pragma once
#include <Precision.hpp>

class FluxLimiter {
  public:
  real calcAt(int i, real* phi, real* f, real* u, real* dxc, real* dxb);

  private:
  int sign(real x);
  int calcSignAt(int i, real* f);
};
