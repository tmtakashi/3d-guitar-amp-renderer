#include <string>
#include <Eigen/Dense>
#include "SOFA.h"
#include "SofaReader.hpp"

class HRIRSofaReader : public SofaReader<sofa::SimpleFreeFieldHRIR>
{
private:
    sofa::SimpleFreeFieldHRIR hrirFile;

    // HRIR L, R pair
    Eigen::MatrixXd hrirL;
    Eigen::MatrixXd hrirR;

    void extractHRIRPair();
    void extractPositionGrid() override;

public:
    explicit HRIRSofaReader(std::string_view path);
    ~HRIRSofaReader();

    Eigen::MatrixXd getHRIRL();
    Eigen::MatrixXd getHRIRR();
};
