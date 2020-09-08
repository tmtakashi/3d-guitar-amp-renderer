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

void SFT(Eigen::VectorXcd &freqDomainSignals, Eigen::MatrixXd &positionGrid,
         Eigen::VectorXcd &sphCoeffs, unsigned int order) {
  unsigned int numSignals = freqDomainSignals.rows();

  assert(sphCoeffs.size() == (order + 1) * (order + 1));

  /* get sphrical harmonics matrix */
  Eigen::MatrixXcd Y(80, (order + 1) * (order + 1));
  getSphHarmMtx(Y, positionGrid, order);

  /* calculate spherical fourier transfrom using pseudo inverse */
  sphCoeffs = Y.bdcSvd(Eigen::ComputeThinU | Eigen::ComputeThinV)
                  .solve(freqDomainSignals);
}
