#include <catch.hpp>
#include <FluxLimiter.hpp>
#include <Precision.hpp>

TEST_CASE("Flux limiter working correctly") {
  CHECK(FluxLimiter::sign(real(1.0f)) == 1);
  CHECK(FluxLimiter::sign(real(-1.0f)) == -1);

  real data[] = {0.0f, 1.0f, 2.0f, 1.0f, 0.0f};
  CHECK(FluxLimiter::calcSignAt(1, data) == 1);
  CHECK(FluxLimiter::calcSignAt(2, data) == 0);
  CHECK(FluxLimiter::calcSignAt(3, data) == -1);
}
