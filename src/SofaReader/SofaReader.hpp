#include <string>
#include <utility>
#include <Eigen/Dense>
#include "SOFA.h"

using namespace Eigen;

class SofaReader
{
private:
    sofa::SimpleFreeFieldHRIR sofaFile;
    double sampleRate;
    // number of measurements
    unsigned int M;
    // number of receivers
    unsigned int R;
    // number of data samples
    unsigned int N;

    // HRIR L (first), R(second) pair
    std::pair<MatrixXd, MatrixXd> hrirs;

public:
    explicit SofaReader(std::string path);
    ~SofaReader();
};
