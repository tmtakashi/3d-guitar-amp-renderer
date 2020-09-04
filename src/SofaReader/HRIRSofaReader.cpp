#include "HRIRSofaReader.hpp"
#include <vector>
#include <cmath>
#define _USE_MATH_DEFINES
#define degreesToRadians(angleDegrees) (angleDegrees * M_PI / 180.0)

HRIRSofaReader::HRIRSofaReader(std::string_view path)
    : SofaReader(path), hrirFile(std::string(path))
{
    assert(hrirFile.IsValid());
    assert(sofaFile.IsValid());
    extractSampleRate();
    extractHRIRPair();
    extractPositionGrid();
}

HRIRSofaReader::~HRIRSofaReader()
{
}

void HRIRSofaReader::extractSampleRate()
{
    double sr;
    const bool isSampleRateValid = hrirFile.GetSamplingRate(sr);
    assert(isSampleRateValid);
    setSampleRate(sr);
}

void HRIRSofaReader::extractHRIRPair()
{
    setM(static_cast<unsigned int>(hrirFile.GetNumMeasurements()));
    setR(static_cast<unsigned int>(hrirFile.GetNumReceivers()));
    setN(static_cast<unsigned int>(hrirFile.GetNumDataSamples()));
    // check if the is for human ears
    assert(getR() == 2);

    // extract HRIR
    std::vector<double> tmp;
    hrirFile.GetDataIR(tmp);

    // resize HRIR containers
    hrirL.resize(getM(), getN());
    hrirR.resize(getM(), getN());
    for (std::size_t receiverIdx = 0; receiverIdx < getR(); receiverIdx++)
    {
        for (std::size_t measurementIdx = 0; measurementIdx < getM(); measurementIdx++)
        {
            for (std::size_t numSampleIdx = 0; numSampleIdx < getN(); numSampleIdx++)
            {
                std::size_t index = array3DIndex(measurementIdx, receiverIdx, numSampleIdx, getM(), getR(), getN());
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

void HRIRSofaReader::extractPositionGrid()
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

    positionGrid.resize(dims[0], dims[1]);
    for (std::size_t i = 0; i < dims[0]; i++)
    {
        for (std::size_t j = 0; j < dims[1]; j++)
        {
            const std::size_t index = array2DIndex(i, j, dims[0], dims[1]);
            positionGrid(i, j) = pos[index];
        }
    }
    // convert degree to radian
    degToRadMat(positionGrid);
}
