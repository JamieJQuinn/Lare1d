#include <FluxLimiter.hpp>
#include <Precision.hpp>
#include <cmath>
#include <algorithm>

using std::abs;
using std::min;

real FluxLimiter::calcAt(int i, real phi, real* f, real uBar, real* dxc, real* dxb) {
  real absD = (2-phi)/3*abs(f[i+1] - f[i])/dxc[i];
  if(uBar>0) {
    absD += (1+phi)/3*abs(f[i] - f[i-1])/dxc[i-1];
  } else {
    absD += (1+phi)/3*abs(f[i+2] - f[i+1])/dxc[i-1];
  }

  return calcSignAt(i, f)*min(absD*dxb[i], min(2*abs(f[i+1] - f[i]), 2*abs(f[i] - f[i-1])));
}

int FluxLimiter::calcSignAt(int i, real* f) {
  if(sign(f[i+1] - f[i]) == sign(f[i] - f[i-1])) {
    return sign(f[i+1] - f[i]);
  } else {
    return 0;
  }
}

int FluxLimiter::sign(real x) {
    return (x > 0) - (x < 0);
}
