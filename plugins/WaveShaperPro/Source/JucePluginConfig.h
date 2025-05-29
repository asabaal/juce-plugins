#pragma once

// Plugin Format Configuration
#define JucePlugin_Build_VST3                1
#define JucePlugin_Build_AU                  0
#define JucePlugin_Build_AUv3                0
#define JucePlugin_Build_AAX                 0
#define JucePlugin_Build_Standalone          1
#define JucePlugin_Build_Unity               0

// Plugin Information
#define JucePlugin_Name                      "WaveShaper Pro"
#define JucePlugin_Desc                      "Professional Waveshaping Distortion"
#define JucePlugin_Manufacturer              "YourCompany"
#define JucePlugin_ManufacturerWebsite       ""
#define JucePlugin_ManufacturerEmail         ""
#define JucePlugin_ManufacturerCode          0x594f5243 // 'YRCM'
#define JucePlugin_PluginCode                0x57535072 // 'WSPr'

// Plugin Characteristics
#define JucePlugin_IsSynth                   0
#define JucePlugin_WantsMidiInput            0
#define JucePlugin_ProducesMidiOutput        0
#define JucePlugin_IsMidiEffect              0
#define JucePlugin_EditorRequiresKeyboardFocus 0

// Version Information
#define JucePlugin_Version                   1.0.0
#define JucePlugin_VersionCode               0x10000
#define JucePlugin_VersionString             "1.0.0"

// Channel Configuration
#define JucePlugin_MaxNumInputChannels       2
#define JucePlugin_MaxNumOutputChannels      2
#define JucePlugin_PreferredChannelConfigurations {1, 1}, {2, 2}