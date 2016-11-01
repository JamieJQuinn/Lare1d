#pragma once
#include "Precision.hpp"

class FluxLimiter {
  public:
  real calcAt(int i, real* phi, real* f, real* u, real* dxc, real* dxb);

  private:
  int sign(real x) {
    return (x > 0) - (x < 0);
  }
  int calcSignAt(int i, real* f);
};
