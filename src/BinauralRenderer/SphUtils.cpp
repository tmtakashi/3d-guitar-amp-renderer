#include "SphUtils.hpp"
#include "boost/math/special_functions/spherical_harmonic.hpp"
#include <cmath>

void getSphHarmMtx(Eigen::MatrixXcd &sphHarmMtx,
                   Eigen::MatrixXd &positionGridMtx, unsigned int order) {
  unsigned int numPositions = positionGridMtx.rows();
  for (std::size_t pos = 0; pos < numPositions; pos++) {
    for (int n = 0; n <= order; n++) {
      for (int m = -n; m <= n; m++) {
        sphHarmMtx(pos, n * n + n + m) = boost::math::spherical_harmonic(
            n, m, positionGridMtx.row(pos)(1), positionGridMtx.row(pos)(0));
      }
    }
  }
}
