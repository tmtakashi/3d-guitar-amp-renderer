#include "PluginEditor.h"
#include "BinaryData.h"

#include "PluginProcessor.h"
#include "fftw3.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor(
    AudioPluginAudioProcessor &p)
    : AudioProcessorEditor(&p), processorRef(p), valueTreeState(p.parameters)
{
    juce::ignoreUnused(processorRef);

    addAndMakeVisible(azimuthDial);
    azimuthDial.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalDrag);
    azimuthDial.setRotaryParameters({0, 2 * juce::float_Pi, true});
    azimuthDial.setRange(0, 359, 1);
    azimuthDial.setValue(0);
    azimuthDial.setLookAndFeel(&azimuthDialLookAndFeel);
    azimuthDial.addListener(this);
    azimuthSliderAttachment.reset(
        new juce::AudioProcessorValueTreeState::SliderAttachment(
            valueTreeState, "azimuthIdx", azimuthDial));

    // setup slider label
    addAndMakeVisible(azimuthLabel);
    azimuthLabel.setText("Azimuth", juce::dontSendNotification);
    azimuthLabel.attachToComponent(&azimuthDial, true);
    // set up file uploader
    fileComp.reset(new juce::FilenameComponent(
        "fileComp", {},          // current file
        false,                   // can edit file name,
        true,                   // is directory,
        false,                   // is for saving,
        {},                      // browser wildcard suffix,
        {},                      // enforced suffix,
        "Select folder of BRIR wav files to open")); // text when nothing selected

    addAndMakeVisible(fileComp.get());
    fileComp->addListener(this);

    oscReceiver.addListener(this);

    addAndMakeVisible (portNumberLabel);

    portNumberField.onTextChange = [this] {};
    addAndMakeVisible (portNumberField);

    addAndMakeVisible (connectButton);
    connectButton.onClick = [this] { connectButtonClicked(); };

    setSize(600, 500);
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor() {}

//==============================================================================
void AudioPluginAudioProcessorEditor::paint(juce::Graphics &g)
{
    // (Our component is opaque, so we must completely fill the background with
    // a solid colour)
    g.fillAll(
        getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    g.setColour(juce::Colours::white);
    g.setFont(15.0f);

    const char *imageData = BinaryData::amp_png;
    const int imageDatasize = BinaryData::amp_pngSize;
    juce::Image amp = juce::ImageCache::getFromMemory(imageData, imageDatasize);
    g.drawImageWithin(amp, 260, 50, 100, 150,
                      juce::RectanglePlacement::centred);
}

void AudioPluginAudioProcessorEditor::resized()
{
    auto sliderLeft = 75;
    // file uploader
    fileComp->setBounds(sliderLeft, 10, 400, 30);

    // azimuth dial
    azimuthDial.setBounds(sliderLeft, 250, 400, 400);
    
    // UDP Port connection
    portNumberLabel.setBounds (getWidth() - 300, getHeight() - 50, 130, 25);
    portNumberField.setBounds (getWidth() - 180, getHeight() - 50, 50, 25);
    connectButton.setBounds (getWidth() - 120,getHeight() - 50, 100, 25);
}

void AudioPluginAudioProcessorEditor::sliderValueChanged(juce::Slider *slider)
{
    if (slider == &azimuthDial && processorRef.isIRLoaded)
    {
        int azimuth = azimuthDial.getValue();
        processorRef.setCurrentIRPointer(azimuth);
    }
}

void AudioPluginAudioProcessorEditor::filenameComponentChanged(
    juce::FilenameComponent *fileComponentThatHasChanged) 
{
    if (fileComponentThatHasChanged == fileComp.get())
    {
        readFile(fileComp->getCurrentFile());
    }
}

void AudioPluginAudioProcessorEditor::readFile(const juce::File &fileToRead)
{
    if (!fileToRead.exists())
    {
        return;
    }
    auto directoryPath = fileToRead.getFullPathName();
    processorRef.loadIRFiles(directoryPath);
}

void AudioPluginAudioProcessorEditor::oscMessageReceived(const juce::OSCMessage &message) 
{
    if (message.getAddressPattern().toString() == "/gyrosc/gyro" &&
        message.size() == 3 && message[2].isFloat32())
    {
        int degrees = juce::radiansToDegrees(message[2].getFloat32());
        degrees = degrees >= 0 ? degrees : 360 + degrees;
        azimuthDial.setValue(degrees);
    }
}

void AudioPluginAudioProcessorEditor::oscBundleReceived(const juce::OSCBundle &bundle) 
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

void AudioPluginAudioProcessorEditor::connectButtonClicked()
{
    if (! isConnected())     
        connect();
    else
        disconnect();
}


void AudioPluginAudioProcessorEditor::connect()
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

void AudioPluginAudioProcessorEditor::disconnect()
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

bool AudioPluginAudioProcessorEditor::isConnected() const
{
    return currentPortNumber != -1;
}

bool AudioPluginAudioProcessorEditor::isValidOscPort (int port) const
{
    return port > 0 && port < 65536;
}

void AudioPluginAudioProcessorEditor::handleConnectError (int failedPort)
{
    juce::AlertWindow::showMessageBoxAsync (juce::AlertWindow::WarningIcon,
                                            "OSC Connection error",
                                            "Error: could not connect to port " + juce::String (failedPort),
                                            "OK");
}

void AudioPluginAudioProcessorEditor::showConnectionErrorMessage(const juce::String &messageText)
{
    juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon,
                                            "Connection error", messageText,
                                            "OK");
}

void AudioPluginAudioProcessorEditor::handleDisconnectError()
{
    juce::AlertWindow::showMessageBoxAsync (juce::AlertWindow::WarningIcon,
                                            "Unknown error",
                                            "An unknown error occured while trying to disconnect from UDP port.",
                                            "OK");
}

void AudioPluginAudioProcessorEditor::handleInvalidPortNumberEntered()
{
    juce::AlertWindow::showMessageBoxAsync (juce::AlertWindow::WarningIcon,
                                            "Invalid port number",
                                            "Error: you have entered an invalid UDP port number.",
                                            "OK");
}