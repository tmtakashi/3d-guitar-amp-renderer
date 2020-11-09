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
