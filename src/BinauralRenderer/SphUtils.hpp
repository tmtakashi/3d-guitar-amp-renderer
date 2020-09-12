#ifndef INCLUDE_SPHUTILS
#define INCLUDE_SPHUTILS
#include <Eigen/Core>
#include <Eigen/Dense>
#include <complex>
#include <unsupported/Eigen/FFT>
#include <vector>

enum class SphHarmType { ComplexAsymmetric, ComplexSymmetric, Real };

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

template <typename Derived>
void rowDirectionZeroPadding(Eigen::MatrixBase<Derived> &mtx,
                             std::size_t targetRowNum) {
  assert(targetRowNum >= mtx.rows());
  mtx.derived().conservativeResizeLike(
      Eigen::MatrixXd::Zero(targetRowNum, mtx.cols()));
}

/**
 * radial component of spherical harmonic expantion
 **/
std::complex<double> bn(int n, double k, double r,
                        MicArrayConfig micArrayConfig, double alpha,
                        double radius);

/**
 * returns spherical harmonic index set of n(-m) up to given order.
 **/
std::vector<std::size_t> getReversedMnIds(unsigned order);

template <typename Derived>
Derived getColumnPermutatedMatrix(Eigen::MatrixBase<Derived> &matrix,
                                  std::vector<std::size_t> permutationIndices) {
  assert(matrix.cols() == permutationIndices.size());
  Derived permutatedMatrix(matrix.rows(), matrix.cols());
  std::size_t numCols = matrix.cols();
  for (std::size_t i = 0; i < numCols; i++) {
    if (i != permutationIndices[i]) {
      permutatedMatrix.col(i) = matrix.col(permutationIndices[i]);
    } else {
      permutatedMatrix.col(i) = matrix.col(i);
    }
  }
  return permutatedMatrix;
}

Eigen::MatrixXd getSphHarmTypeCoeffMtx(unsigned order, unsigned nfft,
                                       SphHarmType);

void rfftEachCol(Eigen::MatrixXcd &freqSignals, Eigen::MatrixXd &timeSignals,
                 unsigned nfft);
void rifftEachCol(Eigen::MatrixXd &timeSignals, Eigen::MatrixXcd &freqSignals,
                  unsigned nfft);

void getRotationMatrix(Eigen::MatrixXcd &mtx, double azimuth, unsigned order,
                       unsigned nfft);
#endif /* ifndef INCLUDE_SPHUTILS */
