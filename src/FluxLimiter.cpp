#include <FluxLimiter.hpp>
#include <Precision.hpp>
#include <cmath>
#include <algorithm>

using std::abs;
using std::min;

real FluxLimiter::calcAt(int i, real phi, real* f, real uBar, real* dxc, real* dxb) {
  real absD = (2.0f-phi)/3.0f*abs(f[i+1] - f[i])/dxc[i];
  if(uBar>0.0f) {
    absD += (1.0f+phi)/3.0f*abs(f[i] - f[i-1])/dxc[i-1];
  } else {
    absD += (1.0f+phi)/3.0f*abs(f[i+2] - f[i+1])/dxc[i+1];
  }
  real dfMin = 2*min(abs(f[i+1] - f[i]), abs(f[i] - f[i-1]));
  real s = 0.5f*(sign(f[i+1] - f[i]) + sign(f[i] - f[i-1]));

  //return s*absD;
  return s*min(absD*dxb[i], dfMin);
}

int FluxLimiter::sign(real x) {
    return (x > 0) - (x < 0);
}
