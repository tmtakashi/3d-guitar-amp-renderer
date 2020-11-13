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

    void sliderValueChanged(juce::Slider *slider) override;
    void filenameComponentChanged(
        juce::FilenameComponent *fileComponentThatHasChanged) override;
    void readFile(const juce::File &fileToRead);
    void oscMessageReceived(const juce::OSCMessage &message) override;
    void oscBundleReceived(const juce::OSCBundle &bundle) override;
    void connectButtonClicked();
    void connect();
    void disconnect();
    bool isConnected() const;
    bool isValidOscPort(int port) const;
    void handleConnectError(int failedPort);
    void showConnectionErrorMessage(const juce::String &messageText);
    void handleDisconnectError();
    void handleInvalidPortNumberEntered();

  private:
    juce::AudioProcessorValueTreeState &valueTreeState;
    juce::Slider azimuthDial;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        azimuthSliderAttachment;
    AzimuthDialLookAndFeel azimuthDialLookAndFeel;

    juce::Label azimuthLabel;
    juce::OSCReceiver oscReceiver;

    int currentPortNumber = -1;
    juce::Label portNumberLabel    { {}, "UDP Port Number: " };
    juce::Label portNumberField    { {}, "9001" };
    juce::TextButton connectButton { "Connect" };

    // File reading
    std::unique_ptr<juce::FilenameComponent> fileComp;
    juce::Array<juce::File> files;

    AudioPluginAudioProcessor &processorRef;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(
        AudioPluginAudioProcessorEditor)
};
