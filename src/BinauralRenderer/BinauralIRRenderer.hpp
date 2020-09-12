#include "SphUtils.hpp"
#include <Eigen/Dense>
#include <vector>

class BinauralIRRenderer {
private:
  std::vector<std::vector<double>> leftIRs;
  std::vector<std::vector<double>> rightIRs;

public:
  explicit BinauralIRRenderer(Eigen::MatrixXd drirs,
                              Eigen::MatrixXd drirPositionGrid,
                              Eigen::MatrixXd hrirsL, Eigen::MatrixXd hrirsR,
                              Eigen::MatrixXd hrirPositionGrid, double fs,
                              double radius, unsigned sphOrder,
                              unsigned radialFiltNfft,
                              MicArrayConfig micArrayConfig,
                              unsigned numIRSets = 360);
  void calculateNewIRs(Eigen::MatrixXd drirs, Eigen::MatrixXd drirPositionGrid,
                       Eigen::MatrixXd hrirsL, Eigen::MatrixXd hrirsR,
                       Eigen::MatrixXd hrirPositionGrid, double fs,
                       double radius, unsigned sphOrder,
                       unsigned radialFiltNfft, MicArrayConfig micArrayConfig,
                       unsigned numIRSets = 360);

  std::vector<double> getLeftIR(std::size_t idx) const;
  std::vector<double> getRightIR(std::size_t idx) const;
};
