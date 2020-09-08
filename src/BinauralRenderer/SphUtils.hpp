#include <Eigen/Dense>

void getSphHarmMtx(Eigen::MatrixXcd &sphHarmMtx,
                   Eigen::MatrixXd &positionGridMtx, unsigned int order);

void SFT(Eigen::VectorXcd &freqDomainSignals, Eigen::MatrixXd &positionGrid,
         Eigen::VectorXcd &sphCoeffs, unsigned int order);
