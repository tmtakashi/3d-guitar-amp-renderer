#include "BinauralIRRenderer.hpp"
#include <unsupported/Eigen/FFT>

BinauralIRRenderer::BinauralIRRenderer(
    Eigen::MatrixXd drirs, Eigen::MatrixXd drirPositionGrid,
    Eigen::MatrixXd hrirsL, Eigen::MatrixXd hrirsR,
    Eigen::MatrixXd hrirPositionGrid, double fs, double radius,
    unsigned sphOrder, unsigned radialFiltNfft, MicArrayConfig micArrayConfig,
    unsigned numIRSets) {
  calculateNewIRs(drirs, drirPositionGrid, hrirsL, hrirsR, hrirPositionGrid, fs,
                  radius, sphOrder, radialFiltNfft, micArrayConfig, numIRSets);
}

void BinauralIRRenderer::calculateNewIRs(
    Eigen::MatrixXd drirs, Eigen::MatrixXd drirPositionGrid,
    Eigen::MatrixXd hrirsL, Eigen::MatrixXd hrirsR,
    Eigen::MatrixXd hrirPositionGrid, double fs, double radius,
    unsigned int sphOrder, unsigned int radialFiltNfft,
    MicArrayConfig micArrayConfig, unsigned numIRSets) {

  // DRIR measurement number should be larger than (sphOrder + 1)^2
  assert(drirs.cols() >= (sphOrder + 1) * (sphOrder + 1));
  unsigned nfft = drirs.rows() + radialFiltNfft;

  // convert signals to freq domain
  Eigen::MatrixXcd drirsFreqDomain =
      Eigen::MatrixXcd::Zero(nfft / 2 + 1, drirs.rows());
  Eigen::MatrixXcd hrirsLFreqDomain =
      Eigen::MatrixXcd::Zero(nfft / 2 + 1, hrirsL.rows());
  Eigen::MatrixXcd hrirsRFreqDomain =
      Eigen::MatrixXcd::Zero(nfft / 2 + 1, hrirsR.rows());

  rfftEachCol(drirsFreqDomain, drirs, nfft);
  rfftEachCol(hrirsLFreqDomain, hrirsL, nfft);
  rfftEachCol(hrirsRFreqDomain, hrirsR, nfft);

  // convert signals to sphercial harmonic domain
  Eigen::MatrixXcd drirsSphDomain =
      Eigen::MatrixXcd::Zero(nfft / 2 + 1, (sphOrder + 1) * (sphOrder + 1));
  Eigen::MatrixXcd hrirsLSphDomain =
      Eigen::MatrixXcd::Zero(nfft / 2 + 1, (sphOrder + 1) * (sphOrder + 1));
  Eigen::MatrixXcd hrirsRSphDomain =
      Eigen::MatrixXcd::Zero(nfft / 2 + 1, (sphOrder + 1) * (sphOrder + 1));

  getSignalMtxInSphDomain(drirsFreqDomain, drirPositionGrid, drirsSphDomain,
                          sphOrder);
  getSignalMtxInSphDomain(hrirsLFreqDomain, hrirPositionGrid, hrirsLSphDomain,
                          sphOrder);
  getSignalMtxInSphDomain(hrirsRFreqDomain, hrirPositionGrid, hrirsRSphDomain,
                          sphOrder);
  // permutate DRIR spherical hamonic signal
  auto permutationIndicies = getReversedMnIds(sphOrder);
  Eigen::MatrixXcd drirsSphDomainRevM =
      getColumnPermutatedMatrix(drirsSphDomain, permutationIndicies);

  // get radial filter matrix
  Eigen::MatrixXcd radFiltMtx =
      Eigen::MatrixXcd::Zero(nfft / 2 + 1, (sphOrder + 1) * (sphOrder + 1));
  getRadialFilter(radFiltMtx, radius, radialFiltNfft, fs, sphOrder,
                  micArrayConfig);
  rowDirectionZeroPadding(radFiltMtx, (nfft / 2) + 1);

  // get coefficient matrix for spherical harmonics conventions
  // TODO: make compatible for different spherical harmonics conventions
  Eigen::MatrixXd aM =
      getSphHarmTypeCoeffMtx(sphOrder, nfft, SphHarmType::ComplexAsymmetric);

  // setup fft instance
  Eigen::FFT<double> fft;
  fft.SetFlag(
      Eigen::FFT<double, Eigen::default_fft_impl<double>>::Flag::HalfSpectrum);

  // allocate memory for IRs
  leftIRs.resize(numIRSets);
  rightIRs.resize(numIRSets);
  for (double idx = 0; idx < numIRSets; idx++) {

    double azimuth = 2 * M_PI * idx / numIRSets;
    // get rotation matrix
    Eigen::MatrixXcd rotationMatrix((nfft / 2) + 1,
                                    (sphOrder + 1) * (sphOrder + 1));
    getRotationMatrix(rotationMatrix, azimuth, sphOrder, nfft);

    // element wise multiplication and summing up
    Eigen::VectorXcd sLeft = (radFiltMtx * aM * drirsSphDomainRevM *
                              hrirsLSphDomain * rotationMatrix)
                                 .rowwise()
                                 .sum();
    Eigen::VectorXcd sRight = (radFiltMtx * aM * drirsSphDomainRevM *
                               hrirsRSphDomain * rotationMatrix)
                                  .rowwise()
                                  .sum();

    // rifft for each ear
    Eigen::VectorXd leftOut;
    Eigen::VectorXd rightOut;
    fft.inv(leftOut, sLeft);
    fft.inv(rightOut, sRight);
    Eigen::Map<Eigen::VectorXd>(&leftIRs[idx][0], leftOut.size()) =
        leftOut / (leftOut.cwiseAbs().maxCoeff());
    Eigen::Map<Eigen::VectorXd>(&rightIRs[idx][0], rightOut.size()) =
        rightOut / (rightOut.cwiseAbs().maxCoeff());
  }
}
