#include <string>
#include <Eigen/Dense>
#include "SOFA.h"
#include "SofaReader.hpp"

class DRIRSofaReader : public SofaReader<sofa::GeneralFIR>
{
private:
    sofa::GeneralFIR firFile;
    void extractIRs() override;
    void extractPositionGrid() override;

public:
    explicit DRIRSofaReader(std::string_view path);
    ~DRIRSofaReader();
};