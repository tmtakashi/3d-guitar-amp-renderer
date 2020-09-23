#include "Convolver.hpp"

Convolver::Convolver(Convolver::ConvolutionMethod convolutionMethod)
    : method(convolutionMethod)
{
}

void Convolver::prepare(int bufSize, int irSz, int fftSz,
                        std::complex<float> *newIR) noexcept
{
    // set parameters
    bufferSize = bufSize;
    irSize = irSz;
    ;
    fftSize = fftSz;
    if (method == ConvolutionMethod::OverlapAdd)
    {
        overlapAddBufferSize = std::pow(
            2, std::ceil(std::log(bufferSize + irSize - 1) / std::log(2)));

        overlapAddBuffer.resize(overlapAddBufferSize, 0);
    }
    else if (method == ConvolutionMethod::OverlapSave)
    {
        overlapSaveBuffer.resize(fftSize, 0);
    }

    rfftSize = fftSize / 2 + 1;
    // assign pointer to pre-computed impulse response rFFT
    IR = newIR;

    // dynamic allocation
    IN.resize(fftSize / 2 + 1);
    Y.resize(fftSize / 2 + 1);
    filteredInput.resize(fftSize);
    paddedIn.resize(fftSize, 0.0f);

    overlapMask = overlapAddBufferSize - 1;
}

void Convolver::process(const float *in, float *out)
{
    if (method == ConvolutionMethod::OverlapAdd)
    {
        memcpy(paddedIn.data(), in, bufferSize * sizeof(float)); // zero-padding

        fftConvolution(paddedIn.data(), filteredInput.data());

        // calculate output buffer
        for (int i = 0; i < bufferSize; ++i)
        {
            out[i] = 0.3 * (filteredInput[i] / fftSize +
                            overlapAddBuffer[overlapAddFrontPointer]);
            ++overlapAddFrontPointer;
            overlapAddFrontPointer &= overlapMask; // wire-anding
        }

        // update overlap buffer
        if (overlapAddFrontPointer - bufferSize >= 0)
        {
            memset(&overlapAddBuffer[overlapAddFrontPointer] - bufferSize, 0,
                   bufferSize * sizeof(float));
        }
        else
        {
            memset(overlapAddBuffer.data(), 0,
                   overlapAddFrontPointer *
                       sizeof(float)); // fill front of the buffer
            memset(&overlapAddBuffer[overlapAddBufferSize -
                                     (bufferSize - overlapAddFrontPointer)],
                   0,
                   (bufferSize - overlapAddFrontPointer) *
                       sizeof(float)); // fill tail of the buffer
        }
        for (int i = 0; i < irSize - 1; ++i)
        {
            overlapAddBuffer[(overlapAddFrontPointer + i) & overlapMask] +=
                filteredInput[bufferSize + i] / fftSize;
        }
    }
    else if (method == ConvolutionMethod::OverlapSave)
    {
        // copy new input to the tail of buffer
        memcpy(&overlapSaveBuffer[fftSize - bufferSize], in,
               bufferSize * sizeof(float));
        // copy buffer since FFT is inplace
        std::copy(overlapSaveBuffer.begin(), overlapSaveBuffer.end(),
                  paddedIn.begin());

        fftConvolution(paddedIn.data(), filteredInput.data());

        // only use last bufferSize samples as output
        for (int i = 0; i < bufferSize; ++i)
        {
            out[i] = 0.3 * (filteredInput[fftSize - bufferSize + i] / fftSize);
        }

        // slide buffer
        memmove(overlapSaveBuffer.data(), &overlapSaveBuffer[bufferSize],
                (fftSize - bufferSize) * sizeof(float));
    }
}

void Convolver::fftConvolution(float *in, float *out)
{
    fftwf_plan p_fwd = fftwf_plan_dft_r2c_1d(
        fftSize, in, reinterpret_cast<fftwf_complex *>(IN.data()),
        FFTW_ESTIMATE);
    fftwf_execute(p_fwd);
    fftwf_destroy_plan(p_fwd);

    for (int i = 0; i < rfftSize; ++i)
    {
        Y[i] = {
            IN[i].real() * IR[i].real() - IN[i].imag() * IR[i].imag(), // real
            IN[i].real() * IR[i].imag() + IN[i].imag() * IR[i].real()  // imag
        };
    }
    fftwf_plan p_inv = fftwf_plan_dft_c2r_1d(
        fftSize, reinterpret_cast<fftwf_complex *>(Y.data()), out,
        FFTW_ESTIMATE);
    fftwf_execute(p_inv);
    fftwf_destroy_plan(p_inv);
}

void Convolver::setNewIR(std::complex<float> *newIR) { IR = newIR; }