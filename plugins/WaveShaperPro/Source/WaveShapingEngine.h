#pragma once

#include <JuceHeader.h>
#include <cmath>
#include <algorithm>

enum class WaveShapeType
{
    SoftClip = 0,
    HardClip,
    Asymmetric,
    Tube
};

class WaveShapingEngine
{
public:
    WaveShapingEngine() = default;
    ~WaveShapingEngine() = default;

    // Process a single sample
    float processSample(float input, WaveShapeType type, float drive, float symmetry = 0.0f) noexcept;

    // Process a block of samples
    void processBlock(juce::AudioBuffer<float>& buffer,
        WaveShapeType type,
        float drive,
        float symmetry = 0.0f) noexcept;

private:
    // Individual waveshaping functions
    static float softClip(float input, float drive) noexcept;
    static float hardClip(float input, float drive) noexcept;
    static float asymmetricClip(float input, float drive, float symmetry) noexcept;
    static float tubeSimulation(float input, float drive) noexcept;

    // Utility functions
    static float saturate(float input) noexcept;
    static float signedPower(float input, float exponent) noexcept;
};