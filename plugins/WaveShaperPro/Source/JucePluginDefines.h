#pragma once

// Plugin identification
#define JucePlugin_Build_VST 0
#define JucePlugin_Build_VST3 1
#define JucePlugin_Build_AU 1
#define JucePlugin_Build_AUv3 0
#define JucePlugin_Build_RTAS 0
#define JucePlugin_Build_AAX 0
#define JucePlugin_Build_Standalone 1

#define JucePlugin_Name "WaveShaper Pro"
#define JucePlugin_Desc "Professional Waveshaping Distortion Plugin"
#define JucePlugin_Manufacturer "YourCompany"
#define JucePlugin_ManufacturerWebsite ""
#define JucePlugin_ManufacturerEmail ""
#define JucePlugin_ManufacturerCode 0x594d4e75
#define JucePlugin_PluginCode 0x57535072
#define JucePlugin_IsSynth 0
#define JucePlugin_WantsMidiInput 0
#define JucePlugin_ProducesMidiOutput 0
#define JucePlugin_IsMidiEffect 0
#define JucePlugin_EditorRequiresKeyboardFocus 0
#define JucePlugin_Version 1.0.0
#define JucePlugin_VersionCode 0x10000
#define JucePlugin_VersionString "1.0.0"
#define JucePlugin_VSTUniqueID JucePlugin_PluginCode
#define JucePlugin_VSTCategory kPlugDistortion
#define JucePlugin_AUMainType 'aufx'
#define JucePlugin_AUSubType JucePlugin_PluginCode
#define JucePlugin_AUExportPrefix WaveShaperProAU
#define JucePlugin_AUExportPrefixQuoted "WaveShaperProAU"
#define JucePlugin_AUManufacturerCode JucePlugin_ManufacturerCode
#define JucePlugin_CFBundleIdentifier com.yourcompany.WaveShaperPro
#define JucePlugin_RTASCategory 2048
#define JucePlugin_RTASManufacturerCode JucePlugin_ManufacturerCode
#define JucePlugin_RTASProductId JucePlugin_PluginCode
#define JucePlugin_RTASDisableBypass 0
#define JucePlugin_RTASDisableMultiMono 0
#define JucePlugin_AAXIdentifier com.yourcompany.WaveShaperPro
#define JucePlugin_AAXManufacturerCode JucePlugin_ManufacturerCode
#define JucePlugin_AAXProductId JucePlugin_PluginCode
#define JucePlugin_AAXCategory 2048
#define JucePlugin_AAXDisableBypass 0
#define JucePlugin_AAXDisableMultiMono 0