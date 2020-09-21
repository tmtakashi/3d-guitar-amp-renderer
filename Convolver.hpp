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
    ~Convolver();
    void prepare(int bufSize, int irSz, std::complex<float> *newIR) noexcept;
    void process(const float *in, float *out);

  private:
    int bufferSize = 0;
    int fftSize = 0;
    int rfftSize = 0;
    int irSize = 0;

    // pre-computed rfft array of the impulse response(size: fftSize / 2 + 1)
    std::complex<float> *IR;
    // rfft array container for input buffer
    fftwf_complex *IN;
    // array container for frequency domain
    fftwf_complex *Y;
    // array container for convolution result
    float *filteredInput;
    float *paddedIn;

    // array container for overlap add
    float *overlapAddBuffer;
    int overlapAddBufferSize = 0;
    int overlapAddFrontPointer = 0;

    // array container for overlap save
    float *overlapSaveBuffer;

    int overlapMask = 0;

    Convolver::ConvolutionMethod method;
};

#endif /* EDA72C9F_9B25_4C13_9BF8_70404723EB0F */
