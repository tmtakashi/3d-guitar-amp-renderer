#include "SphUtils.hpp"
#include "boost/math/special_functions/bessel.hpp"
#include "boost/math/special_functions/spherical_harmonic.hpp"
#include <cmath>

constexpr const double rho = 1.293;      // density of air
constexpr const double kappa = 142.0e3;  // bulk modulus
const double c = std::sqrt(kappa / rho); // speed of sound

void getSphHarmMtx(Eigen::MatrixXcd &sphHarmMtx,
                   const Eigen::MatrixXd &positionGridMtx, unsigned int order) {
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

void SFT(const Eigen::Ref<const Eigen::RowVectorXcd> &freqDomainSignals,
         const Eigen::Ref<const Eigen::MatrixXd> &positionGrid,
         // TODO: find better implementation
         Eigen::Ref<Eigen::RowVectorXcd, 0, Eigen::InnerStride<>> sphCoeffs,
         unsigned int order) {
  unsigned int numSignals = freqDomainSignals.size();

  assert(sphCoeffs.size() == (order + 1) * (order + 1));

  /* get sphrical harmonics matrix */
  Eigen::MatrixXcd Y(numSignals, (order + 1) * (order + 1));
  getSphHarmMtx(Y, positionGrid, order);

  /* calculate spherical fourier transfrom using pseudo inverse */
  sphCoeffs = Y.bdcSvd(Eigen::ComputeThinU | Eigen::ComputeThinV)
                  .solve(freqDomainSignals.transpose())
                  .transpose();
}

void getSignalMtxInSphDomain(const Eigen::MatrixXcd &signals,
                             const Eigen::MatrixXd &positionGridMtx,
                             Eigen::MatrixXcd &sphSignals, unsigned order) {
  // check if numbers of frequencies of input and output matricies are the same
  assert(signals.rows() == sphSignals.rows());
  // check if numbers of signals matches with postion number
  assert(signals.cols() == positionGridMtx.rows());
  // check if the output matrix has appropriate column number
  assert(sphSignals.cols() == (order + 1) * (order + 1));

  // perform SFT for each frequency bin
  unsigned numFreqs = signals.rows();
  for (int i = 0; i < numFreqs; i++) {
    SFT(signals.row(i), positionGridMtx, sphSignals.row(i), order);
  }
}

double bn(unsigned int n, double k, double radius,
          MicArrayConfig micArrayConfig) {
  if (micArrayConfig == MicArrayConfig{ArrayType::open, MicType::omni}) {
    return boost::math::sph_bessel(n, k * radius);
  }
  /* TODO: return bns for other configs */
  return boost::math::sph_bessel(n, k * radius);
}

void getRadialFilter(Eigen::MatrixXcd &radFiltMtx, double radius,
                     unsigned int nfft, double fs, unsigned int order,
                     MicArrayConfig micArrayConfig) {
  assert(radFiltMtx.rows() == std::floor(nfft / 2) + 1);
  assert(radFiltMtx.cols() == (order + 1) * (order + 1));
  Eigen::VectorXd freqs =
      Eigen::VectorXd::LinSpaced(std::floor(nfft / 2) + 1, 0, floor(fs / 2));
  for (std::size_t i = 0; i < freqs.size(); i++) {
    double k = 2 * M_PI * freqs(i) / c;
    for (int n = 0; n < order; n++) {
      for (int m = 0; m < 2 * n + 1; m++) {
        auto b = bn(n, k, radius, micArrayConfig);
        if (b < 1e-12) {
          b = 1e-12; // avoid zero division
                     // TODO: define zero division avoidance for complex bn
        }
        radFiltMtx(i, n * n + m) =
            1.0 / (4 * M_PI * std::pow(std::complex<double>(0, 1), n) * b);
      }
    }
  }
}

std::vector<std::size_t> getReversedMnIds(unsigned order) {
  std::vector<std::size_t> reversedMnIds;
  for (unsigned n = 0; n < order + 1; n++) {
    for (unsigned m = 0; m < 2 * n + 1; m++) {
      reversedMnIds.push_back((n + 1) * (n + 1) - m - 1);
    }
  }
  return reversedMnIds;
}
