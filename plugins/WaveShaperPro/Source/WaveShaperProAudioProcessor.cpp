#include "WaveShaperProAudioProcessor.h"
#include "WaveShaperProAudioProcessorEditor.h"

// Parameter IDs
const juce::String WaveShaperProAudioProcessor::DRIVE_ID = "drive";
const juce::String WaveShaperProAudioProcessor::CURVE_TYPE_ID = "curveType";
const juce::String WaveShaperProAudioProcessor::INPUT_GAIN_ID = "inputGain";
const juce::String WaveShaperProAudioProcessor::OUTPUT_GAIN_ID = "outputGain";
const juce::String WaveShaperProAudioProcessor::MIX_ID = "mix";
const juce::String WaveShaperProAudioProcessor::SYMMETRY_ID = "symmetry";
const juce::String WaveShaperProAudioProcessor::BYPASS_ID = "bypass";

WaveShaperProAudioProcessor::WaveShaperProAudioProcessor()
    : AudioProcessor(BusesProperties()
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
    valueTreeState(*this, nullptr, "Parameters", createParameterLayout())
{
    // Initialize smoothed values
    driveSmoothed.setCurrentAndTargetValue(1.0f);
    inputGainSmoothed.setCurrentAndTargetValue(1.0f);
    outputGainSmoothed.setCurrentAndTargetValue(1.0f);
    mixSmoothed.setCurrentAndTargetValue(1.0f);
    symmetrySmoothed.setCurrentAndTargetValue(0.0f);
}

WaveShaperProAudioProcessor::~WaveShaperProAudioProcessor()
{
}

juce::AudioProcessorValueTreeState::ParameterLayout WaveShaperProAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> parameters;

    // Drive parameter (0-100%)
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>(
        DRIVE_ID, "Drive",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
        50.0f, "%"));

    // Curve type parameter
    juce::StringArray curveChoices{ "Soft Clip", "Hard Clip", "Asymmetric", "Tube" };
    parameters.push_back(std::make_unique<juce::AudioParameterChoice>(
        CURVE_TYPE_ID, "Curve Type", curveChoices, 0));

    // Input gain (-40 to +40 dB)
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>(
        INPUT_GAIN_ID, "Input Gain",
        juce::NormalisableRange<float>(-40.0f, 40.0f, 0.1f),
        0.0f, "dB"));

    // Output gain (-40 to +40 dB)
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>(
        OUTPUT_GAIN_ID, "Output Gain",
        juce::NormalisableRange<float>(-40.0f, 40.0f, 0.1f),
        0.0f, "dB"));

    // Mix (0-100%)
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>(
        MIX_ID, "Mix",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
        100.0f, "%"));

    // Symmetry (-100 to +100%)
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>(
        SYMMETRY_ID, "Symmetry",
        juce::NormalisableRange<float>(-100.0f, 100.0f, 0.1f),
        0.0f, "%"));

    // Bypass
    parameters.push_back(std::make_unique<juce::AudioParameterBool>(
        BYPASS_ID, "Bypass", false));

    return { parameters.begin(), parameters.end() };
}

const juce::String WaveShaperProAudioProcessor::getName() const
{
    return "AA WaveShaper";
}

bool WaveShaperProAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool WaveShaperProAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool WaveShaperProAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double WaveShaperProAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int WaveShaperProAudioProcessor::getNumPrograms()
{
    return 1;
}

int WaveShaperProAudioProcessor::getCurrentProgram()
{
    return 0;
}

void WaveShaperProAudioProcessor::setCurrentProgram(int index)
{
    juce::ignoreUnused(index);
}

const juce::String WaveShaperProAudioProcessor::getProgramName(int index)
{
    juce::ignoreUnused(index);
    return {};
}

void WaveShaperProAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
    juce::ignoreUnused(index, newName);
}

void WaveShaperProAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Prepare smoothed values
    const double smoothingTimeMs = 20.0;
    driveSmoothed.reset(sampleRate, smoothingTimeMs / 1000.0);
    inputGainSmoothed.reset(sampleRate, smoothingTimeMs / 1000.0);
    outputGainSmoothed.reset(sampleRate, smoothingTimeMs / 1000.0);
    mixSmoothed.reset(sampleRate, smoothingTimeMs / 1000.0);
    symmetrySmoothed.reset(sampleRate, smoothingTimeMs / 1000.0);

    // Prepare dry buffer for mix processing
    dryBuffer.setSize(getTotalNumInputChannels(), samplesPerBlock);

    // Setup DC blocking filter
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
    spec.numChannels = static_cast<juce::uint32>(getTotalNumInputChannels());

    for (auto& filter : dcBlocker)
    {
        filter.coefficients = juce::dsp::IIR::Coefficients<float>::makeHighPass(sampleRate, 20.0f);
        filter.prepare(spec);
    }
}

