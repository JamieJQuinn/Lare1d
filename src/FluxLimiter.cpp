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
    absD += (1.0f+phi)/3.0f*abs(f[i+2] - f[i+1])/dxc[i-1];
  }
  real mindf = 2*min(abs(f[i+1] - f[i]), abs(f[i] - f[i-1]));
  real s = real(calcSignAt(i, f));

  return s*min(200*absD*dxb[i], mindf);
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
