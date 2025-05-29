cmake_minimum_required(VERSION 3.15)
project(WaveShaperCLI VERSION 1.0.0)
set(CMAKE_CXX_STANDARD 17)

# Same JUCE setup as the main plugin
include_directories(../JuceLibraryCode)
include_directories(/usr/share/juce/modules)
include_directories(/usr/include/freetype2)
include_directories(/usr/include/gtk-3.0)
include_directories(/usr/include/glib-2.0)
include_directories(/usr/lib/x86_64-linux-gnu/glib-2.0/include)
include_directories(/usr/include/pango-1.0)
include_directories(/usr/include/harfbuzz)
include_directories(/usr/include/cairo)
include_directories(/usr/include/gdk-pixbuf-2.0)
include_directories(/usr/include/atk-1.0)

# JUCE preprocessor definitions (minimal set for file processing)
add_compile_definitions(
    JUCE_GLOBAL_MODULE_SETTINGS_INCLUDED=1
    JUCE_MODULE_AVAILABLE_juce_audio_basics=1
    JUCE_MODULE_AVAILABLE_juce_audio_formats=1
    JUCE_MODULE_AVAILABLE_juce_core=1
    JUCE_MODULE_AVAILABLE_juce_data_structures=1
    JUCE_MODULE_AVAILABLE_juce_events=1
    JUCE_MODULE_AVAILABLE_juce_graphics=1
    JUCE_MODULE_AVAILABLE_juce_gui_basics=1
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_DISPLAY_SPLASH_SCREEN=0
)

# Source files
set(SOURCES
    WaveShaperCLI.cpp
    ../Source/WaveShapingEngine.cpp
    
    # Essential JUCE modules for file processing
    ../JuceLibraryCode/include_juce_audio_basics.cpp
    ../JuceLibraryCode/include_juce_audio_formats.cpp
    ../JuceLibraryCode/include_juce_core.cpp
    ../JuceLibraryCode/include_juce_data_structures.cpp
    ../JuceLibraryCode/include_juce_events.cpp
    ../JuceLibraryCode/include_juce_graphics.cpp
    ../JuceLibraryCode/include_juce_gui_basics.cpp
)

# Create executable
add_executable(waveshaper_cli ${SOURCES})

# Link libraries (minimal set)
target_link_libraries(waveshaper_cli PRIVATE 
    asound pthread dl freetype png z jpeg
    X11 Xext Xinerama Xrandr Xcursor Xrender Xcomposite
    fontconfig
    gtk-3 gdk-3 gobject-2.0 glib-2.0 
    cairo pango-1.0 pangocairo-1.0 gdk_pixbuf-2.0 atk-1.0
    FLAC vorbis vorbisenc vorbisfile ogg)
