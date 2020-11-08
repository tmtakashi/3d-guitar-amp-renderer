#pragma once

#include "PluginProcessor.h"
#include "juce_osc/juce_osc.h"

#include "AzimuthDialLookAndFeel.h"
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
        if (slider == &azimuthDial && processorRef.isIRLoaded)
        {
            int azimuth = azimuthDial.getValue();
            processorRef.setCurrentIRPointer(azimuth);
        }
    }

    void filenameComponentChanged(
        juce::FilenameComponent *fileComponentThatHasChanged) override
    {
        if (fileComponentThatHasChanged == fileComp.get())
        {
            readFile(fileComp->getCurrentFile());
        }
    }

    void readFile(const juce::File &fileToRead)
    {
        if (!fileToRead.exists())
        {
            return;
        }
        auto directoryPath = fileToRead.getFullPathName();
        std::cout << directoryPath << std::endl;
        processorRef.loadIRFiles(directoryPath);
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
            int degrees = juce::radiansToDegrees(message[2].getFloat32());
            degrees = degrees >= 0 ? degrees : 360 + degrees;
            azimuthDial.setValue(degrees);
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
    juce::AudioProcessorValueTreeState &valueTreeState;
    juce::Slider azimuthDial;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        azimuthSliderAttachment;
    AzimuthDialLookAndFeel azimuthDialLookAndFeel;

    juce::Label testLabel;

    AudioPluginAudioProcessor &processorRef;

    juce::Label textLabel;
    juce::Font textFont{14.0f};
    juce::ComboBox styleMenu;

    juce::OSCReceiver oscReceiver;

    // File reading
    std::unique_ptr<juce::FilenameComponent> fileComp;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(
        AudioPluginAudioProcessorEditor)
};
