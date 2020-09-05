#include <string>
#include <utility>
#include <Eigen/Dense>
#include "SOFA.h"

class SofaReader
{
private:
    double sampleRate;
    // number of measurements
    unsigned int M;
    // number of receivers
    unsigned int R;
    // number of data samples
    unsigned int N;

    virtual void extractSampleRate() = 0;
    virtual void extractPositionGrid() = 0;

protected:
    sofa::File sofaFile;
    Eigen::MatrixXd positionGrid;

public:
    explicit SofaReader(std::string_view path);
    ~SofaReader();

    double getSampleRate() const;
    void setSampleRate(double sr);
    unsigned int getM() const;
    unsigned int getR() const;
    unsigned int getN() const;
    void setM(unsigned int m);
    void setR(unsigned int r);
    void setN(unsigned int n);

    Eigen::MatrixXd getPositionGrid();

    static std::size_t array3DIndex(const unsigned long i,
                                    const unsigned long j,
                                    const unsigned long k,
                                    const unsigned long dim1,
                                    const unsigned long dim2,
                                    const unsigned long dim3) noexcept;

    static std::size_t array2DIndex(const unsigned long i,
                                    const unsigned long j,
                                    const unsigned long dim1,
                                    const unsigned long dim2) noexcept;

    static void degToRadMat(Eigen::MatrixXd &degSphCoords) noexcept;
};
