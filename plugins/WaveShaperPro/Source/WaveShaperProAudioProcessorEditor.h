#pragma once

#include <JuceHeader.h>
#include "WaveShaperProAudioProcessor.h"

class WaveShaperLookAndFeel : public juce::LookAndFeel_V4
{
public:
    WaveShaperLookAndFeel();
    ~WaveShaperLookAndFeel() override = default;

    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
        float sliderPos, const float rotaryStartAngle,
        const float rotaryEndAngle, juce::Slider& slider) override;

    void drawComboBox(juce::Graphics& g, int width, int height, bool isButtonDown,
        int buttonX, int buttonY, int buttonWidth, int buttonHeight,
        juce::ComboBox& box) override;

    void drawToggleButton(juce::Graphics& g, juce::ToggleButton& button,
        bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;

private:
    const juce::Colour backgroundColour{ 0xff2a2a2a };
    const juce::Colour accentColour{ 0xff00aaff };
    const juce::Colour textColour{ 0xffffffff };
    const juce::Colour sliderTrackColour{ 0xff404040 };
};

class WaveShaperProAudioProcessorEditor : public juce::AudioProcessorEditor,
    public juce::Timer
{
public:
    WaveShaperProAudioProcessorEditor(WaveShaperProAudioProcessor&);
    ~WaveShaperProAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;

private:
    WaveShaperProAudioProcessor& audioProcessor;
    WaveShaperLookAndFeel customLookAndFeel;

    // UI Components
    juce::Slider driveSlider;
    juce::Label driveLabel;
    juce::Label driveValueLabel;

    juce::ComboBox curveTypeCombo;
    juce::Label curveTypeLabel;

    juce::Slider inputGainSlider;
    juce::Label inputGainLabel;
    juce::Label inputGainValueLabel;

    juce::Slider outputGainSlider;
    juce::Label outputGainLabel;
    juce::Label outputGainValueLabel;

    juce::Slider mixSlider;
    juce::Label mixLabel;
    juce::Label mixValueLabel;

    juce::Slider symmetrySlider;
    juce::Label symmetryLabel;
    juce::Label symmetryValueLabel;

    juce::ToggleButton bypassButton;

    // Parameter attachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> driveAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> curveTypeAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> inputGainAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> outputGainAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mixAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> symmetryAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> bypassAttachment;

    // Helper methods
    void setupSlider(juce::Slider& slider, juce::Label& label, juce::Label& valueLabel,
        const juce::String& labelText, const juce::String& suffix = "");
    void setupLabel(juce::Label& label, const juce::String& text);
    void updateValueLabels();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WaveShaperProAudioProcessorEditor)
};