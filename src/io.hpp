#include <cmath>
#include <Eigen/Dense>
#include <vector>
#include "AudioFile.h"
#include "libsofa/src/SOFA.h"
#include "cnpy.h"

#define degreesToRadians(angleDegrees) ((angleDegrees) * M_PI / 180.0)
#define radiansToDegrees(angleRadians) ((angleRadians) * 180.0 / M_PI)

namespace fs = std::filesystem;
using namespace std;
using namespace Eigen;
typedef unsigned int uint;

static inline const std::size_t array3DIndex(const unsigned long i,
    const unsigned long j,
    const unsigned long k,
    const unsigned long dim1,
    const unsigned long dim2,
    const unsigned long dim3)
{
    return dim2 * dim3 * i + dim3 * j + k;
}


static inline const std::size_t array2DIndex(const unsigned long i,
    const unsigned long j,
    const unsigned long dim1,
    const unsigned long dim2)
{
    return dim2 * i + j;
}


struct HrirData
{
    MatrixXd hrirs_l;
    MatrixXd hrirs_r;
    MatrixXd source_positions;
};

vector<string> get_sorted_ir_path(const fs::path &path)
{
    vector<string> ir_paths;
    for (const auto& dirEntry : fs::recursive_directory_iterator(path))
        ir_paths.push_back(dirEntry.path().string());
    sort(ir_paths.begin(), ir_paths.end());
    return ir_paths;
}

MatrixXd get_irs_as_matrix(const string &path, const int &num_samples)
{
    fs::path p(path);
    vector<string> ir_paths = get_sorted_ir_path(p);
    MatrixXd irs(ir_paths.size(), num_samples);
    AudioFile<double> audioFile;
    for (size_t ir_idx = 0; ir_idx < ir_paths.size(); ir_idx++)
    {
        audioFile.load(ir_paths[ir_idx]);
        RowVectorXd samples = Map<RowVectorXd>(&audioFile.samples[0][0], num_samples);
        irs.row(ir_idx) = samples;
    }

    return irs;
}

MatrixXd get_direction_Matrix(const string &path)
{
    cnpy::NpyArray npy = cnpy::npy_load(path);
    int num_row = npy.shape[0];
    int num_col = npy.shape[1];
    double* arr = npy.data<double>();
    MatrixXd directions(num_row, num_col);
    for (int i = 0; i < num_row; i++)
    {
        for (int j = 0; j < num_col; j++)
        {
            directions(i, j)  = arr[i * num_col + j];
        }
    }

    return directions;
}

HrirData get_hrir_data_from_sofa(string path)
{
    const sofa::SimpleFreeFieldHRIR hrir(path);
    assert(hrir.IsValid());

    double fs = 0.0;
    const bool ok = hrir.GetSamplingRate(fs);
    assert(ok);

    const sofa::File theFile(path);
    std::vector< double > pos;
    std::vector< std::size_t > dims;
    theFile.GetVariableDimensions(dims, "SourcePosition");
    pos.resize(dims[0] * dims[1]);

    theFile.GetSourcePosition(&pos[0], dims[0], dims[1]);

    const unsigned int M = (unsigned int)hrir.GetNumMeasurements();
    const unsigned int R = (unsigned int)hrir.GetNumReceivers();
    const unsigned int N = (unsigned int)hrir.GetNumDataSamples();

    HrirData hrir_data{
        MatrixXd(M, N),
        MatrixXd(M, N),
        MatrixXd(dims[0], dims[1]),
    };

    std::vector< double > data;

    hrir.GetDataIR(data);

    for (std::size_t i = 0; i < M; i++)
    {
        for (std::size_t j = 0; j < R; j++)
        {
            for (std::size_t k = 0; k < N; k++)
            {
                const std::size_t index = array3DIndex(i, j, k, M, R, N);
                if (j == 0)
                {
                    hrir_data.hrirs_l(i, k) = data[index];
                }
                else
                {
                    hrir_data.hrirs_r(i, k) = data[index];
                }
            }
        }
    }


    for (std::size_t i = 0; i < dims[0]; i++)
    {
        for (std::size_t j = 0; j < dims[1]; j++)
        {
            const std::size_t index = array2DIndex(i, j, dims[0], dims[1]);
            if (j < 2)
            {
                hrir_data.source_positions(i, j) = degreesToRadians(pos[index]);
            }
            else
            {
                hrir_data.source_positions(i, j) = pos[index];
            }
        }
    }


    return hrir_data;
}