#include "SofaReader.hpp"
#include <vector>
#include <cmath>
#define _USE_MATH_DEFINES
#define degreesToRadians(angleDegrees) (angleDegrees * M_PI / 180.0)

SofaReader::SofaReader(std::string_view path)
    : sofaFile(std::string(path))
{
}

SofaReader::~SofaReader()
{
}

void SofaReader::degToRadMat(Eigen::MatrixXd &degSphCoords) noexcept
{
    for (std::size_t i = 0; i != degSphCoords.rows(); ++i)
    {
        degSphCoords.row(i)(0) = degreesToRadians(degSphCoords.row(i)(0));
        degSphCoords.row(i)(1) = degreesToRadians(degSphCoords.row(i)(1));
    }
}

double SofaReader::getSampleRate() const
{
    return sampleRate;
}

void SofaReader::setSampleRate(double sr)
{
    sampleRate = sr;
}

unsigned int SofaReader::getM() const
{
    return M;
}

unsigned int SofaReader::getR() const
{
    return R;
}

unsigned int SofaReader::getN() const
{
    return N;
}

void SofaReader::setM(unsigned int m)
{
    M = m;
}

void SofaReader::setR(unsigned int r)
{
    R = r;
}

void SofaReader::setN(unsigned int n)
{
    N = n;
}

std::size_t SofaReader::array3DIndex(const unsigned long i,
                                     const unsigned long j,
                                     const unsigned long k,
                                     const unsigned long dim1,
                                     const unsigned long dim2,
                                     const unsigned long dim3) noexcept
{
    return dim2 * dim3 * i + dim3 * j + k;
}

std::size_t SofaReader::array2DIndex(const unsigned long i,
                                     const unsigned long j,
                                     const unsigned long dim1,
                                     const unsigned long dim2) noexcept
{
    return dim2 * i + j;
}