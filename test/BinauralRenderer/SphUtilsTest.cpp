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
