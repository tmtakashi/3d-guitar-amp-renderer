#include "SofaReader.hpp"
#include <vector>
#include <cmath>
#define _USE_MATH_DEFINES
#define degreesToRadians(angleDegrees) (angleDegrees * M_PI / 180.0)

SofaReader::SofaReader(std::string_view path)
    : hrirFile(std::string(path)), sofaFile(std::string(path))
{
    assert(hrirFile.IsValid());
    assert(sofaFile.IsValid());
    extractSampleRate();
    extractHRIRPair();
    extractHRIRGrid();
}

SofaReader::~SofaReader()
{
}

void SofaReader::extractSampleRate()
{
    const bool validSampleRate = hrirFile.GetSamplingRate(sampleRate);
    assert(validSampleRate);
}

void SofaReader::extractHRIRPair()
{
    M = static_cast<unsigned int>(hrirFile.GetNumMeasurements());
    R = static_cast<unsigned int>(hrirFile.GetNumReceivers());
    N = static_cast<unsigned int>(hrirFile.GetNumDataSamples());
    // check if the is for human ears
    assert(R == 2);

    // extract HRIR
    std::vector<double> tmp;
    hrirFile.GetDataIR(tmp);

    // resize HRIR containers
    hrirL.resize(M, N);
    hrirR.resize(M, N);
    for (std::size_t receiverIdx = 0; receiverIdx < R; receiverIdx++)
    {
        for (std::size_t measurementIdx = 0; measurementIdx < M; measurementIdx++)
        {
            for (std::size_t numSampleIdx = 0; numSampleIdx < N; numSampleIdx++)
            {
                std::size_t index = array3DIndex(measurementIdx, receiverIdx, numSampleIdx, M, R, N);
                switch (receiverIdx)
                {
                case 0:
                    hrirL(measurementIdx, numSampleIdx) = tmp[index];
                    break;
                case 1:
                    hrirR(measurementIdx, numSampleIdx) = tmp[index];
                    break;
                default:
                    break;
                }
            }
        }
    }
}

void SofaReader::extractHRIRGrid()
{
    sofa::Coordinates::Type coordinates;
    sofa::Units::Type units;
    const bool ok = sofaFile.GetSourcePosition(coordinates, units);
    assert(ok);
    // check if the coordinate is spherical
    assert(coordinates == sofa::Coordinates::Type::kSpherical);
    // check if the units are in degree, degree, metre
    assert(units == sofa::Units::Type::kSphericalUnits);

    std::vector<std::size_t> dims;
    sofaFile.GetVariableDimensions(dims, "SourcePosition");
    // check if the dimention is {measurements, coords}
    assert(dims.size() == 2);

    // temporary position container
    std::vector<double> pos(dims[0] * dims[1]);
    sofaFile.GetSourcePosition(&pos[0], dims[0], dims[1]);

    hrirGrid.resize(dims[0], dims[1]);
    for (std::size_t i = 0; i < dims[0]; i++)
    {
        for (std::size_t j = 0; j < dims[1]; j++)
        {
            const std::size_t index = array2DIndex(i, j, dims[0], dims[1]);
            hrirGrid(i, j) = pos[index];
        }
    }
    // convert degree to radian
    degToRadMat(hrirGrid);
}

void SofaReader::degToRadMat(Eigen::MatrixXd &degSphCoords)
{
    for (std::size_t i = 0; i != degSphCoords.rows(); ++i)
    {
        degSphCoords.row(i)(0) = degreesToRadians(degSphCoords.row(i)(0));
        degSphCoords.row(i)(1) = degreesToRadians(degSphCoords.row(i)(1));
    }
}