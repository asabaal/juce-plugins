cmake_minimum_required(VERSION 3.15)
project(WaveShaperPro VERSION 1.0.0)
set(CMAKE_CXX_STANDARD 17)

# JUCE preprocessor definitions
add_compile_definitions(
    JUCE_GLOBAL_MODULE_SETTINGS_INCLUDED=1
    JUCE_MODULE_AVAILABLE_juce_audio_basics=1
    JUCE_MODULE_AVAILABLE_juce_audio_devices=1
    JUCE_MODULE_AVAILABLE_juce_audio_formats=1
    JUCE_MODULE_AVAILABLE_juce_audio_plugin_client=1
    JUCE_MODULE_AVAILABLE_juce_audio_processors=1
    JUCE_MODULE_AVAILABLE_juce_audio_utils=1
    JUCE_MODULE_AVAILABLE_juce_core=1
    JUCE_MODULE_AVAILABLE_juce_data_structures=1
    JUCE_MODULE_AVAILABLE_juce_dsp=1
    JUCE_MODULE_AVAILABLE_juce_events=1
    JUCE_MODULE_AVAILABLE_juce_graphics=1
    JUCE_MODULE_AVAILABLE_juce_gui_basics=1
    JUCE_MODULE_AVAILABLE_juce_gui_extra=1
    JUCE_STANDALONE_APPLICATION=0
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JucePlugin_Build_VST3=0
    JucePlugin_Build_Standalone=1
    JucePlugin_Build_AU=0
    JucePlugin_Build_VST=0
    JucePlugin_Name="AA WaveShaper Pro"
    JucePlugin_Desc="Professional Waveshaping Distortion Plugin"
    JucePlugin_Manufacturer="Asabaal Ventures"
    JucePlugin_ManufacturerCode=0x41564141
    JucePlugin_PluginCode=0x57735072
    JucePlugin_VersionString="1.0.0"
    JucePlugin_Version=1.0.0
    JucePlugin_IsSynth=0
    JucePlugin_WantsMidiInput=0
    JucePlugin_ProducesMidiOutput=0
    JucePlugin_IsMidiEffect=0
    JucePlugin_EditorRequiresKeyboardFocus=0
)

# Include JUCE legacy headers
include_directories(../../JuceLibraryCode)
# Include JUCE system headers
include_directories(/usr/include/juce)
include_directories(/usr/local/include/juce)
include_directories(/usr/share/juce/modules)

# Add FreeType include directories
include_directories(/usr/include/freetype2)
include_directories(/usr/include/freetype2/freetype)

# Add GTK include directories
include_directories(/usr/include/gtk-3.0)
include_directories(/usr/include/glib-2.0)
include_directories(/usr/lib/x86_64-linux-gnu/glib-2.0/include)
include_directories(/usr/include/pango-1.0)
include_directories(/usr/include/harfbuzz)
include_directories(/usr/include/cairo)
include_directories(/usr/include/gdk-pixbuf-2.0)
include_directories(/usr/include/atk-1.0)

# Add WebKit include directory
include_directories(/usr/include/webkitgtk-4.0)

# Source files
set(SOURCES
    WaveShaperProAudioProcessor.cpp
    WaveShaperProAudioProcessorEditor.cpp
    WaveShapingEngine.cpp
    
    # JUCE Library Code files
    ../../JuceLibraryCode/include_juce_audio_basics.cpp
    ../../JuceLibraryCode/include_juce_audio_devices.cpp
    ../../JuceLibraryCode/include_juce_audio_formats.cpp
    #../../JuceLibraryCode/include_juce_audio_plugin_client_VST3.cpp
    ../../JuceLibraryCode/include_juce_audio_plugin_client_Standalone.cpp
    ../../JuceLibraryCode/include_juce_audio_processors.cpp
    ../../JuceLibraryCode/include_juce_audio_utils.cpp
    ../../JuceLibraryCode/include_juce_core.cpp
    ../../JuceLibraryCode/include_juce_data_structures.cpp
    ../../JuceLibraryCode/include_juce_dsp.cpp
    ../../JuceLibraryCode/include_juce_events.cpp
    ../../JuceLibraryCode/include_juce_graphics.cpp
    ../../JuceLibraryCode/include_juce_gui_basics.cpp
    ../../JuceLibraryCode/include_juce_gui_extra.cpp
)

# Create executable/library target
add_executable(WaveShaperPro ${SOURCES})

# Platform-specific settings
if(WIN32)
    target_link_libraries(WaveShaperPro PRIVATE 
        winmm comctl32 comdlg32 imm32 ole32 oleaut32 rpcrt4 shell32 user32 uuid version wininet ws2_32)
elseif(UNIX AND NOT APPLE)
    target_link_libraries(WaveShaperPro PRIVATE 
        asound pthread dl freetype png z jpeg
        X11 Xext Xinerama Xrandr Xcursor Xrender Xcomposite
        fontconfig
        gtk-3 gdk-3 gobject-2.0 glib-2.0 
        cairo pango-1.0 pangocairo-1.0 gdk_pixbuf-2.0 atk-1.0
        # Audio format libraries
        FLAC vorbis vorbisenc vorbisfile ogg)
endif()

# Set output name
set_target_properties(WaveShaperPro PROPERTIES OUTPUT_NAME "AA WaveShaper")