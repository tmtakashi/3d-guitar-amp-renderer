#ifndef EDA72C9F_9B25_4C13_9BF8_70404723EB0F
#define EDA72C9F_9B25_4C13_9BF8_70404723EB0F

#include "fftw3.h"
#include <complex>
#include <vector>

class Convolver
{
  public:
    enum class ConvolutionMethod
    {
        OverlapAdd,
        OverlapSave,
    };
    Convolver(Convolver::ConvolutionMethod convolutionMethod);
    void prepare(int bufSize, int fftSz, int irSz,
                 std::complex<float> *newIR) noexcept;
    void process(const float *in, float *out);
    // IR size need to be the same to call this function
    void setNewIR(std::complex<float> *newIR);

  private:
    int bufferSize = 0;
    int fftSize = 0;
    int rfftSize = 0;
    int irSize = 0;

    // pre-computed rfft array of the impulse response(size: fftSize / 2 + 1)
    std::complex<float> *IR;
    // rfft array container for input buffer
    std::vector<std::complex<float>> IN;
    // array container for frequency domain
    std::vector<std::complex<float>> Y;
    // array container for convolution result
    std::vector<float> filteredInput;
    std::vector<float> paddedIn;

    // array container for overlap add
    std::vector<float> overlapAddBuffer;
    int overlapAddBufferSize = 0;
    int overlapAddFrontPointer = 0;

    // array container for overlap save
    std::vector<float> overlapSaveBuffer;

    int overlapMask = 0;

    Convolver::ConvolutionMethod method;

    void fftConvolution(float *in, float *out);
};

#endif /* EDA72C9F_9B25_4C13_9BF8_70404723EB0F */
