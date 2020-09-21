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
    fftwf_free(filteredInput);
    if (method == ConvolutionMethod::OverlapAdd)
    {
        fftwf_free(overlapAddBuffer);
    }
    else if (method == ConvolutionMethod::OverlapSave)
    {
        fftwf_free(overlapSaveBuffer);
    }
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

        overlapAddBuffer = (float *)calloc(overlapAddBufferSize, sizeof(float));
        ;
    }
    else if (method == ConvolutionMethod::OverlapSave)
    {
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

        fftConvolution(paddedIn, filteredInput);

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

        fftConvolution(paddedIn, filteredInput);

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

void Convolver::fftConvolution(float *in, float *out)
{
    fftwf_plan p_fwd = fftwf_plan_dft_r2c_1d(fftSize, in, IN, FFTW_ESTIMATE);
    fftwf_execute(p_fwd);
    fftwf_destroy_plan(p_fwd);

    for (int i = 0; i < rfftSize; ++i)
    {
        Y[i][0] = IN[i][0] * IR[i].real() - IN[i][1] * IR[i].imag(); // real
        Y[i][1] = IN[i][0] * IR[i].imag() + IN[i][1] * IR[i].real(); // imag
    }
    fftwf_plan p_inv = fftwf_plan_dft_c2r_1d(fftSize, Y, out, FFTW_ESTIMATE);
    fftwf_execute(p_inv);
    fftwf_destroy_plan(p_inv);
}

void Convolver::setNewIR(std::complex<float> *newIR) { IR = newIR; }