#pragma once
#include <Precision.hpp>

class FluxLimiter {
  public:
  static real calcAt(int i, real phi, real* f, real uBar, real* dxc, real* dxb);
  static int sign(real x);
  static int calcSignAt(int i, real* f);
};
