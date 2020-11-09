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

    void connectButtonClicked()
    {
        if (! isConnected())     
            connect();
        else
            disconnect();
    }


    void connect()
    {
        auto portToConnect = portNumberField.getText().getIntValue();   

        if (!isValidOscPort (portToConnect))                           
        {
            handleInvalidPortNumberEntered();
            return;
        }

        if (oscReceiver.connect (portToConnect))                        
        {
            currentPortNumber = portToConnect;
            connectButton.setButtonText ("Disconnect");
        }
        else                                                            
        {
            handleConnectError (portToConnect);
        }
    }

    void disconnect()
    {
        if (oscReceiver.disconnect())   
        {
            currentPortNumber = -1;
            connectButton.setButtonText ("Connect");
        }
        else
        {
            handleDisconnectError();
        }
    }

    bool isConnected() const
    {
        return currentPortNumber != -1;
    }

    bool isValidOscPort (int port) const
    {
        return port > 0 && port < 65536;
    }

    void handleConnectError (int failedPort)
    {
        juce::AlertWindow::showMessageBoxAsync (juce::AlertWindow::WarningIcon,
                                                "OSC Connection error",
                                                "Error: could not connect to port " + juce::String (failedPort),
                                                "OK");
    }

    void showConnectionErrorMessage(const juce::String &messageText)
    {
        juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon,
                                               "Connection error", messageText,
                                               "OK");
    }

     void handleDisconnectError()
    {
        juce::AlertWindow::showMessageBoxAsync (juce::AlertWindow::WarningIcon,
                                                "Unknown error",
                                                "An unknown error occured while trying to disconnect from UDP port.",
                                                "OK");
    }

    void handleInvalidPortNumberEntered()
    {
        juce::AlertWindow::showMessageBoxAsync (juce::AlertWindow::WarningIcon,
                                                "Invalid port number",
                                                "Error: you have entered an invalid UDP port number.",
                                                "OK");
    }

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

    AudioPluginAudioProcessor &processorRef;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(
        AudioPluginAudioProcessorEditor)
};
