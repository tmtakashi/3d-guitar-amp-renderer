#include "SofaReader.hpp"
#include <vector>

SofaReader::SofaReader(std::string_view path) : sofaFile(path)
{
    assert(sofaFile.IsValid());

    // set sample rate
    const bool validSampleRate = sofaFile.GetSamplingRate(sampleRate);
    assert(validSampleRate);

    M = static_cast<unsigned int>(sofaFile.GetNumMeasurements());
    R = static_cast<unsigned int>(sofaFile.GetNumReceivers());
    N = static_cast<unsigned int>(sofaFile.GetNumDataSamples());
    // check if the is for human ears
    assert(R == 2);

    // extract HRIR
    std::vector<double> tmp;
    sofaFile.GetDataIR(tmp);
}

SofaReader::~SofaReader()
{
}