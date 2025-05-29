#include <JuceHeader.h>
#include "WaveShaperProAudioProcessorEditor.h"
#include "WaveShaperProAudioProcessor.h"
// WaveShaperLookAndFeel Implementation
WaveShaperLookAndFeel::WaveShaperLookAndFeel()
{
    setColour(juce::ResizableWindow::backgroundColourId, backgroundColour);
    setColour(juce::Slider::thumbColourId, accentColour);
    setColour(juce::Slider::rotarySliderFillColourId, accentColour);
    setColour(juce::Slider::rotarySliderOutlineColourId, sliderTrackColour);
    setColour(juce::ComboBox::backgroundColourId, backgroundColour);
    setColour(juce::ComboBox::outlineColourId, accentColour);
    setColour(juce::ComboBox::textColourId, textColour);
    setColour(juce::ComboBox::arrowColourId, accentColour);
    setColour(juce::ToggleButton::textColourId, textColour);
    setColour(juce::Label::textColourId, textColour);
}

void WaveShaperLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
    float sliderPos, const float rotaryStartAngle,
    const float rotaryEndAngle, juce::Slider& slider)
{
    const float radius = juce::jmin(width / 2, height / 2) - 4.0f;
    const float centreX = x + width * 0.5f;
    const float centreY = y + height * 0.5f;
    const float rx = centreX - radius;
    const float ry = centreY - radius;
    const float rw = radius * 2.0f;
    const float angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

    // Draw track
    g.setColour(sliderTrackColour);
    g.drawEllipse(rx, ry, rw, rw, 2.0f);

    // Draw fill arc
    juce::Path valueArc;
    valueArc.addPieSegment(rx, ry, rw, rw, rotaryStartAngle, angle, 0.0f);
    g.setColour(accentColour);
    g.strokePath(valueArc, juce::PathStrokeType(3.0f));

    // Draw thumb
    juce::Path thumb;
    const float thumbLength = radius * 0.8f;
    thumb.addRectangle(-2.0f, -thumbLength, 4.0f, thumbLength);
    g.setColour(accentColour);
    g.fillPath(thumb, juce::AffineTransform::rotation(angle).translated(centreX, centreY));
}

void WaveShaperLookAndFeel::drawComboBox(juce::Graphics& g, int width, int height, bool isButtonDown,
    int buttonX, int buttonY, int buttonWidth, int buttonHeight,
    juce::ComboBox& box)
{
    const juce::Rectangle<int> boxBounds(0, 0, width, height);

    g.setColour(backgroundColour);
    g.fillRoundedRectangle(boxBounds.toFloat(), 3.0f);

    g.setColour(accentColour);
    g.drawRoundedRectangle(boxBounds.toFloat().reduced(0.5f), 3.0f, 1.0f);

    // Draw arrow
    const float arrowX = 0.3f;
    const float arrowH = 0.2f;

    juce::Path path;
    path.startNewSubPath(buttonX + buttonWidth * 0.5f - buttonWidth * arrowX,
        buttonY + buttonHeight * 0.5f - buttonHeight * arrowH);
    path.lineTo(buttonX + buttonWidth * 0.5f, buttonY + buttonHeight * 0.5f + buttonHeight * arrowH);
    path.lineTo(buttonX + buttonWidth * 0.5f + buttonWidth * arrowX,
        buttonY + buttonHeight * 0.5f - buttonHeight * arrowH);

    g.setColour(accentColour);
    g.strokePath(path, juce::PathStrokeType(2.0f));
}

void WaveShaperLookAndFeel::drawToggleButton(juce::Graphics& g, juce::ToggleButton& button,
    bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
    const int fontSize = juce::jmin(15, button.getHeight() - 6);
    const float thickness = fontSize * 0.1f;

    const float halfThickness = thickness * 0.5f;
    const juce::Rectangle<float> tickBounds(button.getLocalBounds().toFloat().reduced(thickness, thickness));

    g.setColour(button.getToggleState() ? accentColour : sliderTrackColour);
    g.fillRoundedRectangle(tickBounds, 3.0f);

    if (button.getToggleState())
    {
        g.setColour(backgroundColour);
        const auto tick = tickBounds.reduced(tickBounds.getWidth() * 0.28f, tickBounds.getHeight() * 0.28f);
        g.fillRoundedRectangle(tick, 2.0f);
    }

    g.setColour(textColour);
    g.setFont(fontSize);

    if (!button.isEnabled())
        g.setOpacity(0.5f);

    g.drawFittedText(button.getButtonText(),
        button.getLocalBounds().withTrimmedLeft(static_cast<int>(tickBounds.getRight() + 10.0f)),
        juce::Justification::left, 10);
}

