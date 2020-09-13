#include "BinauralIRRenderer.hpp"
#include "gtest/gtest.h"

class BinauralIRRendererTest : public ::testing::Test {
  void SetUp() override {
    Eigen::MatrixXd drirs = Eigen::MatrixXd::Random(32, 80);
    Eigen::MatrixXd hrirsL = Eigen::MatrixXd::Random(32, 100);
    Eigen::MatrixXd hrirsR = Eigen::MatrixXd::Random(32, 100);

    Eigen::MatrixXd drirPositionGrid = Eigen::MatrixXd::Ones(80, 3);
    Eigen::MatrixXd hrirPositionGrid = Eigen::MatrixXd::Ones(100, 3);
    double radius = drirPositionGrid(0, 2);
    unsigned sphOrder = 7;
    unsigned radialFiltNfft = 32;
    double fs = 48000.0;
    MicArrayConfig micArrayConfig{ArrayType::open, MicType::omni};
    renderer = new BinauralIRRenderer(drirs, drirPositionGrid, hrirsL, hrirsR,
                                      hrirPositionGrid, fs, radius, sphOrder,
                                      radialFiltNfft, micArrayConfig);
  }

protected:
  BinauralIRRenderer *renderer;
};

TEST_F(BinauralIRRendererTest, canExecuteCalculation) {}
