#include "Convolver.hpp"

Convolver::Convolver(Convolver::ConvolutionMethod convolutionMethod)
    : method(convolutionMethod)
{
}

Convolver::~Convolver()
{
    fftwf_free(IN);
    fftwf_free(Y);
    fftwf_free(paddedIn);
    if (method == ConvolutionMethod::OverlapAdd)
    {
        fftwf_free(overlapAddBuffer);
    }
    else if (method == ConvolutionMethod::OverlapSave)
    {
        fftwf_free(overlapSaveBuffer);
    }
    fftwf_free(filteredInput);
}

void Convolver::prepare(int bufSize, int irSz,
                        std::complex<float> *newIR) noexcept
{
    // set parameters
    bufferSize = bufSize;
    irSize = irSz;
    ;
    if (method == ConvolutionMethod::OverlapAdd)
    {
        overlapAddBufferSize = std::pow(
            2, std::ceil(std::log(bufferSize + irSize - 1) / std::log(2)));

        overlapAddBuffer = (float *)calloc(overlapAddBufferSize, sizeof(float));
        fftSize = std::pow(2, std::ceil(std::log(irSz) / std::log(2)));
        ;
    }
    else if (method == ConvolutionMethod::OverlapSave)
    {
        fftSize = std::pow(
            2, std::ceil(std::log(irSz + bufferSize - 1) / std::log(2)));
        overlapSaveBuffer = (float *)calloc(fftSize, sizeof(float));
    }

    rfftSize = fftSize / 2 + 1;
    // assign pointer to pre-computed impulse response rFFT
    IR = newIR;

    // dynamic allocation
    IN = (fftwf_complex *)fftwf_malloc(sizeof(fftwf_complex) *
                                       (fftSize / 2 + 1));
    Y = (fftwf_complex *)fftwf_malloc(sizeof(fftwf_complex) *
                                      (fftSize / 2 + 1));
    filteredInput = (float *)fftwf_malloc(sizeof(float) * fftSize);
    paddedIn = (float *)calloc(
        fftSize, sizeof(float)); // calloc initializes elements to zero

    overlapMask = overlapAddBufferSize - 1;
}

void Convolver::process(const float *in, float *out)
{
    if (method == ConvolutionMethod::OverlapAdd)
    {
        memcpy(paddedIn, in, bufferSize * sizeof(float)); // zero-padding
        fftwf_plan p_fwd =
            fftwf_plan_dft_r2c_1d(fftSize, paddedIn, IN, FFTW_ESTIMATE);
        fftwf_execute(p_fwd);
        fftwf_destroy_plan(p_fwd);
        memset(paddedIn, 0,
               bufferSize * sizeof(float)); // reset values after in-place fft

        for (int i = 0; i < rfftSize; ++i)
        {
            Y[i][0] = IN[i][0] * IR[i].real() - IN[i][1] * IR[i].imag(); // real
            Y[i][1] = IN[i][0] * IR[i].imag() + IN[i][1] * IR[i].real(); // imag
        }
        fftwf_plan p_inv =
            fftwf_plan_dft_c2r_1d(fftSize, Y, filteredInput, FFTW_ESTIMATE);
        fftwf_execute(p_inv);
        fftwf_destroy_plan(p_inv);

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
            memset(overlapAddBuffer, 0,
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
        memcpy(paddedIn, overlapSaveBuffer, fftSize * sizeof(float));

        // perform FFT for input
        fftwf_plan p_fwd =
            fftwf_plan_dft_r2c_1d(fftSize, paddedIn, IN, FFTW_ESTIMATE);
        fftwf_execute(p_fwd);
        fftwf_destroy_plan(p_fwd);

        for (int i = 0; i < rfftSize; ++i)
        {
            Y[i][0] = IN[i][0] * IR[i].real() - IN[i][1] * IR[i].imag(); // real
            Y[i][1] = IN[i][0] * IR[i].imag() + IN[i][1] * IR[i].real(); // imag
        }
        fftwf_plan p_inv =
            fftwf_plan_dft_c2r_1d(fftSize, Y, filteredInput, FFTW_ESTIMATE);
        fftwf_execute(p_inv);
        fftwf_destroy_plan(p_inv);

        // only use last bufferSize samples as output
        for (int i = 0; i < bufferSize; ++i)
        {
            out[i] = 0.3 * (filteredInput[fftSize - bufferSize + i] / fftSize);
        }

        // slide buffer
        memmove(overlapSaveBuffer, &overlapSaveBuffer[bufferSize],
                (fftSize - bufferSize) * sizeof(float));
    }
}