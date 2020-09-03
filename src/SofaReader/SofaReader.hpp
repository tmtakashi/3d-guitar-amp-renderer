#include <string>
#include <utility>
#include <Eigen/Dense>
#include "SOFA.h"

class SofaReader
{
private:
    sofa::SimpleFreeFieldHRIR hrirFile;
    sofa::File sofaFile;
    double sampleRate;
    // number of measurements
    unsigned int M;
    // number of receivers
    unsigned int R;
    // number of data samples
    unsigned int N;

    // HRIR L, R pair
    Eigen::MatrixXd hrirL;
    Eigen::MatrixXd hrirR;
    // Grid Positions
    Eigen::MatrixXd hrirGrid;

    void extractSampleRate();
    void extractHRIRPair();
    void extractHRIRGrid();

    static void degToRadMat(Eigen::MatrixXd &degSphCoords);
    static std::size_t array3DIndex(const unsigned long i,
                                    const unsigned long j,
                                    const unsigned long k,
                                    const unsigned long dim1,
                                    const unsigned long dim2,
                                    const unsigned long dim3) noexcept
    {
        return dim2 * dim3 * i + dim3 * j + k;
    }
    static std::size_t array2DIndex(const unsigned long i,
                                    const unsigned long j,
                                    const unsigned long dim1,
                                    const unsigned long dim2) noexcept
    {
        return dim2 * i + j;
    }

public:
    explicit SofaReader(std::string_view path);
    ~SofaReader();
};
