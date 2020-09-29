#pragma once

#include "Convolver.hpp"
#include "juce_dsp/juce_dsp.h"
#include <juce_audio_processors/juce_audio_processors.h>

//==============================================================================
class AudioPluginAudioProcessor : public juce::AudioProcessor
{
  public:
    //==============================================================================
    AudioPluginAudioProcessor();
    ~AudioPluginAudioProcessor() override;

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported(const BusesLayout &layouts) const override;

    void processBlock(juce::AudioBuffer<float> &, juce::MidiBuffer &) override;

    //==============================================================================
    juce::AudioProcessorEditor *createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String &newName) override;

    //==============================================================================
    void getStateInformation(juce::MemoryBlock &destData) override;
    void setStateInformation(const void *data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState parameters;
    int fftSize;
    Convolver::ConvolutionMethod convolutionMethod =
        Convolver::ConvolutionMethod::UniformOLS;
    Convolver convolverL{convolutionMethod};
    Convolver convolverR{convolutionMethod};
    struct HrtfBuffers

    {
        std::vector<std::vector<std::complex<float>>> hrtfsL;
        std::vector<std::vector<std::complex<float>>> hrtfsR;
    } hrtfBuffers;

    void setCurrentIRPointer(int idx);

  private:
    std::complex<float> *currentLeftIRPointer;
    std::complex<float> *currentRightIRPointer;
    bool isNewIRSet = false;
    std::atomic<int> *azimuthIdx = nullptr;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioPluginAudioProcessor)
};
