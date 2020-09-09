#include <Eigen/Core>
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
                   const Eigen::MatrixXd &positionGridMtx, unsigned int order);

void SFT(const Eigen::Ref<const Eigen::RowVectorXcd> &freqDomainSignals,
         const Eigen::Ref<const Eigen::MatrixXd> &positionGrid,
         Eigen::Ref<Eigen::RowVectorXcd, 0, Eigen::InnerStride<>> sphCoeffs,
         unsigned int order);
/**
 * Calculates signal matrix in frequency domain (numFreqs, numSignals) into
 * spherical domain (nFreqs, (order + 1)^2).
 *
 **/
void getSignalMtxInSphDomain(const Eigen::MatrixXcd &signals,
                             const Eigen::MatrixXd &positionGridMtx,
                             Eigen::MatrixXcd &sphSignals, unsigned order);

/**
 * calculates radial filter in frequency domain (currently only open sphere
 * only) shape= (nFreqs, (order + 1)^2)
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

/**
 * returns spherical harmonic index set of n(-m) up to given order.
 **/
std::vector<unsigned> getReversedMnIds(unsigned order);
