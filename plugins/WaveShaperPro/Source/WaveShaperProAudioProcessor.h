#pragma once

#include "JucePluginConfig.h"
#include <JuceHeader.h>
#include "WaveShapingEngine.h"

class WaveShaperProAudioProcessor : public juce::AudioProcessor
{
public:
    WaveShaperProAudioProcessor();
    ~WaveShaperProAudioProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
#endif

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    // Parameter access
    juce::AudioProcessorValueTreeState& getValueTreeState() { return valueTreeState; }

private:
    // Parameter IDs
    static const juce::String DRIVE_ID;
    static const juce::String CURVE_TYPE_ID;
    static const juce::String INPUT_GAIN_ID;
    static const juce::String OUTPUT_GAIN_ID;
    static const juce::String MIX_ID;
    static const juce::String SYMMETRY_ID;
    static const juce::String BYPASS_ID;

    // Audio processing
    WaveShapingEngine waveShaper;
    juce::AudioBuffer<float> dryBuffer;

    // Parameter smoothing
    juce::SmoothedValue<float> driveSmoothed;
    juce::SmoothedValue<float> inputGainSmoothed;
    juce::SmoothedValue<float> outputGainSmoothed;
    juce::SmoothedValue<float> mixSmoothed;
    juce::SmoothedValue<float> symmetrySmoothed;

    // DC blocking filter
    juce::dsp::IIR::Filter<float> dcBlocker[2];

    // Parameter management
    juce::AudioProcessorValueTreeState valueTreeState;
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WaveShaperProAudioProcessor)
};