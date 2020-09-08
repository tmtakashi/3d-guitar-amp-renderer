#include <Eigen/Dense>
#include <complex>

/* mic array configuration */
enum class ArrayType { open, rigid, dual };

enum class MicType {
  omni,
  cardioid,
};

struct MicArrayConfig {
  ArrayType arrayType;
  MicType micType;

  bool operator==(const MicArrayConfig &rhs) {
    return arrayType == rhs.arrayType && micType == rhs.micType;
  }
};

void getSphHarmMtx(Eigen::MatrixXcd &sphHarmMtx,
                   Eigen::MatrixXd &positionGridMtx, unsigned int order);

void SFT(Eigen::VectorXcd &freqDomainSignals, Eigen::MatrixXd &positionGrid,
         Eigen::VectorXcd &sphCoeffs, unsigned int order);

/**
 * calculates radial filter in frequency domain (currently only open sphere
 *only)
 * @return radial filter vector (nFreqs, (order + 1)^2)
 **/
void getRadialFilter(Eigen::MatrixXcd &radFiltMtx, double radius,
                     unsigned int nfft, double fs, unsigned int order,
                     MicArrayConfig config);

/**
 * radial component of spherical harmonic expantion
 **/
std::complex<double> bn(int n, double k, double r,
                        MicArrayConfig micArrayConfig, double alpha,
                        double radius);
