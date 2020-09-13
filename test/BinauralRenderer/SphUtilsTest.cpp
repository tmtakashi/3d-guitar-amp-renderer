#include "SphUtils.hpp"
#include "gtest/gtest.h"
#include "gtest/internal/gtest-port.h"
#include <Eigen/Dense>
#include <cmath>
#include <complex>
#include <fstream>

TEST(getSphHarmMtxTest, calculatesCorrectSphHarms) {
  Eigen::MatrixXd grid(3, 3);
  grid << M_PI / 4, M_PI / 3, 1.0, 3 * M_PI / 2, M_PI / 6, 1.0, M_PI / 3,
      M_PI / 4.0, 1.0;
  unsigned int order = 3;
  Eigen::MatrixXcd sphHarmMtx(3, (order + 1) * (order + 1));
  getSphHarmMtx(sphHarmMtx, grid, 3);

  std::ifstream file("../testdata/sph_mtx_data.dat");
  double real, imag;
  for (std::size_t i = 0; i < sphHarmMtx.rows(); i++) {
    for (std::size_t j = 0; j < sphHarmMtx.cols(); j++) {
      file >> real;
      file >> imag;
      double eps = 1.0e-5;
      EXPECT_NEAR(sphHarmMtx(i, j).real(), real, eps);
      EXPECT_NEAR(sphHarmMtx(i, j).imag(), imag, eps);
    }
  }
}

TEST(getReversedMnIdsTest, reverseMIdsCorrectly) {
  unsigned order = 3;
  auto reversedMnIds = getReversedMnIds(order);
  std::vector<std::size_t> want{0, 3,  2,  1,  8,  7,  6,  5,
                                4, 15, 14, 13, 12, 11, 10, 9};
  for (int i = 0; i < (order + 1) * (order + 1); i++) {
    EXPECT_EQ(reversedMnIds[i], want[i]);
  }
}

TEST(permutateMatrixColumnTest, reversesColumnCorrectly) {
  unsigned order = 1;
  Eigen::Matrix4i matrix;
  matrix << 1, 2, 3, 4, 1, 2, 3, 4, 1, 2, 3, 4, 1, 2, 3, 4;
  std::vector<std::size_t> reverseIndices{0, 3, 2, 1};
  Eigen::Matrix4i want;
  want << 1, 4, 3, 2, 1, 4, 3, 2, 1, 4, 3, 2, 1, 4, 3, 2;

  auto permutatedMatrix = getColumnPermutatedMatrix(matrix, reverseIndices);
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      EXPECT_EQ(permutatedMatrix(i, j), want(i, j));
    }
  }
}

TEST(getSphHarmTypeCoeffMtxTest, Asymmetric) {
  unsigned order = 1;
  unsigned nfft = 4;
  std::size_t freqBinNum = std::floor(nfft / 2) + 1;
  Eigen::MatrixXd want(freqBinNum, (order + 1) * (order + 1));
  want << 1, -1, 1, -1, 1, -1, 1, -1, 1, -1, 1, -1;
  auto get =
      getSphHarmTypeCoeffMtx(order, nfft, SphHarmType::ComplexAsymmetric);

  for (std::size_t i = 0; i < want.rows(); i++) {
    for (std::size_t j = 0; j < want.rows(); j++) {
      EXPECT_EQ(get(i, j), want(i, j));
    }
  }
}

TEST(getSphHarmTypeCoeffMtxTest, Symmetric) {
  unsigned order = 2;
  unsigned nfft = 8;
  auto want = Eigen::MatrixXd::Ones(std::floor(nfft / 2) + 1,
                                    (order + 1) * (order + 1));
  auto get = getSphHarmTypeCoeffMtx(order, nfft, SphHarmType::ComplexSymmetric);

  for (std::size_t i = 0; i < want.rows(); i++) {
    for (std::size_t j = 0; j < want.rows(); j++) {
      EXPECT_EQ(get(i, j), want(i, j));
    }
  }
}

TEST(getSphHarmTypeCoeffMtxTest, Real) {
  unsigned order = 2;
  unsigned nfft = 8;
  auto want = Eigen::MatrixXd::Ones(std::floor(nfft / 2) + 1,
                                    (order + 1) * (order + 1));
  auto get = getSphHarmTypeCoeffMtx(order, nfft, SphHarmType::Real);

  for (std::size_t i = 0; i < want.rows(); i++) {
    for (std::size_t j = 0; j < want.rows(); j++) {
      EXPECT_EQ(get(i, j), want(i, j));
    }
  }
}

