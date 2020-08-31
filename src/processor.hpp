#include <cmath>
#include <complex>
#include <Eigen/Dense>
#include <Eigen/SVD>
#include <unsupported/Eigen/FFT>
#include <Eigen/Core>
#include <boost/math/special_functions/spherical_harmonic.hpp>

using namespace std;
using namespace Eigen;


VectorXd get_m_vector(unsigned int order)
{
    VectorXd m_array((int)pow(order + 1, 2));
    int i;
    for (int n = 0; n <= order; n++)
    {
        for (int m=-n; m <=n; m++)
        {
            m_array(i) = pow(-1, m);
            i++;
        }
    }
    return m_array;
}

void reverse_m(MatrixXcd &mat, unsigned int order, unsigned int nfft)
{
    for (int k; k < nfft; k++)
    {
        for (int n = 0; n <= order; n++)
        {
            for (int m=-n; m <=n; m++)
            {
                mat(k, pow(n, 2) + n + m) = mat(k, pow(n, 2) + n - m);
            }
        }
    }
}

complex<double> spherial_harmonic(unsigned int n, int m, double phi, double theta)
{
    return boost::math::spherical_harmonic(n, m, theta, phi);
}

void get_SH_matrix(const int &order, const MatrixXd &directions_in_rad, MatrixXcd &Y)
{
    int m, j;
    complex<double> Ynm;
    int num_directions = directions_in_rad.rows();
    for (int dir_idx = 0; dir_idx < num_directions; dir_idx++)
    {
        for (int n = 0; n <= order; n++)
        {
            for (m=-n, j = 0; m <=n; m++, j++)
            {
                RowVectorXd direction_in_rad = directions_in_rad.row(dir_idx);
                Ynm  = spherial_harmonic(n, m, direction_in_rad(0), direction_in_rad(1));
                Y(dir_idx, pow(n, 2) + n + m) = Ynm;
            }
        }
    }
}

VectorXcd SFT(const VectorXcd &Signals, const MatrixXd &directions_in_rad, const int &order)
{
    MatrixXcd Y(directions_in_rad.rows(), (int)pow(order + 1, 2));
    get_SH_matrix(order, directions_in_rad, Y);
    JacobiSVD<MatrixXcd> svd(Y, Eigen::ComputeFullU | Eigen::ComputeFullV);
    auto V = svd.matrixV();
    auto sigInv = svd.singularValues().cwiseInverse();
    auto U = svd.matrixU().adjoint();
    VectorXcd result = svd.solve(Signals);
    return result;
}

void row_wise_rFFT(MatrixXd &signals, MatrixXcd &Signals, int nfft)
{
    Eigen::FFT<double> fft;
    int rows = signals.rows();
    for (int i = 0; i < rows; i++)
    {
        RowVectorXcd tmp_out(Signals.cols());
        fft.fwd(tmp_out, signals.row(i), nfft);
        Signals.row(i) = tmp_out;
    }
}