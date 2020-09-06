#pragma once

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
            azimuth = azimuthDial.getValue();
    }
    // void styleMenuChanged()
    // {
    //     switch(styleMenu.getSelectedId())
    //     {
    //         case 1:
    //             textFont.setStyleFlags(juce::Font::plain);
    //             break;
    //         case 2:
    //             textFont.setStyleFlags(juce::Font::bold);
    //             break;
    //         case 3:
    //             textFont.setStyleFlags(juce::Font::italic);
    //             break;
    //         default:
    //             break;
    //     }
    //     textLabel.setFont(textFont);
    // }

    void filenameComponentChanged(juce::FilenameComponent *fileComponentThatHasChanged) override
    {
        if (fileComponentThatHasChanged == fileComp.get())
            readFile(fileComp->getCurrentFile());
    }

    void readFile(const juce::File &fileToRead)
    {
        if (!fileToRead.existsAsFile()) // [1]
            return;

        auto fileText = fileToRead.loadFileAsString();

        textContent->setText(fileText);
    }

private:

    juce::Slider azimuthDial;
    juce::Label testLabel;

    float azimuth;
    AudioPluginAudioProcessor &processorRef;

    juce::Label textLabel;
    juce::Font textFont{14.0f};
    juce::ComboBox styleMenu;

    // File reading
    std::unique_ptr<juce::FilenameComponent> fileComp;
    std::unique_ptr<juce::TextEditor> textContent;
    // juce::File path(File("/SofaReader").getChildFile("to").getChildFile("file.txt"));

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioPluginAudioProcessorEditor)
};
