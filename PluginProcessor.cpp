#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AudioPluginAudioProcessor::AudioPluginAudioProcessor()
    : AudioProcessor(
          BusesProperties()
#if !JucePlugin_IsMidiEffect
#if !JucePlugin_IsSynth
              .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
              .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
      )
{
}

AudioPluginAudioProcessor::~AudioPluginAudioProcessor() {}

//==============================================================================
const juce::String AudioPluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool AudioPluginAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool AudioPluginAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool AudioPluginAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double AudioPluginAudioProcessor::getTailLengthSeconds() const { return 0.0; }

int AudioPluginAudioProcessor::getNumPrograms()
{
    return 1; // NB: some hosts don't cope very well if you tell them there are
              // 0 programs, so this should be at least 1, even if you're not
              // really implementing programs.
}

int AudioPluginAudioProcessor::getCurrentProgram() { return 0; }

void AudioPluginAudioProcessor::setCurrentProgram(int index)
{
    juce::ignoreUnused(index);
}

const juce::String AudioPluginAudioProcessor::getProgramName(int index)
{
    juce::ignoreUnused(index);
    return {};
}

void AudioPluginAudioProcessor::changeProgramName(int index,
                                                  const juce::String &newName)
{
    juce::ignoreUnused(index, newName);
}

//==============================================================================
void AudioPluginAudioProcessor::prepareToPlay(double sampleRate,
                                              int samplesPerBlock)
{
    // convolver.prepare({sampleRate, (juce::uint32)samplesPerBlock,
    //                    (juce::uint32)juce::jmax(getTotalNumInputChannels(),
    //                                             getTotalNumOutputChannels())});
    // convolver.reset();
    // if (hrtfBuffers.hrtfL.empty() && hrtfBuffers.hrtfR.empty())
    // {

    juce::File file("/Users/takashiminagawa/src/github.com/tmtakashi/"
                    "3d-guitar-amp-renderer/irs/ir_090.wav");
    juce::AudioFormatManager formatManager;
    formatManager.registerBasicFormats();
    std::unique_ptr<juce::InputStream> stream(file.createInputStream());
    std::unique_ptr<juce::AudioFormatReader> reader(
        formatManager.createReaderFor(std::move(stream)));

    if (reader.get() != nullptr)
    {
        // load irs
        juce::Array<juce::File> files;
        juce::File("/Users/takashiminagawa/src/github.com/tmtakashi/"
                   "3d-guitar-amp-renderer/irs/")
            .findChildFiles(files, juce::File::findFilesAndDirectories, false,
                            "*.wav");
        bool putFoldersFirst = false;
        juce::File::NaturalFileComparator sorter(putFoldersFirst);
        files.sort(sorter);

        juce::AudioFormatManager formatManager;
        formatManager.registerBasicFormats();

        hrtfBuffers.hrtfsL.resize(files.size());
        hrtfBuffers.hrtfsR.resize(files.size());
        for (int i = 0; i < files.size(); ++i)
        {
            auto &file = files.getReference(i);
            std::unique_ptr<juce::InputStream> stream(file.createInputStream());
            std::unique_ptr<juce::AudioFormatReader> reader(
                formatManager.createReaderFor(std::move(stream)));

            if (reader.get() != nullptr)
            {
                juce::AudioBuffer<float> buffer(
                    static_cast<int>(reader->numChannels),
                    static_cast<int>(reader->lengthInSamples));
                reader->read(buffer.getArrayOfWritePointers(),
                             buffer.getNumChannels(), 0,
                             buffer.getNumSamples());
                fftSize =
                    std::pow(2, std::ceil(std::log(reader->lengthInSamples) /
                                          std::log(2)));
                buffer.setSize(buffer.getNumChannels(), fftSize, true);

                hrtfBuffers.hrtfsL[i].resize(fftSize / 2 + 1);
                hrtfBuffers.hrtfsR[i].resize(fftSize / 2 + 1);

                fftwf_plan p_fwd_L =
                    fftwf_plan_dft_r2c_1d(fftSize, buffer.getWritePointer(0),
                                          reinterpret_cast<fftwf_complex *>(
                                              hrtfBuffers.hrtfsL[i].data()),
                                          FFTW_ESTIMATE);
                fftwf_execute(p_fwd_L);

                fftwf_plan p_fwd_R =
                    fftwf_plan_dft_r2c_1d(fftSize, buffer.getWritePointer(1),
                                          reinterpret_cast<fftwf_complex *>(
                                              hrtfBuffers.hrtfsR[i].data()),
                                          FFTW_ESTIMATE);
                fftwf_execute(p_fwd_R);

                fftwf_destroy_plan(p_fwd_L);
                fftwf_destroy_plan(p_fwd_R);
            }
        }

        convolverL.prepare(samplesPerBlock, reader->lengthInSamples, fftSize,
                           hrtfBuffers.hrtfsL[0].data());
        convolverR.prepare(samplesPerBlock, reader->lengthInSamples, fftSize,
                           hrtfBuffers.hrtfsR[0].data());
    }
}
void AudioPluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

bool AudioPluginAudioProcessor::isBusesLayoutSupported(
    const BusesLayout &layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
    return true;
#else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono() &&
        layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

        // This checks if the input layout matches the output layout
#if !JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif

    return true;
#endif
}

void AudioPluginAudioProcessor::processBlock(juce::AudioBuffer<float> &buffer,
                                             juce::MidiBuffer &midiMessages)
{
    juce::ignoreUnused(midiMessages);

    juce::ScopedNoDenormals noDenormals;
    int totalNumInputChannels = getTotalNumInputChannels();
    int totalNumOutputChannels = getTotalNumOutputChannels();

    const int bufferNumChannels = buffer.getNumChannels();
    const int bufferNumSamples = buffer.getNumSamples();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    if (isNewIRSet)
    {
        convolverL.setNewIR(currentLeftIRPointer);
        convolverR.setNewIR(currentRightIRPointer);
        isNewIRSet = false;
    }

    auto *inBufferL = buffer.getReadPointer(0, 0);
    auto *inBufferR = buffer.getReadPointer(1, 0);
    auto *outBufferL = buffer.getWritePointer(0, 0);
    auto *outBufferR = buffer.getWritePointer(1, 0);

    convolverL.process(inBufferL, outBufferL);
    convolverR.process(inBufferR, outBufferR);
}

//==============================================================================
bool AudioPluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor *AudioPluginAudioProcessor::createEditor()
{
    return new AudioPluginAudioProcessorEditor(*this);
}

//==============================================================================
void AudioPluginAudioProcessor::getStateInformation(juce::MemoryBlock &destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    juce::ignoreUnused(destData);
}

void AudioPluginAudioProcessor::setStateInformation(const void *data,
                                                    int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory
    // block, whose contents will have been created by the getStateInformation()
    // call.
    juce::ignoreUnused(data, sizeInBytes);
}

void AudioPluginAudioProcessor::setCurrentIRPointer(int idx)
{
    currentLeftIRPointer = hrtfBuffers.hrtfsL[idx].data();
    currentRightIRPointer = hrtfBuffers.hrtfsR[idx].data();
    isNewIRSet = true;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter()
{
    return new AudioPluginAudioProcessor();
}
