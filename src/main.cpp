#include "SphUtils.hpp"
#include <Eigen/Dense>
#include <iostream>

int main() {
  Eigen::MatrixXd grid(3, 3);
  grid << -M_PI / 4, M_PI / 3, 1.0, M_PI / 2, -M_PI / 6, 1.0, -M_PI / 3,
      M_PI / 4.0, 1.0;
  unsigned int order = 3;
  Eigen::MatrixXcd sphHarmMtx(3, static_cast<int>(std::pow(order + 1, 2)));
  getSphHarmMtx(sphHarmMtx, grid, 3);
  std::cout << sphHarmMtx << std::endl;
}