void WaveShaperProAudioProcessor::releaseResources()
{
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool WaveShaperProAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
    return true;
#else
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;

#if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;
#endif

    return true;
#endif
}
#endif

void WaveShaperProAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused(midiMessages);
    juce::ScopedNoDenormals noDenormals;

    const int totalNumInputChannels = getTotalNumInputChannels();
    const int totalNumOutputChannels = getTotalNumOutputChannels();
    const int numSamples = buffer.getNumSamples();

    // Clear unused output channels
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, numSamples);

    // Check bypass
    const bool bypassed = *valueTreeState.getRawParameterValue(BYPASS_ID) > 0.5f;
    if (bypassed)
        return;

    // Get parameter values
    const float drivePercent = *valueTreeState.getRawParameterValue(DRIVE_ID);
    const float curveTypeFloat = *valueTreeState.getRawParameterValue(CURVE_TYPE_ID);
    const float inputGainDb = *valueTreeState.getRawParameterValue(INPUT_GAIN_ID);
    const float outputGainDb = *valueTreeState.getRawParameterValue(OUTPUT_GAIN_ID);
    const float mixPercent = *valueTreeState.getRawParameterValue(MIX_ID);
    const float symmetryPercent = *valueTreeState.getRawParameterValue(SYMMETRY_ID);

    // Convert to usable values
    const float drive = 1.0f + (drivePercent / 100.0f) * 9.0f; // 1-10x drive
    const WaveShapeType curveType = static_cast<WaveShapeType>(static_cast<int>(curveTypeFloat));
    const float inputGain = juce::Decibels::decibelsToGain(inputGainDb);
    const float outputGain = juce::Decibels::decibelsToGain(outputGainDb);
    const float mix = mixPercent / 100.0f;
    const float symmetry = symmetryPercent / 100.0f;

    // Update smoothed values
    driveSmoothed.setTargetValue(drive);
    inputGainSmoothed.setTargetValue(inputGain);
    outputGainSmoothed.setTargetValue(outputGain);
    mixSmoothed.setTargetValue(mix);
    symmetrySmoothed.setTargetValue(symmetry);

    // Store dry signal for mixing
    if (mix < 1.0f)
    {
        dryBuffer.makeCopyOf(buffer, true);
    }

    // Process each sample
    for (int sample = 0; sample < numSamples; ++sample)
    {
        const float currentDrive = driveSmoothed.getNextValue();
        const float currentInputGain = inputGainSmoothed.getNextValue();
        const float currentOutputGain = outputGainSmoothed.getNextValue();
        const float currentMix = mixSmoothed.getNextValue();
        const float currentSymmetry = symmetrySmoothed.getNextValue();

        for (int channel = 0; channel < totalNumInputChannels; ++channel)
        {
            auto* channelData = buffer.getWritePointer(channel);

            // Apply input gain
            float sample_value = channelData[sample] * currentInputGain;

            // Apply waveshaping
            sample_value = waveShaper.processSample(sample_value, curveType, currentDrive, currentSymmetry);

            // Apply output gain
            sample_value *= currentOutputGain;

            // Apply DC blocking
            sample_value = dcBlocker[channel].processSample(sample_value);

            // Mix with dry signal
            if (currentMix < 1.0f)
            {
                const float dryValue = dryBuffer.getSample(channel, sample);
                sample_value = dryValue * (1.0f - currentMix) + sample_value * currentMix;
            }

            // Store processed sample
            channelData[sample] = sample_value;
        }
    }
}

bool WaveShaperProAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* WaveShaperProAudioProcessor::createEditor()
{
    return new WaveShaperProAudioProcessorEditor(*this);
}

void WaveShaperProAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto xml = valueTreeState.copyState().createXml();
    copyXmlToBinary(*xml, destData);
}

void WaveShaperProAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    auto xmlState = getXmlFromBinary(data, sizeInBytes);
    if (xmlState != nullptr)
        valueTreeState.replaceState(juce::ValueTree::fromXml(*xmlState));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new WaveShaperProAudioProcessor();
}