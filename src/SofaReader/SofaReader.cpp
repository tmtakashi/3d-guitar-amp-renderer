#include "SofaReader.hpp"
#include <vector>
#include <cmath>
#define _USE_MATH_DEFINES
#define degreesToRadians(angleDegrees) (angleDegrees * M_PI / 180.0)

template <typename T>
SofaReader<T>::SofaReader(std::string_view path)
    : sofaFile(std::string(path))
{
    assert(sofaFile.IsValid());
    extractSampleRate();
    extractPositionGrid();
}

template <typename T>
void SofaReader<T>::extractSampleRate()
{
    double sr;
    const bool isSampleRateValid = hrirFile.GetSamplingRate(sr);
    assert(isSampleRateValid);
    setSampleRate(sr);
}

template <typename T>
Eigen::MatrixXd SofaReader<T>::getPositionGrid()
{
    return positionGrid;
}

template <typename T>
void SofaReader<T>::degToRadMat(Eigen::MatrixXd &degSphCoords) noexcept
{
    for (std::size_t i = 0; i != degSphCoords.rows(); ++i)
    {
        degSphCoords.row(i)(0) = degreesToRadians(degSphCoords.row(i)(0));
        degSphCoords.row(i)(1) = degreesToRadians(degSphCoords.row(i)(1));
    }
}

template <typename T>
double SofaReader<T>::getSampleRate() const
{
    return sampleRate;
}

template <typename T>
void SofaReader<T>::setSampleRate(double sr)
{
    sampleRate = sr;
}

template <typename T>
unsigned int SofaReader<T>::getM() const
{
    return M;
}

template <typename T>
unsigned int SofaReader<T>::getR() const
{
    return R;
}

template <typename T>
unsigned int SofaReader<T>::getN() const
{
    return N;
}

template <typename T>
void SofaReader<T>::setM(unsigned int m)
{
    M = m;
}

template <typename T>
void SofaReader<T>::setR(unsigned int r)
{
    R = r;
}

template <typename T>
void SofaReader<T>::setN(unsigned int n)
{
    N = n;
}

template <typename T>
std::size_t SofaReader<T>::array3DIndex(const unsigned long i,
                                        const unsigned long j,
                                        const unsigned long k,
                                        const unsigned long dim1,
                                        const unsigned long dim2,
                                        const unsigned long dim3) noexcept
{
    return dim2 * dim3 * i + dim3 * j + k;
}

template <typename T>
std::size_t SofaReader<T>::array2DIndex(const unsigned long i,
                                        const unsigned long j,
                                        const unsigned long dim1,
                                        const unsigned long dim2) noexcept
{
    return dim2 * i + j;
}
