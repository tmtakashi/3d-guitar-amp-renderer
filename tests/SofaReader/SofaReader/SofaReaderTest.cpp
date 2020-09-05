#include <Eigen/Dense>
#include <cmath>
#include "gtest/gtest.h"
#include "SofaReader.hpp"
#include "SOFA.h"

TEST(array3DIndexTest, calculatesCorrectIndex)
{
    EXPECT_EQ(SofaReader<sofa::SimpleFreeFieldHRIR>::array3DIndex(0, 0, 0, 2702, 2, 128), 0);
    EXPECT_EQ(SofaReader<sofa::SimpleFreeFieldHRIR>::array3DIndex(200, 0, 60, 2702, 2, 128), 51260);
}

TEST(array2DIndexTest, calculatesCorrectIndex)
{
    EXPECT_EQ(SofaReader<sofa::SimpleFreeFieldHRIR>::array2DIndex(0, 0, 2702, 128), 0);
    EXPECT_EQ(SofaReader<sofa::SimpleFreeFieldHRIR>::array2DIndex(200, 60, 2702, 128), 25660);
}

TEST(degToRadMatTest, convertsPostionMatrixFromDegToRad)
{
    Eigen::MatrixXd input(3, 3);
    input << 45, 90, 3,
        90, 180, 3,
        60, 45, 3;
    Eigen::MatrixXd want(3, 3);
    want << M_PI / 4, M_PI / 2, 3,
        M_PI / 2, M_PI, 3,
        M_PI / 3, M_PI / 4, 3;
    SofaReader<sofa::SimpleFreeFieldHRIR>::degToRadMat(input);
    for (std::size_t i = 0; i != input.rows(); i++)
        for (std::size_t j = 0; j != input.cols(); j++)
            EXPECT_DOUBLE_EQ(input(i, j), want(i, j));
}