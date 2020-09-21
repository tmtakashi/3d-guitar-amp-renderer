#pragma once

#include "BufferTransfer.h"
#include "PluginProcessor.h"

//==============================================================================
class AudioPluginAudioProcessorEditor : public juce::AudioProcessorEditor,
                                        public juce::Slider::Listener,
                                        public juce::FilenameComponentListener
{
  public:
    explicit AudioPluginAudioProcessorEditor(AudioPluginAudioProcessor &);
    ~AudioPluginAudioProcessorEditor() override;

    //==============================================================================
    void paint(juce::Graphics &) override;
    void resized() override;

    void sliderValueChanged(juce::Slider *slider) override
    {
        if (slider == &azimuthDial)
        {
            // azimuth = azimuthDial.getValue();
            // if (azimuth < 0)
            // {
            //     azimuth = 180 - azimuth;
            // }
            // auto irBuffer = irs.at(azimuth);
            // processorRef.bufferTransfer.set(irBuffer);
        }
    }

    void filenameComponentChanged(
        juce::FilenameComponent *fileComponentThatHasChanged) override
    {
        if (fileComponentThatHasChanged == fileComp.get())
            readFile(fileComp->getCurrentFile());
    }

    void readFile(const juce::File &fileToRead)
    {
        if (!fileToRead.existsAsFile()) // [1]
            return;

        // processorRef.convolver.loadImpulseResponse(
        //     fileToRead, juce::dsp::Convolution::Stereo::yes,
        //     juce::dsp::Convolution::Trim::no, 0,
        //     juce::dsp::Convolution::Normalise::no);
    }

  private:
    juce::Slider azimuthDial;
    juce::Label testLabel;

    int azimuth;
    AudioPluginAudioProcessor &processorRef;

    juce::Label textLabel;
    juce::Font textFont{14.0f};
    juce::ComboBox styleMenu;

    // File reading
    std::unique_ptr<juce::FilenameComponent> fileComp;

    std::vector<BufferWithSampleRate> irs;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(
        AudioPluginAudioProcessorEditor)
};
