#include <string>
#include <utility>
#include <Eigen/Dense>
#include <vector>
#include <cmath>
#include "SOFA.h"
#define _USE_MATH_DEFINES
#define degreesToRadians(angleDegrees) (angleDegrees * M_PI / 180.0)

template <typename T>
class SofaReader
{
private:
    double sampleRate;
    // number of measurements
    unsigned int M;
    // number of receivers
    unsigned int R;
    // number of data samples
    unsigned int N;

    virtual void extractIRs() = 0;
    virtual void extractPositionGrid() = 0;

protected:
    SofaReader(std::string_view path)
        : sofaFile(std::string(path)), irFile(std::string(path))
    {
        assert(sofaFile.IsValid());
        assert(irFile.IsValid());
        extractSampleRate();
    }
    sofa::File sofaFile;
    T irFile;
    Eigen::MatrixXd positionGrid;

    void extractSampleRate()
    {
        double sr;
        const bool isSampleRateValid = irFile.GetSamplingRate(sr);
        assert(isSampleRateValid);
        setSampleRate(sr);
    }

    Eigen::MatrixXd getPositionGrid()
    {
        return positionGrid;
    }

    double getSampleRate() const
    {
        return sampleRate;
    }

    void setSampleRate(double sr)
    {
        sampleRate = sr;
    }

    unsigned int getM() const
    {
        return M;
    }

    unsigned int getR() const
    {
        return R;
    }

    unsigned int getN() const
    {
        return N;
    }

    void setM(unsigned int m)
    {
        M = m;
    }

    void setR(unsigned int r)
    {
        R = r;
    }

    void setN(unsigned int n)
    {
        N = n;
    }

    static std::size_t array3DIndex(const unsigned long i,
                                    const unsigned long j,
                                    const unsigned long k,
                                    const unsigned long dim1,
                                    const unsigned long dim2,
                                    const unsigned long dim3) noexcept
    {
        return dim2 * dim3 * i + dim3 * j + k;
    }

    static std::size_t array2DIndex(const unsigned long i,
                                    const unsigned long j,
                                    const unsigned long dim1,
                                    const unsigned long dim2) noexcept
    {
        return dim2 * i + j;
    }

    static void degToRadMat(Eigen::MatrixXd &degSphCoords) noexcept
    {
        for (std::size_t i = 0; i != degSphCoords.rows(); ++i)
        {
            degSphCoords.row(i)(0) = degreesToRadians(degSphCoords.row(i)(0));
            degSphCoords.row(i)(1) = degreesToRadians(degSphCoords.row(i)(1));
        }
    }
};
