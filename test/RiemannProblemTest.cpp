#include "catch.hpp"
#include <RiemannProblem.hpp>
#include <ModelVariables.hpp>
#include <Constants.hpp>

TEST_CASE( "RiemannProblem produces the correct analytical solution", "[riemann]" ) {
  int N = 300;
  const Constants c(0.0001f, 2.0f, N, 10, 2.0f, 3.0f);
  ModelVariables vars(c);

  setupAnalyticalSolution(vars, c, 0.0050);

  vars.save("aSoln50.dat");
}
