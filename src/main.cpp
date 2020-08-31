#include <iostream>
#include <Eigen/Dense>
#include "processor.hpp"
#include "io.hpp"

using namespace std;
using namespace Eigen;

int main()
{
    int order = 7;
    float fs = 48000.0;
    int num_samples_per_ir = pow(2, 10);

    int fc_low = (int)((num_samples_per_ir / fs) * 50);
    int fc_high = (int)((num_samples_per_ir / fs) * 8000);

    MatrixXd irs = get_irs_as_matrix("./data/irs", num_samples_per_ir);
    MatrixXd directions = get_direction_Matrix("./data/fullerene_polar.npy");
    HrirData hrir_data = get_hrir_data_from_sofa("./data/mit_kemar_normal_pinna.sofa");

    int nfft = (int)(irs.cols()/2);
    // FFT soundfield ir
    MatrixXcd IRs(irs.rows(), nfft);
    row_wise_rFFT(irs, IRs, nfft);

    // Convert soundfield ir to spherical domain
    MatrixXcd Pnm(nfft, (int)pow(order + 1, 2));
    for (int k = fc_low; k < fc_high; k++)
    {
        VectorXcd result_ir = SFT(IRs.col(k), directions, order);
        Pnm.row(k) = result_ir;
    }

    // FFT hrir 
    MatrixXcd HRTF_L(hrir_data.hrirs_l.rows(), nfft);
    MatrixXcd HRTF_R(hrir_data.hrirs_r.rows(), nfft);
    row_wise_rFFT(hrir_data.hrirs_l, HRTF_L, nfft);
    row_wise_rFFT(hrir_data.hrirs_r, HRTF_R, nfft);

    // Convert HRTF to spherical domain
    MatrixXcd Hnm_L(nfft, (int)pow(order + 1, 2));
    MatrixXcd Hnm_R(nfft, (int)pow(order + 1, 2));
    for (int k = fc_low; k < fc_high; k++)
    {
        VectorXcd result_l = SFT(HRTF_L.col(k), hrir_data.source_positions, order);
        Hnm_L.row(k) = result_l;
        VectorXcd result_r = SFT(HRTF_R.col(k), hrir_data.source_positions, order);
        Hnm_R.row(k) = result_r;
    }
    reverse_m(Pnm, order, nfft);

    MatrixXcd Pnm_Hnm_L = Pnm.cwiseProduct(Hnm_L);
    MatrixXcd Pnm_Hnm_R = Pnm.cwiseProduct(Hnm_R);

    cout << Pnm_Hnm_L << endl;
}
