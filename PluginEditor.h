#pragma once

#include "BufferTransfer.h"
#include "PluginProcessor.h"
#include "juce_osc/juce_osc.h"

//==============================================================================
class AudioPluginAudioProcessorEditor
    : public juce::AudioProcessorEditor,
      public juce::Slider::Listener,
      public juce::FilenameComponentListener,
      private juce::OSCReceiver::Listener<
          juce::OSCReceiver::MessageLoopCallback>
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
            azimuth = azimuthDial.getValue();
            std::size_t idx = azimuth >= 0 ? azimuth : 180 - azimuth;
            processorRef.setCurrentIRPointer(idx);
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
    void showConnectionErrorMessage(const juce::String &messageText)
    {
        juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon,
                                               "Connection error", messageText,
                                               "OK");
    }

    void oscMessageReceived(const juce::OSCMessage &message) override
    {
        if (message.getAddressPattern().toString() == "/gyrosc/gyro" &&
            message.size() == 3 && message[2].isFloat32())
        {
            azimuthDial.setValue(
                juce::radiansToDegrees(message[2].getFloat32()));
        }
    }

    void oscBundleReceived(const juce::OSCBundle &bundle) override
    {
        for (int i = 0; i < bundle.size(); ++i)
        {
            auto elem = bundle[i];
            if (elem.isMessage())
                oscMessageReceived(elem.getMessage());
            else if (elem.isBundle())
                oscBundleReceived(elem.getBundle());
        }
    }

  private:
    juce::Slider azimuthDial;
    juce::Label testLabel;

    int azimuth;
    AudioPluginAudioProcessor &processorRef;

    juce::Label textLabel;
    juce::Font textFont{14.0f};
    juce::ComboBox styleMenu;

    juce::OSCReceiver oscReceiver;

    // File reading
    std::unique_ptr<juce::FilenameComponent> fileComp;

    std::vector<BufferWithSampleRate> irs;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(
        AudioPluginAudioProcessorEditor)
};
