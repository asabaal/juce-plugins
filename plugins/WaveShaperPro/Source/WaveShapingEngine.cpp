#include "WaveShapingEngine.h"

float WaveShapingEngine::processSample(float input, WaveShapeType type, float drive, float symmetry) noexcept
{
    switch (type)
    {
    case WaveShapeType::SoftClip:
        return softClip(input, drive);
    case WaveShapeType::HardClip:
        return hardClip(input, drive);
    case WaveShapeType::Asymmetric:
        return asymmetricClip(input, drive, symmetry);
    case WaveShapeType::Tube:
        return tubeSimulation(input, drive);
    default:
        return input;
    }
}

void WaveShapingEngine::processBlock(juce::AudioBuffer<float>& buffer,
    WaveShapeType type,
    float drive,
    float symmetry) noexcept
{
    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();

    for (int channel = 0; channel < numChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);

        for (int sample = 0; sample < numSamples; ++sample)
        {
            channelData[sample] = processSample(channelData[sample], type, drive, symmetry);
        }
    }
}

float WaveShapingEngine::softClip(float input, float drive) noexcept
{
    const float drivenInput = input * drive;
    return std::tanh(drivenInput);
}

float WaveShapingEngine::hardClip(float input, float drive) noexcept
{
    const float drivenInput = input * drive;
    return std::clamp(drivenInput, -1.0f, 1.0f);
}

float WaveShapingEngine::asymmetricClip(float input, float drive, float symmetry) noexcept
{
    const float drivenInput = input * drive;

    if (drivenInput >= 0.0f)
    {
        const float positiveGain = 1.0f + (symmetry * 0.5f);
        return std::tanh(drivenInput * positiveGain);
    }
    else
    {
        const float negativeGain = 1.0f - (symmetry * 0.5f);
        return std::tanh(drivenInput * negativeGain);
    }
}

float WaveShapingEngine::tubeSimulation(float input, float drive) noexcept
{
    const float drivenInput = input * drive;
    const float abs_input = std::abs(drivenInput);

    if (abs_input <= 1.0f / 3.0f)
    {
        return 2.0f * drivenInput;
    }
    else if (abs_input <= 2.0f / 3.0f)
    {
        const float sign = drivenInput >= 0.0f ? 1.0f : -1.0f;
        return sign * (3.0f - std::pow(2.0f - 3.0f * abs_input, 2.0f)) / 3.0f;
    }
    else
    {
        return drivenInput >= 0.0f ? 1.0f : -1.0f;
    }
}

float WaveShapingEngine::saturate(float input) noexcept
{
    return std::clamp(input, -1.0f, 1.0f);
}

float WaveShapingEngine::signedPower(float input, float exponent) noexcept
{
    const float sign = input >= 0.0f ? 1.0f : -1.0f;
    return sign * std::pow(std::abs(input), exponent);
}