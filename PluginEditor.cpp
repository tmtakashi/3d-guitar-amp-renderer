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
    azimuthDial.setRange(0, 360, 1);
    azimuthDial.setValue(0);
    azimuthDial.setLookAndFeel(&azimuthDialLookAndFeel);
    azimuthDial.addListener(this);
    azimuthSliderAttachment.reset(
        new juce::AudioProcessorValueTreeState::SliderAttachment(
            valueTreeState, "azimuthIdx", azimuthDial));

    // setup slider label
    addAndMakeVisible(testLabel);
    testLabel.setText("Azimuth", juce::dontSendNotification);
    testLabel.attachToComponent(&azimuthDial, true);
    // set up file uploader
    // fileComp.reset(new juce::FilenameComponent(
    //     "fileComp", {},          // current file
    //     false,                   // can edit file name,
    //     false,                   // is directory,
    //     false,                   // is for saving,
    //     {},                      // browser wildcard suffix,
    //     {},                      // enforced suffix,
    //     "Select file to open")); // text when nothing selected

    // addAndMakeVisible(fileComp.get());
    // fileComp->addListener(this);

    oscReceiver.addListener(this);
    oscReceiver.connect(9001);

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

    // setbounds(x, y, width, height)
    // file uploader
    // fileComp->setBounds(sliderLeft, 50, 400, 30);

    // azimuth dial
    azimuthDial.setBounds(sliderLeft, 250, 400, 400);
    textLabel.setBounds(sliderLeft + 50, 25, 200, 50);
    // styleMenu.setBounds(sliderLeft, 250, getWidth() - sliderLeft, 20);
}
