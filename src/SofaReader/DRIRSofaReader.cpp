#include "DRIRSofaReader.hpp"

DRIRSofaReader::DRIRSofaReader(std::string_view path)
    : SofaReader<sofa::GeneralFIR>(path), firFile(std::string(path))
{
    extractIRs();
    extractPositionGrid();
}

void DRIRSofaReader::extractIRs()
{
    extractDimentions();
    // ensure there is only one measurement set
    assert(getM() == 1);

    std::vector<double> tmp;
    irFile.GetDataIR(tmp);
    // resize DRIR container
    drirs.resize(getR(), getN());
    for (std::size_t receiverIdx = 0; receiverIdx < getR(); receiverIdx++)
    {
        for (std::size_t measurementIdx = 0; measurementIdx < getM(); measurementIdx++)
        {
            for (std::size_t numSampleIdx = 0; numSampleIdx < getN(); numSampleIdx++)
            {
                std::size_t index = array3DIndex(measurementIdx, receiverIdx, numSampleIdx, getM(), getR(), getN());
                std::cout << tmp[index] << std::endl;
                drirs(receiverIdx, numSampleIdx) = tmp[index];
            }
        }
    }
}

void DRIRSofaReader::extractPositionGrid()
{
}

Eigen::MatrixXd DRIRSofaReader::getDRIRs()
{
    return drirs;
}