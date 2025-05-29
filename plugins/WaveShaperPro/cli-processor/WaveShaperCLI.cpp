#include <JuceHeader.h>
#include "../Source/WaveShapingEngine.h"
#include <iostream>
#include <string>

class WaveShaperCLI
{
public:
    int run(int argc, char* argv[])
    {
        if (argc < 3) {
            printUsage();
            return 1;
        }
        
        juce::String inputFile = argv[1];
        juce::String outputFile = argv[2];
        
        // Parse options
        WaveShapeType curveType = WaveShapeType::SoftClip;
        float drive = 50.0f;
        float mix = 100.0f;
        float symmetry = 0.0f;
        float inputGain = 0.0f;
        float outputGain = 0.0f;
        
        parseArguments(argc, argv, curveType, drive, mix, symmetry, inputGain, outputGain);
        
        return processFile(inputFile, outputFile, curveType, drive, mix, symmetry, inputGain, outputGain);
    }
    
private:
    void printUsage()
    {
        std::cout << "WaveShaper CLI Processor\n";
        std::cout << "Usage: waveshaper_cli input.wav output.wav [options]\n\n";
        std::cout << "Options:\n";
        std::cout << "  --curve <type>    Curve type: soft, hard, asymmetric, tube (default: soft)\n";
        std::cout << "  --drive <0-100>   Drive amount (default: 50)\n";
        std::cout << "  --mix <0-100>     Dry/wet mix (default: 100)\n";
        std::cout << "  --symmetry <-100-100> Symmetry bias (default: 0)\n";
        std::cout << "  --input-gain <dB> Input gain in dB (default: 0)\n";
        std::cout << "  --output-gain <dB> Output gain in dB (default: 0)\n\n";
        std::cout << "Examples:\n";
        std::cout << "  waveshaper_cli input.wav output.wav --curve hard --drive 75\n";
        std::cout << "  waveshaper_cli input.wav output.wav --curve tube --drive 60 --mix 50\n";
    }
    
    void parseArguments(int argc, char* argv[], WaveShapeType& curveType, float& drive, 
                       float& mix, float& symmetry, float& inputGain, float& outputGain)
    {
        for (int i = 3; i < argc; i += 2) {
            if (i + 1 >= argc) break;
            
            juce::String option = argv[i];
            juce::String value = argv[i + 1];
            
            if (option == "--curve") {
                if (value == "soft") curveType = WaveShapeType::SoftClip;
                else if (value == "hard") curveType = WaveShapeType::HardClip;
                else if (value == "asymmetric") curveType = WaveShapeType::Asymmetric;
                else if (value == "tube") curveType = WaveShapeType::Tube;
            }
            else if (option == "--drive") drive = value.getFloatValue();
            else if (option == "--mix") mix = value.getFloatValue();
            else if (option == "--symmetry") symmetry = value.getFloatValue();
            else if (option == "--input-gain") inputGain = value.getFloatValue();
            else if (option == "--output-gain") outputGain = value.getFloatValue();
        }
    }
    
    int processFile(const juce::String& inputPath, const juce::String& outputPath,
                   WaveShapeType curveType, float drive, float mix, float symmetry,
                   float inputGain, float outputGain)
    {
        std::cout << "Processing: " << inputPath << " -> " << outputPath << "\n";
        std::cout << "Settings: ";
        std::cout << "Curve=" << getCurveTypeName(curveType);
        std::cout << ", Drive=" << drive << "%";
        std::cout << ", Mix=" << mix << "%";
        std::cout << ", Symmetry=" << symmetry << "%\n";
        
        // Initialize JUCE
        juce::AudioFormatManager formatManager;
        formatManager.registerBasicFormats();
        
        // Load input file
        juce::File inputFile(inputPath);
        if (!inputFile.exists()) {
            std::cerr << "Error: Input file does not exist: " << inputPath << "\n";
            return 1;
        }
        
        auto reader = std::unique_ptr<juce::AudioFormatReader>(formatManager.createReaderFor(inputFile));
        if (!reader) {
            std::cerr << "Error: Could not read input file: " << inputPath << "\n";
            return 1;
        }
        
        // Create buffer and read audio
        juce::AudioBuffer<float> buffer(static_cast<int>(reader->numChannels), 
                                       static_cast<int>(reader->lengthInSamples));
        reader->read(&buffer, 0, static_cast<int>(reader->lengthInSamples), 0, true, true);
        
        std::cout << "Loaded: " << reader->numChannels << " channels, " 
                  << reader->lengthInSamples << " samples, " 
                  << reader->sampleRate << " Hz\n";
        
        // Store dry signal for mixing if needed
        juce::AudioBuffer<float> dryBuffer;
        if (mix < 100.0f) {
            dryBuffer.makeCopyOf(buffer);
        }
        
        // Apply input gain
        if (inputGain != 0.0f) {
            float inputGainLinear = juce::Decibels::decibelsToGain(inputGain);
            buffer.applyGain(inputGainLinear);
        }
        
        // Process with WaveShapingEngine
        WaveShapingEngine engine;
        float driveNormalized = 1.0f + (drive / 100.0f) * 9.0f; // 1-10x drive
        float symmetryNormalized = symmetry / 100.0f; // -1 to +1
        
        engine.processBlock(buffer, curveType, driveNormalized, symmetryNormalized);
        
        // Apply output gain
        if (outputGain != 0.0f) {
            float outputGainLinear = juce::Decibels::decibelsToGain(outputGain);
            buffer.applyGain(outputGainLinear);
        }
        
        // Mix with dry signal if needed
        if (mix < 100.0f) {
            float mixNormalized = mix / 100.0f;
            for (int channel = 0; channel < buffer.getNumChannels(); ++channel) {
                for (int sample = 0; sample < buffer.getNumSamples(); ++sample) {
                    float wetSample = buffer.getSample(channel, sample);
                    float drySample = dryBuffer.getSample(channel, sample);
                    float mixedSample = drySample * (1.0f - mixNormalized) + wetSample * mixNormalized;
                    buffer.setSample(channel, sample, mixedSample);
                }
            }
        }
        
        // Save output file
        juce::File outputFile(outputPath);
        outputFile.deleteFile();
        
        auto outputStream = outputFile.createOutputStream();
        if (!outputStream) {
            std::cerr << "Error: Could not create output file: " << outputPath << "\n";
            return 1;
        }
        
        juce::WavAudioFormat wavFormat;
        auto writer = std::unique_ptr<juce::AudioFormatWriter>(
            wavFormat.createWriterFor(outputStream.release(), reader->sampleRate, 
                                    buffer.getNumChannels(), 24, {}, 0));
        
        if (!writer) {
            std::cerr << "Error: Could not create audio writer\n";
            return 1;
        }
        
        writer->writeFromAudioSampleBuffer(buffer, 0, buffer.getNumSamples());
        writer.reset();
        
        std::cout << "Successfully processed and saved: " << outputPath << "\n";
        return 0;
    }
    
    juce::String getCurveTypeName(WaveShapeType type)
    {
        switch (type) {
            case WaveShapeType::SoftClip: return "Soft Clip";
            case WaveShapeType::HardClip: return "Hard Clip";
            case WaveShapeType::Asymmetric: return "Asymmetric";
            case WaveShapeType::Tube: return "Tube";
            default: return "Unknown";
        }
    }
};

int main(int argc, char* argv[])
{
    juce::ScopedJuceInitialiser_GUI juceInitialiser;
    WaveShaperCLI cli;
    return cli.run(argc, argv);
}
