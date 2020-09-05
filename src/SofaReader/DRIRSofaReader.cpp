#include "DRIRSofaReader.hpp"

DRIRSofaReader::DRIRSofaReader(std::string_view path)
    : SofaReader<sofa::GeneralFIR>(path), firFile(std::string(path))
{
    extractIRs();
    extractPositionGrid();
}