TEST(rfftEachColTest, checkIfRifftConvertsBack) {
  unsigned nfft = 8;
  unsigned numSignals = 5;
  Eigen::MatrixXd timeSignals = Eigen::MatrixXd::Random(nfft, numSignals);
  Eigen::MatrixXd timeSignalsCopy = timeSignals;
  Eigen::MatrixXcd freqSignals =
      Eigen::MatrixXcd::Zero((nfft / 2) + 1, numSignals);
  rfftEachCol(freqSignals, timeSignals, nfft);
  Eigen::MatrixXd anotherTimeSignals = Eigen::MatrixXd::Zero(nfft, numSignals);
  rifftEachCol(anotherTimeSignals, freqSignals, nfft);
  for (std::size_t i = 0; i < timeSignals.rows(); i++) {
    for (std::size_t j = 0; j < timeSignals.cols(); j++) {
      EXPECT_FLOAT_EQ(nfft * timeSignalsCopy(i, j), anotherTimeSignals(i, j));
    }
  }
}

TEST(rowDirectionZeroPaddingTest, checkIfZeroPaddedCorrectly) {
  Eigen::MatrixXd mtx = Eigen::MatrixXd::Ones(3, 3);
  std::size_t targetRowNum = 5;
  rowDirectionZeroPadding(mtx, targetRowNum);

  Eigen::MatrixXd want(5, 3);
  want << 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0;
  for (std::size_t i = 0; i < want.rows(); i++) {
    for (std::size_t j = 0; j < want.cols(); j++) {
      EXPECT_EQ(mtx(i, j), want(i, j));
    }
  }
}

TEST(getRotationMatrixTest, generatesValidRotationMatrix) {
  typedef std::complex<double> cd;
  double azimuth = M_PI / 2;
  unsigned order = 1;
  unsigned nfft = 4;
  Eigen::MatrixXcd get((nfft / 2) + 1, (order + 1) * (order + 1));
  Eigen::MatrixXcd want((nfft / 2) + 1, (order + 1) * (order + 1));
  want << cd(1.0, 0.0), cd(0.0, 1.0), cd(1.0, 0.0), cd(0.0, -1.0), cd(1.0, 0.0),
      cd(0.0, 1.0), cd(1.0, 0.0), cd(0.0, -1.0), cd(1.0, 0.0), cd(0.0, 1.0),
      cd(1.0, 0.0), cd(0.0, -1.0);
  getRotationMatrix(get, azimuth, order, nfft);
  for (int i = 0; i < want.rows(); i++) {
    for (int j = 0; j < want.cols(); j++) {
      ASSERT_NEAR(get(i, j).real(), want(i, j).real(), 1e-10);
      ASSERT_NEAR(get(i, j).imag(), want(i, j).imag(), 1e-10);
    }
  }
}

TEST(getRadialFilterTest, canGetRadialFilter) {
  unsigned radialFiltNfft = 32;
  unsigned sphOrder = 7;
  double fs = 48000.0;
  double radius = 0.2;
  MicArrayConfig config{ArrayType::open, MicType::omni};
  Eigen::MatrixXcd radFiltMtx = Eigen::MatrixXcd::Zero(
      radialFiltNfft / 2 + 1, (sphOrder + 1) * (sphOrder + 1));
  getRadialFilter(radFiltMtx, radius, radialFiltNfft, fs, sphOrder, config);
}

TEST(rfftEachIRMtxTest, canExecuteRfft) {
  Eigen::MatrixXd drirs = Eigen::MatrixXd::Random(5000, 80);
  Eigen::MatrixXd hrirsL = Eigen::MatrixXd::Random(128, 2000);
  Eigen::MatrixXd hrirsR = Eigen::MatrixXd::Random(128, 2000);
  unsigned nfft = drirs.rows();
  Eigen::MatrixXcd drirsFreqDomain =
      Eigen::MatrixXcd::Zero((nfft / 2) + 1, drirs.cols());
  Eigen::MatrixXcd hrirsLFreqDomain =
      Eigen::MatrixXcd::Zero((nfft / 2) + 1, hrirsL.cols());
  Eigen::MatrixXcd hrirsRFreqDomain =
      Eigen::MatrixXcd::Zero((nfft / 2) + 1, hrirsR.cols());
  rfftEachIRMtx(drirsFreqDomain, drirs, hrirsLFreqDomain, hrirsL,
                hrirsRFreqDomain, hrirsR, nfft);
}
