#include <string>
#include <utility>
#include <Eigen/Dense>
#include "SOFA.h"
#include "SofaReader.hpp"

class HRIRSofaReader : public SofaReader
{
private:
    sofa::SimpleFreeFieldHRIR hrirFile;

    // HRIR L, R pair
    Eigen::MatrixXd hrirL;
    Eigen::MatrixXd hrirR;

    void extractSampleRate() override;
    void extractHRIRPair();
    void extractPositionGrid() override;

public:
    explicit HRIRSofaReader(std::string_view path);
    ~HRIRSofaReader();
};
