#include "SofaReader.hpp"
#include <vector>

SofaReader::SofaReader(std::string_view path) : sofaFile(std::string(path))
{
    extractSampleRate();
    extractHRIRPair();
}

SofaReader::~SofaReader()
{
}

void SofaReader::extractSampleRate()
{
    assert(sofaFile.IsValid());
    const bool validSampleRate = sofaFile.GetSamplingRate(sampleRate);
    assert(validSampleRate);
}

void SofaReader::extractHRIRPair()
{
    M = static_cast<unsigned int>(sofaFile.GetNumMeasurements());
    R = static_cast<unsigned int>(sofaFile.GetNumReceivers());
    N = static_cast<unsigned int>(sofaFile.GetNumDataSamples());
    // check if the is for human ears
    assert(R == 2);

    // extract HRIR
    std::vector<double> tmp;
    sofaFile.GetDataIR(tmp);

    // resize HRIR containers
    hrirL.resize(M, N);
    hrirR.resize(M, N);
    for (std::size_t receiverIdx = 0; receiverIdx < R; receiverIdx++)
    {
        for (std::size_t measurementIdx = 0; measurementIdx < M; receiverIdx++)
        {
            for (std::size_t numSampleIdx = 0; numSampleIdx < N; numSampleIdx++)
            {
                const std::size_t index = array3DIndex(measurementIdx, receiverIdx, numSampleIdx, M, R, N);
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

std::size_t
SofaReader::array3DIndex(const unsigned long i,
                         const unsigned long j,
                         const unsigned long k,
                         const unsigned long dim1,
                         const unsigned long dim2,
                         const unsigned long dim3) noexcept
{
    return dim2 * dim3 * i + dim3 * j + k;
}