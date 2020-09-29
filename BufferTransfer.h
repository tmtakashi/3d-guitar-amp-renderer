#include <juce_audio_processors/juce_audio_processors.h>

#pragma once

struct BufferWithSampleRate
{
    BufferWithSampleRate() = default;
    BufferWithSampleRate(juce::AudioBuffer<float> &&bufferIn,
                         double sampleRateIn)
        : buffer(std::move(bufferIn)), sampleRate(sampleRateIn)
    {
    }

    juce::AudioBuffer<float> buffer;
    double sampleRate = 0.0;
};

class BufferTransfer
{
  private:
    bool isNewBuffer = false;
    BufferWithSampleRate buffer;
    juce::SpinLock mutex;

  public:
    void set(const BufferWithSampleRate &buf)
    {
        const juce::SpinLock::ScopedLockType lock(mutex);
        buffer = buf;
        isNewBuffer = true;
    }

    void set(const BufferWithSampleRate &&buf)
    {
        const juce::SpinLock::ScopedLockType lock(mutex);
        buffer = buf;
        isNewBuffer = true;
    }

    template <typename Fn> void get(Fn &&fn)
    {
        const juce::SpinLock::ScopedTryLockType lock(mutex);
        if (lock.isLocked() && isNewBuffer)
        {
            fn(buffer);
            isNewBuffer = false;
        }
    }
};
