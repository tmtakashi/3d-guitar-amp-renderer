#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor(AudioPluginAudioProcessor &p)
    : AudioProcessorEditor(&p), processorRef(p)
{
    juce::ignoreUnused(processorRef);
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.

    // // setup ComboBox
    // addAndMakeVisible(textLabel);
    // textLabel.setText("Select the azimuth.", juce::dontSendNotification);
    // textLabel.setFont(textFont);

    // add items to the combo-box
    // addAndMakeVisible(styleMenu);
    // styleMenu.addItem("0", 1);
    // styleMenu.addItem("+45", 2);
    // styleMenu.addItem("+90", 3);
    // styleMenu.addItem("-90", 4);
    // styleMenu.addItem("-45", 5);
    // styleMenu.onChange = [this] { styleMenuChanged(); };
    // styleMenu.setSelectedId(1);

    // setup slider
    addAndMakeVisible(azimuthDial);
    azimuthDial.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalDrag);
    // azimuthDial.setRotaryParameters();
    azimuthDial.setRange(-180, 180);
    azimuthDial.addListener(this);

    // setup slider label
    addAndMakeVisible(testLabel);
    testLabel.setText("Azimuth", juce::dontSendNotification);
    testLabel.attachToComponent(&azimuthDial, true);

    // set up file uploader
    fileComp.reset(new juce::FilenameComponent("fileComp",
                                               {},                      // current file
                                               false,                   // can edit file name,
                                               false,                   // is directory,
                                               false,                   // is for saving,
                                               {},                      // browser wildcard suffix,
                                               {},                      // enforced suffix,
                                               "Select file to open")); // text when nothing selected

    addAndMakeVisible(fileComp.get());
    fileComp->addListener(this);

    textContent.reset(new juce::TextEditor());
    addAndMakeVisible(textContent.get());
    textContent->setMultiLine(true);
    textContent->setReadOnly(true);
    textContent->setCaretVisible(false);
    setSize(800, 500);
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
}

//==============================================================================
void AudioPluginAudioProcessorEditor::paint(juce::Graphics &g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    g.setColour(juce::Colours::white);
    g.setFont(15.0f);
    // g.drawFittedText("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void AudioPluginAudioProcessorEditor::resized()
{
    auto sliderLeft = 200;

    // setbounds(x, y, width, height)
    // file uploader
    textContent->setBounds(sliderLeft, 70, 400, 30);
    fileComp->setBounds(sliderLeft, 100, 400, 30);

    // azimuth dial
    azimuthDial.setBounds(sliderLeft, 150, 400, 400);
    textLabel.setBounds(sliderLeft, 450, 400, 50);
    // styleMenu.setBounds(sliderLeft, 250, getWidth() - sliderLeft, 20);
}
