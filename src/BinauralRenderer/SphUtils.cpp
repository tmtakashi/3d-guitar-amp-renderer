#include "SphUtils.hpp"
#include "boost/math/special_functions/bessel.hpp"
#include "boost/math/special_functions/spherical_harmonic.hpp"
#include "fftw3.h"
#include <cmath>
#include <complex>

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

void SFT(Eigen::MatrixXcd &freqDomainSignals, Eigen::MatrixXd &positionGrid,
         Eigen::MatrixXcd &sphDomainSignals, unsigned int order) {
  unsigned int numSignals = freqDomainSignals.cols();

  assert(sphDomainSignals.rows() == freqDomainSignals.rows());
  assert(sphDomainSignals.cols() == (order + 1) * (order + 1));

  /* get sphrical harmonics matrix */
  Eigen::MatrixXcd Y(numSignals, (order + 1) * (order + 1));
  getSphHarmMtx(Y, positionGrid, order);

  /* calculate spherical fourier transfrom using pseudo inverse */
  sphDomainSignals = Y.bdcSvd(Eigen::ComputeThinU | Eigen::ComputeThinV)
                         .solve(freqDomainSignals.transpose())
                         .transpose();
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
  assert(radFiltMtx.rows() == (nfft / 2) + 1);
  assert(radFiltMtx.cols() == (order + 1) * (order + 1));
  Eigen::VectorXd freqs =
      Eigen::VectorXd::LinSpaced((nfft / 2) + 1, 0, floor(fs / 2));
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

Eigen::MatrixXd getSphHarmTypeCoeffMtx(unsigned order, unsigned nfft,
                                       SphHarmType sphHarmType) {
  std::size_t freqBinNum = std::floor(nfft / 2) + 1;
  Eigen::MatrixXd mtx(freqBinNum, (order + 1) * (order + 1));
  if (sphHarmType == SphHarmType::ComplexAsymmetric) {
    for (int n = 0; n < order + 1; n++) {
      for (int m = -n; m <= n; m++) {
        if (m % 2 == 0) {
          mtx.col(n * n + n + m) = Eigen::VectorXd::Ones(freqBinNum);
        } else {
          mtx.col(n * n + n + m) = -Eigen::VectorXd::Ones(freqBinNum);
        }
      }
    }
  } else {
    mtx = Eigen::MatrixXd::Ones(mtx.rows(), mtx.cols());
  }
  return mtx;
}

void rfftEachCol(Eigen::MatrixXcd &freqSignals, Eigen::MatrixXd &timeSignals,
                 unsigned nfft) {
  int numFreqBins = (nfft / 2) + 1;
  int numSignals = timeSignals.cols();
  int numSamples = timeSignals.rows();

  assert(freqSignals.rows() == numFreqBins);
  assert(nfft >= timeSignals.rows());
  assert(freqSignals.cols() == numSignals);

  double *in = (double *)fftw_malloc(sizeof(double) * nfft * numSignals);
  fftw_complex *out = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) *
                                                  numFreqBins * numSignals);

  int rank = 1;
  int n[] = {(int)nfft};
  int howmany = numSignals;
  int idist = 1;
  int odist = 1;
  int istride = numSignals;
  int ostride = numSignals;
  int *inembed = n;
  int *onembed = n;
  fftw_plan p =
      fftw_plan_many_dft_r2c(rank, n, howmany, in, inembed, istride, idist, out,
                             onembed, ostride, odist, FFTW_ESTIMATE);

  // apply zero-padding to input
  rowDirectionZeroPadding(timeSignals, nfft);

  // change col-major(default) matrix to row-major
  Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>
      timeSignalsRowMaj(timeSignals);
  // assign to raw input array
  Eigen::Map<Eigen::MatrixXd>(in, nfft, numSignals) = timeSignalsRowMaj;

  fftw_execute(p);
  for (std::size_t i = 0; i < numFreqBins; i++) {
    for (std::size_t j = 0; j < numSignals; j++) {
      int idx = i * ostride + j * odist;
      freqSignals(i, j) = std::complex<double>(out[idx][0], out[idx][1]);
    }
  }

  fftw_destroy_plan(p);
  fftw_free(in);
  fftw_free(out);
}

void rifftEachCol(Eigen::MatrixXd &timeSignals, Eigen::MatrixXcd &freqSignals,
                  unsigned nfft) {
  int numFreqBins = (nfft / 2) + 1;
  int numSignals = timeSignals.cols();
  int numSamples = timeSignals.rows();

  int rank = 1;
  int n[] = {(int)nfft};
  int howmany = numSignals;
  int idist = 1;
  int odist = 1;
  int istride = numSignals;
  int ostride = numSignals;
  int *inembed = n;
  int *onembed = n;

  assert(freqSignals.rows() == numFreqBins);
  assert(nfft >= timeSignals.rows());
  assert(freqSignals.cols() == numSignals);

  fftw_complex *in = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) *
                                                 numFreqBins * numSignals);
  double *out = (double *)fftw_malloc(sizeof(double) * nfft * numSignals);

  // assign to raw input array
  for (std::size_t i = 0; i < numFreqBins; i++) {
    for (std::size_t j = 0; j < numSignals; j++) {
      int idx = i * ostride + j * odist;
      in[idx][0] = freqSignals(i, j).real();
      in[idx][1] = freqSignals(i, j).imag();
    }
  }

  fftw_plan p =
      fftw_plan_many_dft_c2r(rank, n, howmany, in, inembed, istride, idist, out,
                             onembed, ostride, odist, FFTW_ESTIMATE);
  fftw_execute(p);
  for (std::size_t i = 0; i < nfft; i++) {
    for (std::size_t j = 0; j < numSignals; j++) {
      int idx = i * ostride + j * odist;
      timeSignals(i, j) = out[idx];
    }
  }

  fftw_destroy_plan(p);
  fftw_free(in);
  fftw_free(out);
}

void rfftEachIRMtx(Eigen::MatrixXcd &drtfs, Eigen::MatrixXd &drirs,
                   Eigen::MatrixXcd &hrtfsL, Eigen::MatrixXd &hrirsL,
                   Eigen::MatrixXcd hrtfsR, Eigen::MatrixXd &hrirsR,
                   unsigned nfft) {
  rfftEachCol(drtfs, drirs, nfft);
  rfftEachCol(hrtfsL, hrirsL, nfft);
  rfftEachCol(hrtfsR, hrirsR, nfft);
}

void getRotationMatrix(Eigen::MatrixXcd &mtx, double azimuth, unsigned order,
                       unsigned nfft) {
  assert(mtx.rows() == (nfft / 2) + 1);
  assert(mtx.cols() == (order + 1) * (order + 1));
  for (int n = 0; n < order + 1; n++) {
    for (int m = -n; m <= n; m++) {
      Eigen::VectorXcd col =
          Eigen::VectorXcd::Constant(mtx.rows(), std::polar(1.0, -azimuth * m));
      mtx.col(n * n + n + m) = col;
    }
  }
}
