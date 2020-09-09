#include "SphUtils.hpp"
#include "gtest/gtest.h"
#include "gtest/internal/gtest-port.h"
#include <Eigen/Dense>
#include <cmath>
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
  std::vector<unsigned> want{0, 3,  2,  1,  8,  7,  6,  5,
                             4, 15, 14, 13, 12, 11, 10, 9};
  for (int i = 0; i < (order + 1) * (order + 1); i++) {
    EXPECT_EQ(reversedMnIds[i], want[i]);
  }
}