// Main Editor Implementation
WaveShaperProAudioProcessorEditor::WaveShaperProAudioProcessorEditor(WaveShaperProAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    setLookAndFeel(&customLookAndFeel);

    // Setup Drive controls
    setupSlider(driveSlider, driveLabel, driveValueLabel, "DRIVE", "%");
    driveSlider.setRange(0.0, 100.0, 0.1);
    driveSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);

    // Setup Curve Type combo
    setupLabel(curveTypeLabel, "CURVE TYPE");
    addAndMakeVisible(curveTypeCombo);
    curveTypeCombo.addItem("Soft Clip", 1);
    curveTypeCombo.addItem("Hard Clip", 2);
    curveTypeCombo.addItem("Asymmetric", 3);
    curveTypeCombo.addItem("Tube", 4);
    curveTypeCombo.setSelectedId(1);

    // Setup Input Gain controls
    setupSlider(inputGainSlider, inputGainLabel, inputGainValueLabel, "INPUT GAIN", " dB");
    inputGainSlider.setRange(-40.0, 40.0, 0.1);
    inputGainSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);

    // Setup Output Gain controls
    setupSlider(outputGainSlider, outputGainLabel, outputGainValueLabel, "OUTPUT GAIN", " dB");
    outputGainSlider.setRange(-40.0, 40.0, 0.1);
    outputGainSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);

    // Setup Mix controls
    setupSlider(mixSlider, mixLabel, mixValueLabel, "MIX", "%");
    mixSlider.setRange(0.0, 100.0, 0.1);
    mixSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);

    // Setup Symmetry controls
    setupSlider(symmetrySlider, symmetryLabel, symmetryValueLabel, "SYMMETRY", "%");
    symmetrySlider.setRange(-100.0, 100.0, 0.1);
    symmetrySlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);

    // Setup Bypass button
    addAndMakeVisible(bypassButton);
    bypassButton.setButtonText("BYPASS");
    bypassButton.setColour(juce::ToggleButton::textColourId, juce::Colours::white);

    // Create parameter attachments
    driveAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getValueTreeState(), "drive", driveSlider);
    curveTypeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        audioProcessor.getValueTreeState(), "curveType", curveTypeCombo);
    inputGainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getValueTreeState(), "inputGain", inputGainSlider);
    outputGainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getValueTreeState(), "outputGain", outputGainSlider);
    mixAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getValueTreeState(), "mix", mixSlider);
    symmetryAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.getValueTreeState(), "symmetry", symmetrySlider);
    bypassAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        audioProcessor.getValueTreeState(), "bypass", bypassButton);

    // Set initial size and start timer for value updates
    setSize(800, 600);
    setResizable(true, true);
    startTimerHz(30);
}

WaveShaperProAudioProcessorEditor::~WaveShaperProAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
}

void WaveShaperProAudioProcessorEditor::setupSlider(juce::Slider& slider, juce::Label& label,
    juce::Label& valueLabel, const juce::String& labelText,
    const juce::String& suffix)
{
    addAndMakeVisible(slider);
    slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);

    setupLabel(label, labelText);
    setupLabel(valueLabel, "0" + suffix);
    valueLabel.setFont(juce::Font(14.0f, juce::Font::bold));
}

void WaveShaperProAudioProcessorEditor::setupLabel(juce::Label& label, const juce::String& text)
{
    addAndMakeVisible(label);
    label.setText(text, juce::dontSendNotification);
    label.setJustificationType(juce::Justification::centred);
    label.setColour(juce::Label::textColourId, juce::Colours::white);
    label.setFont(juce::Font(12.0f, juce::Font::bold));
}

void WaveShaperProAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff2a2a2a));

    // Draw title
    g.setColour(juce::Colours::white);
    g.setFont(juce::Font(24.0f, juce::Font::bold));
    g.drawText("WaveShaper Pro", getLocalBounds().removeFromTop(50), juce::Justification::centred, true);

    // Draw decorative lines
    g.setColour(juce::Colour(0xff00aaff));
    g.fillRect(20, 45, getWidth() - 40, 2);
    g.fillRect(20, getHeight() - 50, getWidth() - 40, 2);
}

void WaveShaperProAudioProcessorEditor::resized()
{
    const int margin = 20;
    const int titleHeight = 50;
    const int footerHeight = 50;
    const int availableHeight = getHeight() - titleHeight - footerHeight;
    const int availableWidth = getWidth() - 2 * margin;

    // Calculate grid layout (3x2 for sliders + combo + bypass)
    const int cols = 3;
    const int rows = 3;
    const int sliderSize = 120;
    const int spacing = 20;

    const int totalWidth = cols * sliderSize + (cols - 1) * spacing;
    const int startX = margin + (availableWidth - totalWidth) / 2;
    const int startY = titleHeight + 30;

    // Row 1: Drive, Input Gain, Output Gain
    driveLabel.setBounds(startX, startY, sliderSize, 20);
    driveSlider.setBounds(startX, startY + 25, sliderSize, sliderSize);
    driveValueLabel.setBounds(startX, startY + 150, sliderSize, 20);

    inputGainLabel.setBounds(startX + (sliderSize + spacing), startY, sliderSize, 20);
    inputGainSlider.setBounds(startX + (sliderSize + spacing), startY + 25, sliderSize, sliderSize);
    inputGainValueLabel.setBounds(startX + (sliderSize + spacing), startY + 150, sliderSize, 20);

    outputGainLabel.setBounds(startX + 2 * (sliderSize + spacing), startY, sliderSize, 20);
    outputGainSlider.setBounds(startX + 2 * (sliderSize + spacing), startY + 25, sliderSize, sliderSize);
    outputGainValueLabel.setBounds(startX + 2 * (sliderSize + spacing), startY + 150, sliderSize, 20);

    // Row 2: Mix, Symmetry, Curve Type
    const int row2Y = startY + 200;

    mixLabel.setBounds(startX, row2Y, sliderSize, 20);
    mixSlider.setBounds(startX, row2Y + 25, sliderSize, sliderSize);
    mixValueLabel.setBounds(startX, row2Y + 150, sliderSize, 20);

    symmetryLabel.setBounds(startX + (sliderSize + spacing), row2Y, sliderSize, 20);
    symmetrySlider.setBounds(startX + (sliderSize + spacing), row2Y + 25, sliderSize, sliderSize);
    symmetryValueLabel.setBounds(startX + (sliderSize + spacing), row2Y + 150, sliderSize, 20);

    curveTypeLabel.setBounds(startX + 2 * (sliderSize + spacing), row2Y, sliderSize, 20);
    curveTypeCombo.setBounds(startX + 2 * (sliderSize + spacing), row2Y + 25, sliderSize, 30);

    // Bypass button
    bypassButton.setBounds(startX + 2 * (sliderSize + spacing), row2Y + 70, sliderSize, 30);
}

void WaveShaperProAudioProcessorEditor::timerCallback()
{
    updateValueLabels();
}

void WaveShaperProAudioProcessorEditor::updateValueLabels()
{
    driveValueLabel.setText(juce::String(driveSlider.getValue(), 1) + "%", juce::dontSendNotification);
    inputGainValueLabel.setText(juce::String(inputGainSlider.getValue(), 1) + " dB", juce::dontSendNotification);
    outputGainValueLabel.setText(juce::String(outputGainSlider.getValue(), 1) + " dB", juce::dontSendNotification);
    mixValueLabel.setText(juce::String(mixSlider.getValue(), 1) + "%", juce::dontSendNotification);
    symmetryValueLabel.setText(juce::String(symmetrySlider.getValue(), 1) + "%", juce::dontSendNotification);
}